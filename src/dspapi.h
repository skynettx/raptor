#pragma once

struct dsp_t {
    int16_t format;
    int16_t freq;
    int32_t length;
    char data[1];
};

void DSP_Init(int channels, int freq);
void DSP_Mix(int16_t *buffer, int len);
int DSP_PatchIsPlaying(int handle);
int DSP_StartPatch(dsp_t* dsp, int sep, int pitch, int volume, int priority);
void DSP_StopPatch(int handle);
