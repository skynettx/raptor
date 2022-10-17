#include <string.h>
#include "common.h"
#include "rap.h"
#include "i_video.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "fx.h"
#include "imsapi.h"
#include "movie.h"
#include "kbdapi.h"
#include "windows.h"
#include "fileids.h"

movie_t frm[90];

/***************************************************************************
INTRO_City () - Shows City with planes flying into it
 ***************************************************************************/
int 
INTRO_City(
    void
)
{
    int loop;
    int maxframes = 30;
    int framecnt = maxframes - 1;
    movie_t *cur = frm;
    
    for (loop = 0; loop < maxframes; loop++, cur++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 8;
        cur->numframes = framecnt--;
        cur->item = FILE141_CHASE_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur->fx_vol = 0;
        cur->fx_xpos = 127;
        
        if (loop == 4)
        {
            cur->soundfx = FX_FLYBY;
            cur->fx_xpos = 210;
        }
        
        if (loop == 9)
        {
            cur->soundfx = FX_FLYBY;
            cur->fx_xpos = 100;
        }
    }
    
    cur = frm;
    
    cur->startf = M_FADEIN;
    cur->startsteps = 128;
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    SND_StopPatches();
    
    return 0;
}

/***************************************************************************
INTRO_Side1 () - Show Side OF Player ship going thru city
 ***************************************************************************/
int 
INTRO_Side1(
    void
)
{
    int loop;
    int maxframes = 20;
    int framecnt = maxframes - 1;
    movie_t *cur = frm;
    
    MOVIE_BPatch(FX_JETSND);
    
    for (loop = 0; loop < maxframes; loop++, cur++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 18;
        cur->numframes = framecnt--;
        cur->item = FILE15f_SHIPSD1_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->fx_vol = 127;
        cur->fx_xpos = 127;
        cur->soundfx = -1;
    }
    
    if (MOVIE_Play(frm, 2, palette) == K_SKIPALL)
        return 1;
    
    return 0;
}

/***************************************************************************
INTRO_Pilot () - Shows Pilots Face with lights moving thru
 ***************************************************************************/
int 
INTRO_Pilot(
    void
)
{
    int loop;
    int maxframes = 21;
    int framecnt = maxframes - 1;
    movie_t *cur = frm;
    
    MOVIE_BPatch(FX_IJETSND);
    
    for (loop = 0; loop < maxframes; loop++, cur++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 10;
        cur->numframes = framecnt--;
        cur->item = FILE173_PILOT_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur->fx_vol = 127;
        cur->fx_xpos = 127;
    }
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    return 0;
}

/***************************************************************************
INTRO_Explosion () - Bad Guy Blowing UP
 ***************************************************************************/
int 
INTRO_Explosion(
    void
)
{
    int loop;
    int maxframes = 22;
    int framecnt = maxframes - 1;
    movie_t *cur = frm;
    
    MOVIE_BPatch(FX_EJETSND);
    
    for (loop = 0; loop < maxframes; loop++, cur++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 12;
        cur->numframes = framecnt--;
        cur->item = FILE19c_EXPLO_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur->fx_vol = 127;
        cur->fx_xpos = 127;
        
        if (loop >= 2 && loop < 10)
        {
            cur->soundfx = FX_INTROHIT;
            cur->fx_xpos = 0x6e + (wrand() % 40);
        }
        
        if (loop >= 8)
        {
            if (loop & 1)
            {
                cur->soundfx = FX_AIREXPLO;
            }
        }
    }
    
    cur--;
    cur->soundfx = FX_AIREXPLO;
    cur->endf = M_FADEOUT;
    cur->red = 63;
    cur->green = 28;
    cur->blue = 3;
    cur->endsteps = 60;
    cur++;

    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    return 0;
}

/***************************************************************************
INTRO_Side2 () - Player Side flying thru city Shooting
 ***************************************************************************/
