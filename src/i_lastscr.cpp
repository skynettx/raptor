//
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
//    Exit text-mode ENDOOM screen.
//


#include <stdio.h>
#include <string.h>

#include "rap.h"
#include "i_lastscr.h"
#include "textscreen.h"
#include "i_video.h"
#include "joyapi.h"
#include "kbdapi.h"
#include "ptrapi.h"
#include "imsapi.h"

#define LASTSCR_W 80
#define LASTSCR_H 25


// 
// Displays the text mode ending screen after the game quits
//
void I_LASTSCR(void)
{
    unsigned char* screendata;
    int y;
    int indent;

    TXT_Fullscreen(txt_fullscreen);
    
    // Set up text mode screen

    TXT_Init();

    TXT_SetWindowTitle("Raptor");
    // SDL2-TODO I_InitWindowTitle();
    // SDL2-TODO I_InitWindowIcon();

    // Write the data to the screen memory

    screendata = TXT_GetScreenData();

    indent = (LASTSCR_W - TXT_SCREEN_W) / 2;

    for (y = 0; y < TXT_SCREEN_H; ++y)
    {
        memcpy(screendata + (y * TXT_SCREEN_W * 2),
            LASTSCR + (y * LASTSCR_W + indent) * 2,
            TXT_SCREEN_W * 2);
    }

    // Wait for a keypress

    IMS_StartAck();

    while (true)
    {
        TXT_UpdateScreen();
        I_GetEvent();

        if (joy_ack || kbd_ack || mouse_b1_ack || mouse_b2_ack || mouse_b3_ack)
            break;

        TXT_Sleep(0);
    }

    // Shut down text mode screen

    TXT_Shutdown();
}