#pragma once
#include <stdint.h>
#include "common.h"

struct musdevice_t {
    int (*Init)(int option);
    void (*DeInit)(void);
    void (*Mix)(int16_t *stream, int len);
    
    void (*KeyOffEvent)(unsigned int chan, unsigned int key);
    void (*KeyOnEvent)(int chan, unsigned int key, unsigned int volume);
    void (*ControllerEvent)(unsigned int chan, unsigned int controller, unsigned int param);
    void (*PitchBendEvent)(unsigned int chan, int bend);
    void (*ProgramEvent)(unsigned int chan, unsigned int param);
    void (*AllNotesOffEvent)(unsigned int chan, unsigned int param);
};

extern musdevice_t mus_device_fm, mus_device_mpu, mus_device_tsf, mus_device_alsa, mus_device_core;
extern musdevice_t *music_device;


int MUS_Init(int card, int option);
void MUS_DeInit(void);
void MUS_PlaySong(void *ptr, int loop, int fadein);
void MUS_StopSong(int fadeout);
int MUS_SongPlaying(void);
void MUS_Mix(int16_t *stream, int len);
void MUS_Poll(void);
void MUS_SetVolume(int volume);