int 
INTRO_Side2(
    void
)
{
    int loop, opt;
    int maxframes = 20;
    int framecnt = maxframes - 1;
    movie_t *cur = frm;
    
    MOVIE_BPatch(FX_JETSND);
    
    for (loop = 0; loop < maxframes; loop++, cur++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 18;
        cur->numframes = framecnt--;
        cur->item = FILE15f_SHIPSD1_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur->fx_vol = 50;
        cur->fx_xpos = 127;
        cur->soundfx = -1;
    }
    
    opt = MOVIE_Play(frm, 1, palette);
    
    if (opt == K_SKIPALL)
        return 1;
    
    if (opt == K_NEXTFRAME)
        return 0;
    
    framecnt = maxframes - 1;
    cur = frm;
    
    for (loop = 0; loop < maxframes; loop++, cur++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 18;
        cur->numframes = framecnt--;
        cur->item = FILE188_SHIPSD2_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur->fx_xpos = 127;
        if (loop > 1)
            cur->soundfx = FX_INTROGUN;
    }
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    SND_StopPatches();
    
    return 0;
}

/***************************************************************************
INTRO_Base() - Base Landing at sunset
 ***************************************************************************/
int 
INTRO_Base(
    void
)
{
    int loop, maxframes, framecnt;
    movie_t *cur;
    maxframes = 30;
    framecnt = maxframes - 1;
    cur = frm;
    
    if (!GAME2)
        return 1;
    
    for (loop = 0; loop < maxframes; loop++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 10;
        cur->numframes = framecnt--;
        cur->item = FILE20e_BASE_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur++;
    }
    
    cur = frm;
    
    cur->startf = M_FADEIN;
    cur->startsteps = 128;
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    return 0;
}

/***************************************************************************
INTRO_Landing () - Ship Landing on Base
 ***************************************************************************/
int 
INTRO_Landing(
    void
)
{
    int loop, maxframes, framecnt;
    movie_t *cur;
    maxframes = 33;
    framecnt = maxframes - 1;
    cur = frm;
    
    for (loop = 0; loop < maxframes; loop++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 10;
        cur->numframes = framecnt--;
        cur->item = FILE1bd_LANDING_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur++;
    }
    
    cur--;
    cur->startf = M_FADEOUT;
    cur->startsteps = 64;
    cur->endf = M_PALETTE;
    cur->endsteps = 64;
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    return 0;
}

/***************************************************************************
INTRO_Death2 () - Ground Death Scene
 ***************************************************************************/
int 
INTRO_Death2(
    void
)
{
    int loop, maxframes, framecnt;
    movie_t *cur;
    maxframes = 6;
    framecnt = maxframes - 1;
    cur = frm;
    
    for (loop = 0; loop < maxframes; loop++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 3;
        cur->numframes = framecnt--;
        cur->item = FILE1b2_SDEATH_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur++;
    }
    
    if (MOVIE_Play(frm, 8, palette) == K_SKIPALL)
        return 1;
    
    GFX_FadeOut(0, 0, 0, 100);
    
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    
    return 0;
}

/***************************************************************************
INTRO_Death1 () - Air Death Scene
 ***************************************************************************/
int 
INTRO_Death1(
    void
)
{
    int loop, maxframes, framecnt;
    movie_t* cur;
    maxframes = 30;
    framecnt = maxframes - 1;
    cur = frm;
    
    for (loop = 0; loop < maxframes; loop++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 11;
        cur->numframes = framecnt--;
        cur->item = FILE1de_DOWN_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur++;
    }
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    return 0;
}

/***************************************************************************
INTRO_Death () - Death Scene
 ***************************************************************************/
int 
INTRO_Death(
    void
)
{
    if (INTRO_Death1())
        return 1;
    
    if (INTRO_Death2())
        return 1;
    
    return 0;
}

/***************************************************************************
INTRO_Game1End () - Game 1 Victory
 ***************************************************************************/
int 
INTRO_Game1End(
    void
)
{
    int loop, maxframes, framecnt;
    movie_t* cur;
    maxframes = 5;
    framecnt = maxframes - 1;
    cur = frm;
    
    for (loop = 0; loop < maxframes; loop++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 4;
        cur->numframes = framecnt--;
        cur->item = FILE1b8_GAME1END_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        cur++;
    }
    
    if (MOVIE_Play(frm, 8, palette) == K_SKIPALL)
        return 1;
    
    GFX_FadeOut(0, 0, 0, 120);
    
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    
    GFX_SetPalette(palette, 0);
    
    return 0;
}

