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

/***************************************************************************
DEMO_MakePlayer() -
 ***************************************************************************/
void 
DEMO_MakePlayer(
    int game
)
{
    memset(&player, 0, sizeof(player));

    player.sweapon = -1;
    player.diff[0] = DIFF_3;
    player.diff[1] = DIFF_3;
    player.diff[2] = DIFF_3;
    player.id_pic = 0;
    
    RAP_SetPlayerDiff();
    
    OBJS_Add(S_FORWARD_GUNS);
    OBJS_Add(S_ENERGY);
    OBJS_Add(S_ENERGY);
    OBJS_Add(S_ENERGY);
    OBJS_Add(S_ENERGY);
    OBJS_Add(S_DETECT);
    player.score = 10000;
    
    switch (game)
    {
    default:
    case 0:
        OBJS_Add(S_MICRO_MISSLE);
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_MINI_GUN);
        OBJS_Add(S_AIR_MISSLE);
        OBJS_Add(S_TURRET);
        OBJS_Add(S_DEATH_RAY);
        break;
    
    case 1:
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_SUPER_SHIELD);
        OBJS_Add(S_PLASMA_GUNS);
        OBJS_Add(S_MICRO_MISSLE);
        OBJS_Add(S_TURRET);
        OBJS_Add(S_GRD_MISSLE);
        player.score += 0x50003;
        break;
    
    case 2:
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_MEGA_BOMB);
        OBJS_Add(S_SUPER_SHIELD);
        OBJS_Add(S_PLASMA_GUNS);
        OBJS_Add(S_MICRO_MISSLE);
        OBJS_Add(S_FORWARD_LASER);
        OBJS_Add(S_GRD_MISSLE);
        player.score += 0xd5fff;
        break;
    }
    
    OBJS_GetNext();
}

/***************************************************************************
DEMO_DisplayStats (
 ***************************************************************************/
void 
DEMO_DisplayStats(
    void
)
{
    char temp[81];
    
    if (demo_mode != DEMO_RECORD)
        return;
    
    sprintf(temp, "REC %d", MAX_DEMO - cur_play);
    GFX_Print(MAP_LEFT + 5, 20, temp, (font_t*)GLB_GetItem(FILE104_FONT2_FNT), 84);
    
}

/***************************************************************************
DEMO_StartRec (
 ***************************************************************************/
void 
DEMO_StartRec(
    void
)
{
    SND_Patch(FX_BONUS, 127);
    memset(playback, 0, sizeof(playback));
    demo_mode = DEMO_RECORD;
    cur_play = 1;
}

/***************************************************************************
DEMO_StartPlayback (
 ***************************************************************************/
void 
DEMO_StartPlayback(
    void
)
{
    demo_mode = DEMO_PLAYBACK;
    cur_play = 1;
}

/***************************************************************************
DEMO_SetFileName ()
 ***************************************************************************/
void 
DEMO_SetFileName(
    const char *in_name
)
{
    strcpy(demo_name, in_name);
}

/***************************************************************************
DEMO_GLBFile (
 ***************************************************************************/
void 
DEMO_GLBFile(
    int item
)
{
    char *mem = GLB_GetItem(item);
    int size = GLB_GetItemSize(item);
    
    if (!mem)
        return;
    
    memcpy(playback, mem, size);
        
    cur_play = 1;
    
    max_play = playback[0].playerpic;
    demo_game = playback[0].px;
    demo_wave = playback[0].py;
        
    GLB_FreeItem(item);
}

/***************************************************************************
DEMO_LoadFile()
 ***************************************************************************/
void 
DEMO_LoadFile(
    void
)
{
    int filesize;

    filesize = GLB_ReadFile(demo_name, 0);
    GLB_ReadFile(demo_name, (char*)playback);
    
    cur_play = 1;
    
    max_play = playback[0].playerpic;
    demo_game = playback[0].px;
    demo_wave = playback[0].py;
}

/***************************************************************************
DEMO_SaveFile (
 ***************************************************************************/
void 
DEMO_SaveFile(
    void
)
{
    if (cur_play < 2)
        return;
    
    playback[0].px = cur_game;
    playback[0].py = game_wave[cur_game];
    playback[0].playerpic = cur_play;
    
    GLB_SaveFile(demo_name, (char*)playback, cur_play * sizeof(demo_t));
}

/***************************************************************************
DEMO_Play() - Demo playback routine
 ***************************************************************************/
int        // TRUE=Aborted, FALSE = timeout
DEMO_Play( 
    void
)
{
    int rval;

    rval = 0;

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
    
    rval = Do_Game();
    
    retraceflag = 1;
    
    if (!rval)
    {
        if (OBJS_GetAmt(S_ENERGY) <= 0)
        {
            SND_PlaySong(FILE05d_RAP5_MUS, 1, 1);
            INTRO_Death();
        }
        else
            INTRO_Landing();
    }
    
    RAP_ClearPlayer();
    
    demo_mode = DEMO_OFF;
    
    return rval;
}

/***************************************************************************
DEMO_Think (
 ***************************************************************************/
int 
DEMO_Think(
    void
)
{
    int rval;

    rval = 0;

    switch (demo_mode)
    {
    default:
    case 0:
        rval = 0;
        break;
    
    case DEMO_PLAYBACK:
        buttons[0] = playback[cur_play].b1;
        buttons[1] = playback[cur_play].b2;
        buttons[2] = playback[cur_play].b3;
        buttons[3] = playback[cur_play].b4;
        playerx = playback[cur_play].px;
        playery = playback[cur_play].py;
        player_cx = playerx + 16;
        player_cy = playery + 16;
        playerpic = playback[cur_play].playerpic;
        cur_play++;
        if (cur_play > max_play)
        {
            demo_mode = DEMO_OFF;
            rval = 1;
        }
        break;
    
    case DEMO_RECORD:
        playback[cur_play].b1 = buttons[0];
        playback[cur_play].b2 = buttons[1];
        playback[cur_play].b3 = buttons[2];
        playback[cur_play].b4 = buttons[3];
        playback[cur_play].px = playerx;
        playback[cur_play].py = playery;
        playback[cur_play].playerpic = playerpic;
        cur_play++;
        if (cur_play == MAX_DEMO)
        {
            SND_Patch(FX_BONUS, 127);
            demo_mode = 0;
            rval = 1;
        }
        break;
    }
    
    return rval;
}
