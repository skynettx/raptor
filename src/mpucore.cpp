#ifdef __APPLE__
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "musapi.h"

enum {
    CAErr_Warning = -2,
    CAErr_Error   = -1,
    CAErr_Ok      = 0,
    CAErr_Uninitialised,
    CAErr_AssembleAUGraph,
    CAErr_InitialiseAUGraph,
    CAErr_SetPCMFormat,
    CAErr_Mutex,
    CAErr_SetSoundBank
};

enum {
    CASystem_none = 0,
    CASystem_pcm = 1,
    CASystem_midi = 2
};

static int ErrorCode = CAErr_Ok;
static int Initialised = CASystem_none;
static pthread_mutex_t pcmmutex, midimutex;
static AUGraph graph = 0;
static AudioUnit mixerunit, pcmunit, synthunit;
static unsigned int MidiFrameOffset = 0;
static unsigned int MidiTick = 0;
static unsigned int MidiFramesPerTick = 0;

static char *MixBuffer = 0;
static int MixBufferSize = 0;
static int MixBufferCount = 0;
static int MixBufferCurrent = 0;
static int MixBufferUsed = 0;
static void ( *MixCallBack )( void ) = 0;
static void ( *MidiCallBack)(void) = 0;

static char soundBankName[PATH_MAX+1] = "";

/***************************************************************************
_ASS_MessageOutputString() -
 ***************************************************************************/
static void 
_ASS_MessageOutputString(
    const char *str
)
{
   fputs(str, stderr);
}

void (*ASS_MessageOutputString)(const char *) = _ASS_MessageOutputString;

/***************************************************************************
ASS_Message() -
 ***************************************************************************/
void 
ASS_Message(
    const char *fmt, 
    ...
)
{
    char text[256];
    va_list va;

    va_start(va, fmt);
    vsnprintf(text, sizeof(text), fmt, va);
    va_end(va);
    ASS_MessageOutputString(text);
}

