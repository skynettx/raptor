#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "SDL.h"
#define TSF_IMPLEMENTATION
#include "tsf.h"

#include "musapi.h"

static tsf* g_TinySoundFont;

void AudioCallback(void* data, uint8_t* stream, int len)
{
    int SampleCount = (len / (2 * sizeof(short))); //2 output channels
    tsf_render_short(g_TinySoundFont, (short*)stream, SampleCount, 0);
}

int Init (int option)
{
    SDL_AudioSpec OutputAudioSpec;
    OutputAudioSpec.freq = 44100;
    OutputAudioSpec.format = AUDIO_S16SYS;
    OutputAudioSpec.channels = 2;
    OutputAudioSpec.samples = 512;
    OutputAudioSpec.callback = AudioCallback;
    
    // Load the SoundFont from a file
    g_TinySoundFont = tsf_load_filename("SoundFont.sf2");
    
    if (!g_TinySoundFont)
    {
        fprintf(stderr, "Could not load SoundFont.sf2\n");
        EXIT_Error("Could not load SoundFont.");
        return 0;
    }
    
    // Set the SoundFont rendering output mode
    tsf_set_output(g_TinySoundFont, TSF_STEREO_INTERLEAVED, OutputAudioSpec.freq, 0.0f);
    

    // Request the desired audio output format
    if (SDL_OpenAudio(&OutputAudioSpec, NULL) < 0)
    {
        fprintf(stderr, "Could not open the audio hardware or the desired audio output format\n");
        EXIT_Error("Could not open the audio hardware or the desired audio output format.");
        return 0;
    }
    return 1;
}

void DeInit(void) 
{
    tsf_close(g_TinySoundFont);
}

static unsigned int MPU_MapChannel(unsigned chan)
{
    if (chan < 9)
        return chan;
    if (chan == 15)
        return 9;
    return chan +1;
}

void KeyOffEvent(unsigned int chan, unsigned int key)
{
    tsf_channel_note_off(g_TinySoundFont, MPU_MapChannel(chan), key);
}

void KeyOnEvent(int chan, unsigned int key, unsigned int volume) 
{
    float velocity = (float)volume; 
    velocity /= 127;
  
    tsf_channel_note_on(g_TinySoundFont, MPU_MapChannel(chan), key, velocity);
}

void PitchBendEvent(unsigned int chan, int bend) 
{
    tsf_channel_set_pitchwheel(g_TinySoundFont, MPU_MapChannel(chan), bend);
}

void ProgramEvent(unsigned int chan, unsigned int param)
{
    tsf_channel_set_bank_preset(g_TinySoundFont, 9, 128, 0);
    tsf_channel_set_presetindex(g_TinySoundFont, MPU_MapChannel(chan), param);
}

void AllNotesOffEvent(unsigned int chan, unsigned int param)
{
    tsf_note_off_all(g_TinySoundFont);
}

void SetChannelVolume(unsigned int chan, unsigned int param)
{
    float volume = (float)param;
    volume /= 127;
   
    tsf_channel_set_volume(g_TinySoundFont, MPU_MapChannel(chan), volume);
}

void SetChannelPan(unsigned int chan, unsigned int param)
{

    float pan = (float)param;
    pan /= 127;
   
    tsf_channel_set_pan(g_TinySoundFont, MPU_MapChannel(chan), pan);
}

void ControllerEvent(unsigned int chan, unsigned int controller, unsigned int param)
{
    
    static int event_map[] = {          
      0, 0, 1, 7, 10, 11, 91, 93, 64, 67, 120, 123, -1, -1, 121, -1
    };
   
    switch (controller)
    {
    case 0:
        ProgramEvent(MPU_MapChannel(chan), param);
        break;
    case 3:
        SetChannelVolume(MPU_MapChannel(chan), param);
        break;
    case 4:
        SetChannelPan(MPU_MapChannel(chan), param);
        break;
    case 10:
    case 11:
        AllNotesOffEvent(MPU_MapChannel(chan), param);
        break;
    case 12:
    case 13:
        // TODO: mono/poly mode
        break;
    case 14:
        AllNotesOffEvent(MPU_MapChannel(chan), param);
        break;
    default:
        break;
    }
    
    tsf_channel_midi_control(g_TinySoundFont, MPU_MapChannel(chan), event_map[controller], param);
    SDL_PauseAudio(0);
}

musdevice_t mus_device_tsf = {
    Init,
    DeInit,
    NULL,

    KeyOffEvent,
    KeyOnEvent,
    ControllerEvent,
    PitchBendEvent,
};