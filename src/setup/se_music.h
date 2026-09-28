#pragma once

extern int MidiPort;
extern int CardType;
extern int BasePort;

void GetMusicBasePort(TXT_UNCAST_ARG(widget), void* user_data);
void GetMusiccard(TXT_UNCAST_ARG(widget), void* user_data);
void GetMusicGMPort(TXT_UNCAST_ARG(widget), void* user_data);