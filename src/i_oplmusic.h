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
//	The not so system specific sound interface.
//
#pragma once
// DMX version to emulate for OPL emulation:
typedef enum {
    opl_doom1_1_666,    // Doom 1 v1.666
    opl_doom2_1_666,    // Doom 2 v1.666, Hexen, Heretic
    opl_rap12        // Doom v1.9, Strife
} opl_driver_ver_t;

void LoadInstrumentTable(char *genmidi);
void I_OPL_PlaySong(void);
void I_OPL_PauseSong(void);
void I_OPL_ResumeSong(void);
void I_OPL_StopSong(void);
void I_OPL_ShutdownMusic(void);
int I_OPL_InitMusic(int dummy);
void I_SetOPLDriverVer(opl_driver_ver_t ver);
void I_OPL_SetMusicVolume(int volume);