/***************************************************************************
INTRO_Game2End () - Game 2 Victory
 ***************************************************************************/
int 
INTRO_Game2End(
    void
)
{
    int loop, maxframes, framecnt;
    movie_t* cur;
    maxframes = 25;
    framecnt = maxframes - 1;
    cur = frm;
    
    for (loop = 0; loop < maxframes; loop++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 4;
        cur->numframes = framecnt--;
        cur->item = FILE22c_GAME2END_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        
        if (loop == 22)
        {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 127;
        }
        
        if (loop == 24)
        {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 127;
        }
        cur++;
    }
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    GFX_FadeOut(0, 0, 0, 120);
    
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    
    GFX_SetPalette(palette, 0);
    
    return 0;
}

/***************************************************************************
INTRO_Game3End () - Game 3 Victory
 ***************************************************************************/
int 
INTRO_Game3End(
    void
)
{
    int loop, maxframes, framecnt;
    movie_t* cur;
    maxframes = 39;
    framecnt = maxframes - 1;
    cur = frm;
    
    MOVIE_BPatch(FX_JETSND);
    
    for (loop = 0; loop < maxframes; loop++)
    {
        cur->holdframe = 0;
        cur->opt = M_ANIM;
        cur->framerate = 8;
        cur->numframes = framecnt--;
        cur->item = FILE3c5_GAME3END_AGX + loop;
        cur->startf = M_NORM;
        cur->startsteps = 0;
        cur->endf = M_NORM;
        cur->endsteps = 0;
        cur->red = 0;
        cur->green = 0;
        cur->blue = 0;
        cur->songid = -1;
        cur->songopt = S_PLAY;
        cur->songstep = 0;
        cur->soundfx = -1;
        
        if (loop == 30)
        {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 100;
        }
        
        if (loop == 32)
        {
            cur->soundfx = FX_AIREXPLO;
            cur->fx_xpos = 100;
        }
        cur++;
    }
    
    if (MOVIE_Play(frm, 1, palette) == K_SKIPALL)
        return 1;
    
    GFX_FadeOut(0, 0, 0, 120);
    
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    
    GFX_SetPalette(palette, 0);
    
    return 0;
}

/***************************************************************************
INTRO_EndGame() - Ends the current game anims
 ***************************************************************************/
void 
INTRO_EndGame(
    int game
)
{
    IMS_StartAck();
    
    if (!gameflag[game])
        return;
    
    if (game > 2)
        game = 2;
    
    while (IMS_IsAck())
    {
    }
    IMS_StartAck();
    
    switch (game)
    {
    default:
    case 0:
        INTRO_Game1End();
        break;
    
    case 1:
        INTRO_Game2End();
        break;
    
    case 2:
        INTRO_Game3End();
        break;
    }
    
    while (IMS_IsAck())
    {
    }
    IMS_StartAck();
    INTRO_Base();
    
    while (IMS_IsAck())
    {
    }
    IMS_StartAck();
    INTRO_Landing();
    
    while (IMS_IsAck())
    {
    }
    IMS_StartAck();
    WIN_WinGame(game);
    
    if (game < 1)
        WIN_Order();
}

/***************************************************************************
INTRO_Taiwan (
 ***************************************************************************/
void 
INTRO_Taiwan(
    void
)
{
    int local_cnt, loop;
    texture_t *pic1;
    char *pal1;
    local_cnt = GFX_GetFrameCount();
    
    framecount = 0;
    
    pic1 = (texture_t*)GLB_GetItem(FILE12f_TAIWARN_PIC);
    pal1 = GLB_GetItem(FILE130_TAIPAL_DAT);
    
    GFX_FadeOut(0, 0, 0, 5);

    GFX_PutImage(pic1, 0, 0, 0);
    GFX_DisplayUpdate();
    
    GFX_FadeIn(pal1, 64);
    
    for (loop = 0; loop < 0x2d; loop++)
    {
        local_cnt = GFX_GetFrameCount();
        if (IMS_IsAck())
            break;
        while (GFX_GetFrameCount() - local_cnt < 4)
        {
        }
    }
    
    GFX_FadeOut(0, 0, 0, 63);
    
    GLB_FreeItem(FILE130_TAIPAL_DAT);
    GLB_FreeItem(FILE12f_TAIWARN_PIC);
}

