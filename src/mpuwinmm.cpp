#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>

#include "SDL.h"
#include "common.h"
#include "musapi.h"
#include "fx.h"

HMIDISTRM mpu_stream;
UINT mpu_device;
int mpu_init;
int mpu_initcnt;
int mpu_timer;

/***************************************************************************
MPU_Init() -
 ***************************************************************************/
int 
MPU_Init(
    int device
)
{
    MIDIOUTCAPS caps;

    if (mpu_initcnt++ != 0)
        return 1;

    mpu_device = winmm_mpu_device;

    if (mpu_device > midiOutGetNumDevs() || midiOutGetDevCaps(mpu_device, &caps, sizeof(caps)))
        mpu_device = MIDI_MAPPER;

    if (midiStreamOpen(&mpu_stream, &mpu_device, 1, 0, 0, 0))
    {
        return 0;
    }

    mpu_timer = SDL_GetTicks();
    mpu_init = 1;
    
    return 1;
}

/***************************************************************************
MPU_DeInit() -
 ***************************************************************************/
void 
MPU_DeInit(
    void
)
{
    if (--mpu_initcnt != 0)
        return;
    
    if (!mpu_init)
        return;

    midiStreamClose(mpu_stream);

    mpu_stream = NULL;
    mpu_init = 0;
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
    
    return chan + 1;
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
    if (!mpu_init)
        return;
    
    uint32_t data = 0;

    data = 0x80 | MPU_MapChannel(chan);
    data |= key << 8;
    data |= 64 << 16;

    midiOutShortMsg((HMIDIOUT)mpu_stream, data);
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
    if (!mpu_init)
        return;
    
    uint32_t data = 0;

    data = 0x90 | MPU_MapChannel(chan);
    data |= key << 8;
    data |= volume << 16;

    midiOutShortMsg((HMIDIOUT)mpu_stream, data);
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
    if (!mpu_init)
        return;
    
    static int event_map[] = {
        0, 0, 1, 7, 10, 11, 91, 93, 64, 67, 120, 123, -1, -1, 121, -1
    };
    uint32_t data = 0;

    if (event_map[controller] == -1)
        return;
    
    data = 0xb0 | MPU_MapChannel(chan);
    data |= event_map[controller] << 8;
    data |= param << 16;

    midiOutShortMsg((HMIDIOUT)mpu_stream, data);
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
    if (!mpu_init)
        return;
    
    uint32_t data = 0;
    
    bend <<= 6;

    data = 0xe0 | MPU_MapChannel(chan);
    data |= (bend & 127) << 8;
    data |= ((bend>>7)&127) << 16;

    midiOutShortMsg((HMIDIOUT)mpu_stream, data);
}

/***************************************************************************
ProgramEvent() -
 ***************************************************************************/
void 
ProgramEvent(
    unsigned int chan, 
    unsigned int param
)
{
    if (!mpu_init)
        return;
    
    uint32_t data = 0;

    data = 0xc0 | MPU_MapChannel(chan);
    data |= (param & 127) << 8;

    midiOutShortMsg((HMIDIOUT)mpu_stream, data);
}

/***************************************************************************
AllNotesOffEvent() -
 ***************************************************************************/
void 
AllNotesOffEvent(
    unsigned int chan, 
    unsigned int param
)
{
    if (!mpu_init)
        return;
    
    uint32_t data = 0;

    data = 0xB0 | MPU_MapChannel(chan);
    data |= 0x7B;

    midiOutShortMsg((HMIDIOUT)mpu_stream, data);
}

musdevice_t mus_device_winmm = {
    MPU_Init,
    MPU_DeInit,
    NULL,

    KeyOffEvent,
    KeyOnEvent,
    ControllerEvent,
    PitchBendEvent,
    ProgramEvent,
    AllNotesOffEvent,
};
#endif // _WIN32
