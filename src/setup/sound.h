#pragma once

extern int SoundCardType;
extern int SoundMidiPort;
extern int SoundBasePort;
extern int SoundIrq;
extern int SoundDma;
extern int SoundChannels;

void GetSoundChannels(TXT_UNCAST_ARG(widget), void* user_data);
void GetSoundDma(TXT_UNCAST_ARG(widget), void* user_data);
void GetSoundIrq(TXT_UNCAST_ARG(widget), void* user_data);
void GetSoundBasePort(TXT_UNCAST_ARG(widget), void* user_data);
void GetSoundcard(TXT_UNCAST_ARG(widget), void* user_data);
void GetSoundGMPort(TXT_UNCAST_ARG(widget), void* user_data);
