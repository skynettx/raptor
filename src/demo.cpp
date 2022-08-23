#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "common.h"
#include "demo.h"
#include "rap.h"
#include "loadsave.h"
#include "objects.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "fx.h"
#include "windows.h"
#include "intro.h"
#include "input.h"
#include "fileids.h"

#ifdef _MSC_VER
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif




char demo_name[PATH_MAX];
int cur_play;

demo_t playback[2801];

int max_play;
int demo_game;
int demo_wave;

void DEMO_MakePlayer(int a1)
{
    memset(&player, 0, sizeof(player));

    player.currentWeapon = -1;
    player.waveProgression[0] = 3;
    player.waveProgression[1] = 3;
    player.waveProgression[2] = 3;
    player.pilotPicId = 0;
    RAP_SetPlayerDiff();
    OBJS_Add(0);
    OBJS_Add(16);
    OBJS_Add(16);
    OBJS_Add(16);
    OBJS_Add(16);
    OBJS_Add(17);
    player.money = 10000;
    switch (a1)
    {
    default:
    case 0:
        OBJS_Add(2);
        OBJS_Add(11);
        OBJS_Add(4);
        OBJS_Add(7);
        OBJS_Add(5);
        OBJS_Add(14);
        break;
    case 1:
        OBJS_Add(11);
        OBJS_Add(11);
        OBJS_Add(11);
        OBJS_Add(11);
        OBJS_Add(15);
        OBJS_Add(1);
        OBJS_Add(2);
        OBJS_Add(5);
        OBJS_Add(8);
        player.money += 0x50003;
        break;
    case 2:
        OBJS_Add(11);
        OBJS_Add(11);
        OBJS_Add(11);
        OBJS_Add(11);
        OBJS_Add(15);
        OBJS_Add(1);
        OBJS_Add(2);
        OBJS_Add(13);
        OBJS_Add(8);
        player.money += 0xd5fff;
        break;
    }
    OBJS_GetNext();
}

void DEMO_DisplayStats(void)
{
    char buf[84];
    if (demo_mode == 1)
    {
        sprintf(buf, "REC %d", 0xaf0 - cur_play);
        GFX_Print(21, 20, buf, (font_t*)GLB_GetItem(FILE104_FONT2_FNT), 84);
    }
}

void DEMO_StartRec(void)
{
    SND_Patch(10, 127);
    memset(playback, 0, sizeof(playback));
    demo_mode = 1;
    cur_play = 1;
}

void DEMO_StartPlayback(void)
{
    demo_mode = 2;
    cur_play = 1;
}

void DEMO_SetFileName(const char *a1)
{
    strcpy(demo_name, a1);
}

void DEMO_GLBFile(int a1)
{
    char *demo = GLB_GetItem(a1);
    int size = GLB_GetItemSize(a1);
    if (demo)
    {
        memcpy(playback, demo, size);
        cur_play = 1;
        max_play = playback[0].f_8;
        demo_game = playback[0].f_4;
        demo_wave = playback[0].f_6;
        GLB_FreeItem(a1);
    }
}

void DEMO_LoadFile(void)
{
    GLB_ReadFile(demo_name, 0);
    GLB_ReadFile(demo_name, (char*)playback);
    cur_play = 1;
    max_play = playback[0].f_8;
    demo_game = playback[0].f_4;
    demo_wave = playback[0].f_6;
}

void DEMO_SaveFile(void)
{
    if (cur_play < 2)
        return;
    playback[0].f_4 = cur_game;
    playback[0].f_6 = game_wave[cur_game];
    playback[0].f_8 = cur_play;
    GLB_SaveFile(demo_name, (char*)playback, cur_play * sizeof(demo_t));
}

int DEMO_Play(void)
{
    int v1c;
    DEMO_StartPlayback();
    cur_game = demo_game;
    game_wave[cur_game] = demo_wave;
    DEMO_MakePlayer(demo_game);
    switch (cur_game)
    {
    case 0:
    default:
        SND_PlaySong(songsg1[demo_wave], 1, 1);
        break;
    case 1:
        SND_PlaySong(songsg2[demo_wave], 1, 1);
        break;
    case 2:
        SND_PlaySong(songsg3[demo_wave], 1, 1);
        break;
    }

    WIN_LoadComp();
    RAP_LoadMap();
    retraceflag = 0;
    v1c = Do_Game();
    retraceflag = 1;
    if (!v1c)
    {
        if (OBJS_GetAmt(16) <= 0)
        {
            SND_PlaySong(93, 1, 1);
            INTRO_Death();
        }
        else
            INTRO_Landing();
    }
    RAP_ClearPlayer();
    demo_mode = 0;
    return v1c;
}

int DEMO_Think(void)
{
    int v1c;

    v1c = 0;
    switch (demo_mode)
    {
    default:
    case 0:
        v1c = 0;
        break;
    case 2:
        buttons[0] = playback[cur_play].f_0;
        buttons[1] = playback[cur_play].f_1;
        buttons[2] = playback[cur_play].f_2;
        buttons[3] = playback[cur_play].f_3;
        playerx = playback[cur_play].f_4;
        playery = playback[cur_play].f_6;
        player_cx = playerx + 16;
        player_cy = playery + 16;
        playerpic = playback[cur_play].f_8;
        cur_play++;
        if (cur_play > max_play)
        {
            demo_mode = 0;
            v1c = 1;
        }
        break;
    case 1:
        playback[cur_play].f_0 = buttons[0];
        playback[cur_play].f_1 = buttons[1];
        playback[cur_play].f_2 = buttons[2];
        playback[cur_play].f_3 = buttons[3];
        playback[cur_play].f_4 = playerx;
        playback[cur_play].f_6 = playery;
        playback[cur_play].f_8 = playerpic;
        cur_play++;
        if (cur_play == 2800)
        {
            SND_Patch(10, 127);
            demo_mode = 0;
            v1c = 1;
        }
        break;
    }
    return v1c;
}