#define check_result(fcall, errval) \
if ((result = (fcall)) != noErr) {\
    ASS_Message("CoreAudioDrv: error %d at line %d:" #fcall "\n", (int)result, __LINE__);\
    ErrorCode = errval;\
    return CAErr_Error;\
}

/***************************************************************************
CoreAudioDrv_PCM_Unlock() -
 ***************************************************************************/
void 
CoreAudioDrv_PCM_Unlock(
    void
)
{
    if (!(Initialised & CASystem_pcm)) {
        return;
    }

    pthread_mutex_unlock(&pcmmutex);
}

/***************************************************************************
CoreAudioDrv_PCM_Lock() -
 ***************************************************************************/
void 
CoreAudioDrv_PCM_Lock(
    void
)
{
    if (!(Initialised & CASystem_pcm)) {
        return;
    }

    pthread_mutex_lock(&pcmmutex);
}

/***************************************************************************
CoreAudioDrv_MIDI_Lock() -
 ***************************************************************************/
void 
CoreAudioDrv_MIDI_Lock(
    void
)
{
    if (!(Initialised & CASystem_midi)) {
        return;
    }
    
    pthread_mutex_lock(&midimutex);
}

/***************************************************************************
CoreAudioDrv_MIDI_Unlock() -
 ***************************************************************************/
void 
CoreAudioDrv_MIDI_Unlock(
    void
)
{
    if (!(Initialised & CASystem_midi)) {
        return;
    }
    
    pthread_mutex_unlock(&midimutex);
}

/***************************************************************************
pcmService() -
 ***************************************************************************/
static 
OSStatus pcmService(
    void *inRefCon,
    AudioUnitRenderActionFlags *inActionFlags,
    const AudioTimeStamp *inTimeStamp,
    UInt32 inBusNumber,
    UInt32 inNumberFrames,
    AudioBufferList *ioData
)
{
    UInt32 remaining, len, bufn;
    char *ptr, *sptr;

    (void)inRefCon; (void)inActionFlags; (void)inTimeStamp; (void)inBusNumber; (void)inNumberFrames;

    if (MixCallBack == 0) return noErr;

    CoreAudioDrv_PCM_Lock();
    for (bufn = 0; bufn < ioData->mNumberBuffers; bufn++) {
        remaining = ioData->mBuffers[bufn].mDataByteSize;
        ptr = (char*) ioData->mBuffers[bufn].mData;

        while (remaining > 0) {
            if (MixBufferUsed == MixBufferSize) {
                MixCallBack();
                
                MixBufferUsed = 0;
                MixBufferCurrent++;
                if (MixBufferCurrent >= MixBufferCount) {
                    MixBufferCurrent -= MixBufferCount;
                }
            }
            
            while (remaining > 0 && MixBufferUsed < MixBufferSize) {
                sptr = MixBuffer + (MixBufferCurrent * MixBufferSize) + MixBufferUsed;
                
                len = MixBufferSize - MixBufferUsed;
                if (remaining < len) {
                    len = remaining;
                }
                
                memcpy(ptr, sptr, len);
                
                ptr += len;
                MixBufferUsed += len;
                remaining -= len;
            }
        }
    }
    CoreAudioDrv_PCM_Unlock();

    return noErr;
}

/***************************************************************************
midiService() -
 ***************************************************************************/
static 
OSStatus midiService(
    void *inRefCon,
    AudioUnitRenderActionFlags *ioActionFlags,
    const AudioTimeStamp *inTimeStamp,
    UInt32 inBusNumber,
    UInt32 inNumberFrames,
    AudioBufferList *ioData
)
{
    int secondsThisCall = (inNumberFrames << 16) / 44100;

    (void)inRefCon; (void)inTimeStamp; (void)inBusNumber; (void)ioData;

    if (MidiCallBack == 0) return noErr;
    
    if (!(*ioActionFlags & kAudioUnitRenderAction_PreRender)) return noErr;
    
    CoreAudioDrv_MIDI_Lock();
    while (MidiFrameOffset < inNumberFrames) {
        MidiCallBack();
        
        MidiTick++;
        MidiFrameOffset += MidiFramesPerTick;
    }
    MidiFrameOffset -= inNumberFrames;
    CoreAudioDrv_MIDI_Unlock();
    
    return noErr;
}

/***************************************************************************
CORE_Init() -
 ***************************************************************************/
int 
CORE_Init(
    int subsystem
)
{
    OSStatus result;
    AudioComponentDescription desc;
    AURenderCallbackStruct callback;
    AudioStreamBasicDescription pcmDesc;
    AUNode outputnode, mixernode, pcmnode, synthnode;

    memset(&pcmDesc, 0, sizeof(pcmDesc));
    
    if (Initialised) {
        Initialised |= subsystem;
        return CAErr_Ok;
    }
    
    if (pthread_mutex_init(&pcmmutex, 0) < 0) {
        ErrorCode = CAErr_Mutex;
        return CAErr_Error;
    }
    if (pthread_mutex_init(&midimutex, 0) < 0) {
        ErrorCode = CAErr_Mutex;
        return CAErr_Error;
    }
    
    // create the graph
    check_result(NewAUGraph(&graph), CAErr_AssembleAUGraph);
    
    // add the output node to the graph
    desc.componentType = kAudioUnitType_Output;
    desc.componentSubType = kAudioUnitSubType_DefaultOutput;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    check_result(AUGraphAddNode(graph, &desc, &outputnode), CAErr_AssembleAUGraph);

    // add the mixer node to the graph
    desc.componentType = kAudioUnitType_Mixer;
    desc.componentSubType = kAudioUnitSubType_StereoMixer;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    check_result(AUGraphAddNode(graph, &desc, &mixernode), CAErr_AssembleAUGraph);
    
    // add a node for PCM audio
    desc.componentType = kAudioUnitType_FormatConverter;
    desc.componentSubType = kAudioUnitSubType_AUConverter;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    check_result(AUGraphAddNode(graph, &desc, &pcmnode), CAErr_AssembleAUGraph);
    
    // add a node for a MIDI synth
    desc.componentType = kAudioUnitType_MusicDevice;
    desc.componentSubType = kAudioUnitSubType_DLSSynth;
    desc.componentManufacturer = kAudioUnitManufacturer_Apple;
    desc.componentFlags = 0;
    desc.componentFlagsMask = 0;
    check_result(AUGraphAddNode(graph, &desc, &synthnode), CAErr_AssembleAUGraph);
    
    // connect the nodes together
    check_result(AUGraphConnectNodeInput(graph, pcmnode, 0, mixernode, 0), CAErr_AssembleAUGraph);
    check_result(AUGraphConnectNodeInput(graph, synthnode, 0, mixernode, 1), CAErr_AssembleAUGraph);
    check_result(AUGraphConnectNodeInput(graph, mixernode, 0, outputnode, 0), CAErr_AssembleAUGraph);
    
    // open the nodes
    check_result(AUGraphOpen(graph), CAErr_AssembleAUGraph);

    // get the units
    check_result(AUGraphNodeInfo(graph, pcmnode, NULL, &pcmunit), CAErr_AssembleAUGraph);
    check_result(AUGraphNodeInfo(graph, synthnode, NULL, &synthunit), CAErr_AssembleAUGraph);
    check_result(AUGraphNodeInfo(graph, mixernode, NULL, &mixerunit), CAErr_AssembleAUGraph);
    
    // set the mixer bus count
    UInt32 buscount = 2;
    check_result(AudioUnitSetProperty(mixerunit,
                    kAudioUnitProperty_ElementCount,
                    kAudioUnitScope_Input,
                    0,
                    &buscount,
                    sizeof(buscount)), CAErr_InitialiseAUGraph);

    // set the pcm audio callback
    callback.inputProc = pcmService;
    callback.inputProcRefCon = 0;
    check_result(AUGraphSetNodeInputCallback(graph, pcmnode, 0, &callback), CAErr_InitialiseAUGraph);

    // set a default pcm audio format
    pcmDesc.mFormatID = kAudioFormatLinearPCM;
    pcmDesc.mFormatFlags = kLinearPCMFormatFlagIsPacked;
    pcmDesc.mChannelsPerFrame = 2;
    pcmDesc.mSampleRate = 44100;
    pcmDesc.mBitsPerChannel = 16;
    pcmDesc.mFormatFlags |= kLinearPCMFormatFlagIsSignedInteger;
#ifdef __POWERPC__
    pcmDesc.mFormatFlags |= kLinearPCMFormatFlagIsBigEndian;
#endif
    pcmDesc.mFramesPerPacket = 1;
    pcmDesc.mBytesPerFrame = pcmDesc.mBitsPerChannel * pcmDesc.mChannelsPerFrame / 8;
    pcmDesc.mBytesPerPacket = pcmDesc.mBytesPerFrame * pcmDesc.mFramesPerPacket;

    check_result(AudioUnitSetProperty(pcmunit,
                    kAudioUnitProperty_StreamFormat,
                    kAudioUnitScope_Input,
                    0,
                    &pcmDesc,
                    sizeof(pcmDesc)), CAErr_SetPCMFormat);

    // Set a sound bank for the DLS synth
    if (soundBankName[0]) {
        CFURLRef url = CFURLCreateFromFileSystemRepresentation(NULL,
                           (const UInt8 *)soundBankName, strlen(soundBankName), FALSE);
        if (url) {
            check_result(AudioUnitSetProperty(synthunit,
                            kMusicDeviceProperty_SoundBankURL,
                            kAudioUnitScope_Global,
                            0,
                            &url,
                            sizeof(url)), CAErr_SetSoundBank);
            CFRelease(url);
        }
    }

    // set the synth notify callback
    check_result(AudioUnitAddRenderNotify(synthunit, midiService, NULL), CAErr_InitialiseAUGraph);

    // initialise the graph
    check_result(AUGraphInitialize(graph), CAErr_InitialiseAUGraph);

    // start the graph
    check_result(AUGraphStart(graph), CAErr_InitialiseAUGraph);
    
    //CAShow(graph);
    
    Initialised |= subsystem;
    return CAErr_Ok;
}

/***************************************************************************
CORE_DeInit() -
 ***************************************************************************/
void 
CORE_DeInit(
    void
)
{
    OSStatus result;

    CoreAudioDrv_PCM_Lock();
    CoreAudioDrv_MIDI_Lock();
    
    // clean up the graph
    AUGraphStop(graph);
    DisposeAUGraph(graph);
    graph = 0;
    
    CoreAudioDrv_MIDI_Unlock();
    CoreAudioDrv_PCM_Unlock();
    
    pthread_mutex_destroy(&pcmmutex);
    pthread_mutex_destroy(&midimutex);
}

/***************************************************************************
MPU_MapChannel() -
 ***************************************************************************/
static unsigned int 
MPU_MapChannel(
    unsigned chan
)
{
    if (chan < 9)
        return chan;
    
    if (chan == 15)
        return 9;
    
    return chan +1;
}

/***************************************************************************
KeyOffEvent() -
 ***************************************************************************/
static void 
KeyOffEvent(
    unsigned int chan, 
    unsigned int key
)
{
    MusicDeviceMIDIEvent(synthunit,
                         0x80 | MPU_MapChannel(chan),
                         key,
                         0,
                         MidiFrameOffset);
}

/***************************************************************************
KeyOnEvent() -
 ***************************************************************************/
static void 
KeyOnEvent(
    int chan, 
    unsigned int key, 
    unsigned int volume
)
{
    MusicDeviceMIDIEvent(synthunit,
                         0x90 | MPU_MapChannel(chan),
                         key,
                         volume,
                         MidiFrameOffset);
}

/***************************************************************************
ProgramEvent() -
 ***************************************************************************/
static void 
ProgramEvent(
    unsigned int chan, 
    unsigned int param
) 
{
    MusicDeviceMIDIEvent(synthunit,
                         0xC0 | MPU_MapChannel(chan),
                         param,
                         0,
                         MidiFrameOffset);
}

/***************************************************************************
PitchBendEvent() -
 ***************************************************************************/
static void 
PitchBendEvent(
    unsigned int chan, 
    int bend
)
{
    int lsb;
    int msb;
   
    bend <<= 6;

    lsb = bend;
    msb = bend;   

    lsb &= 0x7F;
    msb >>= 7;
    
    MusicDeviceMIDIEvent(synthunit,
                         0xE0 | MPU_MapChannel(chan),
                         lsb,
                         msb,
                         MidiFrameOffset);
}

/***************************************************************************
AllNotesOffEvent() -
 ***************************************************************************/
static void 
AllNotesOffEvent(
    unsigned int chan, 
    unsigned int param
)
{
    MusicDeviceMIDIEvent(synthunit,
                         0x7B | chan,
                         param,
                         0,
                         MidiFrameOffset);
}

/***************************************************************************
ControllerEvent() -
 ***************************************************************************/
static void 
ControllerEvent(
    unsigned int chan, 
    unsigned int controller, 
    unsigned int param
)
{
    static int event_map[] = {
      0, 0, 1, 7, 10, 11, 91, 93, 64, 67, 120, 123, -1, -1, 121, -1
    };
    
    MusicDeviceMIDIEvent(synthunit,
                         0xB0 | MPU_MapChannel(chan),
                         event_map[controller],
                         param,
                         MidiFrameOffset); 
    
}

musdevice_t mus_device_core = {
    CORE_Init,
    CORE_DeInit,
    NULL,

    KeyOffEvent,
    KeyOnEvent,
    ControllerEvent,
    PitchBendEvent,
    ProgramEvent,
	AllNotesOffEvent
};    
#endif
