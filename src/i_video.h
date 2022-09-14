//
// Copyright(C) 1993-1996 Id Software, Inc.
// Copyright(C) 2005-2014 Simon Howard
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// DESCRIPTION:
//	System specific interface stuff.
//
#pragma once

#include <stdint.h>

// Screen width and height.

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

// Screen height used when aspect_ratio_correct=true.

#define SCREENHEIGHT_4_3 240

typedef uint8_t pixel_t;
typedef bool (*grabmouse_callback_t)(void);

// Screen width and height, from configuration file.

extern int window_width;
extern int window_height;
extern int txt_fullscreen;

extern pixel_t *I_VideoBuffer;

void I_InitGraphics(uint8_t *pal);
void I_ShutdownGraphics(void);
void I_FinishUpdate (void);
void I_GetEvent(void);
void I_SetPalette(uint8_t *doompalette, int start = 0);
void I_GetPalette(uint8_t *pal);
void I_GetMousePos(int *x, int *y);
void I_SetMousePos(int x, int y);
void I_SetGrabMouseCallback(grabmouse_callback_t func);
void VIDEO_LoadPrefs(void);
void closewindow(void); 