/***************************************************************************
INTRO_Credits() - Credits Screen
 ***************************************************************************/
int 
INTRO_Credits(
    void
)
{
    texture_t *pic1, *pic2;
    char *pal1, *pal2;
    int local_cnt, loop;
    local_cnt = GFX_GetFrameCount();
    
    framecount = 0;
    
    pic1 = (texture_t*)GLB_GetItem(FILE12b_APOGEE_PIC);
    pal1 = GLB_GetItem(FILE12c_POGPAL_DAT);
    
    GFX_FadeOut(0, 0, 0, 5);
    
    GFX_PutImage(pic1, 0, 0, 0);
    GFX_DisplayUpdate();
    
    GFX_FadeIn(pal1, 64);
    
    if (bday_num != -1 && dig_flag)
        SND_Patch(FX_THEME, 127);
    else
        SND_PlaySong(FILE061_APOGEE_MUS, 0, 0);
    
    for (loop = 0; loop < 30; loop++)
    {
        local_cnt = GFX_GetFrameCount();
        if (IMS_IsAck())
            break;
        while (GFX_GetFrameCount() - local_cnt < 4)
        {
        }
    }
    
    if (bday_num != -1 && dig_flag)
    {
        while (SND_IsPatchPlaying(FX_THEME)) {
            I_GetEvent();
        }
    }
    else
    {
        while (SND_SongPlaying() && !IMS_IsAck()) {
            I_GetEvent();
        }
    }
    
    GFX_FadeOut(0, 0, 0, 63);
    
    GLB_FreeItem(FILE12c_POGPAL_DAT);
    GLB_FreeItem(FILE12b_APOGEE_PIC);
    
    SND_PlaySong(-1, 0, 1);
    
    memset(displayscreen, 0, 64000);
    memset(displaybuffer, 0, 64000);
    
    pic2 = (texture_t*)GLB_GetItem(FILE12d_CYGNUS_PIC);
    pal2 = GLB_GetItem(FILE12e_CYGPAL_DAT);
    
    GFX_PutImage(pic2, 0, 0, 0);
    GFX_DisplayUpdate();
    
    GFX_FadeIn(pal2, 64); 
    
    GLB_CacheItem(FILE056_RINTRO_MUS);
    
    for (loop = 0; loop < 65; loop++)
    {
        if (IMS_IsAck() && loop > 0)
            break;
        
        if (loop == 1 || loop == 40)
            SND_Patch(FX_BOSS1, 127);
        
        if (loop == 45)
            SND_PlaySong(FILE056_RINTRO_MUS, 1, 1);
        
        if (keyboard[SC_ESC])
            break;
        local_cnt = GFX_GetFrameCount();
        while (GFX_GetFrameCount() - local_cnt < 3)
        {
        }
    }
    
    GFX_FadeOut(0, 0, 0, 63);
    
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    
    GFX_SetPalette(palette, 0);
    
    GLB_FreeItem(FILE12e_CYGPAL_DAT);
    GLB_FreeItem(FILE12d_CYGNUS_PIC);
    
    IMS_StartAck();
    
    return 0;
}

/***************************************************************************
INTRO_BaseLanding() - BaseLanding PLays all needed MOVES
 ***************************************************************************/
void 
INTRO_BaseLanding(
    void
)
{
    if (!GAME2)
        return;
    
    if (INTRO_Base())
        return;
    if (INTRO_Landing())
        return;
}

/***************************************************************************
INTRO_PlayMain() - Plays Main Intro
 ***************************************************************************/
int 
INTRO_PlayMain(
    void
)
{
    if (INTRO_City())
        return 1;
    if (INTRO_Side1())
        return 1;
    if (INTRO_Pilot())
        return 1;
    if (INTRO_Side2())
        return 1;
    if (INTRO_Explosion())
        return 1;
    
    return 0;
}

