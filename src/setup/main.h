#pragma once
#include "textscreen.h"

extern txt_window_t* infowindow;
extern txt_table_t* infotable;

extern char* textbox_control;
extern char* textbox_musiccard;
extern char* textbox_soundfx;

extern int keymoveup, keymovedown, keymoveleft, keymoveright, keyfire, keyspecial, keymega;

void MusicCard(TXT_UNCAST_ARG(widget), void* user_data);
void MainMenu(TXT_UNCAST_ARG(widget), void* user_data);
void FXCard(TXT_UNCAST_ARG(widget), void* user_data);
void ControlButtonConfig(TXT_UNCAST_ARG(widget), void* user_data);
void SaveSettings(TXT_UNCAST_ARG(widget), void* user_data);
void WindowSelect(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(window));
void ClosePwnBox(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(window));

