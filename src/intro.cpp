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

movie_t frm[90];

int INTRO_City(void)
{
    int i;
    int v20 = 30;
    int v24 = v20 - 1;
    movie_t *v28 = frm;
    for (i = 0; i < v20; i++, v28++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 8;
        v28->f_8 = v24--;
        v28->f_c = 0x10041 + i;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28->f_40 = 0;
        v28->f_44 = 127;
        if (i == 4)
        {
            v28->f_3c = 13;
            v28->f_44 = 210;
        }
        if (i == 9)
        {
            v28->f_3c = 13;
            v28->f_44 = 100;
        }
    }
    v28 = frm;
    v28->f_10 = 1;
    v28->f_14 = 128;
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    SND_StopPatches();
    return 0;
}

int INTRO_Side1(void)
{
    int i;
    int v20 = 20;
    int v24 = v20 - 1;
    movie_t *v28 = frm;
    MOVIE_BPatch(17);
    for (i = 0; i < v20; i++, v28++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 18;
        v28->f_8 = v24--;
        v28->f_c = 0x1005f + i;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_40 = 127;
        v28->f_44 = 127;
        v28->f_3c = -1;
    }
    if (MOVIE_Play(frm, 2, palette) == 2)
        return 1;
    return 0;
}

int INTRO_Pilot(void)
{
    int i;
    int v20 = 21;
    int v24 = v20 - 1;
    movie_t *v28 = frm;
    MOVIE_BPatch(24);
    for (i = 0; i < v20; i++, v28++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 10;
        v28->f_8 = v24--;
        v28->f_c = 0x10073 + i;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28->f_40 = 127;
        v28->f_44 = 127;
    }
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    return 0;
}

int INTRO_Explosion(void)
{
    int i;
    int v20 = 22;
    int v24 = v20 - 1;
    movie_t *v28 = frm;
    MOVIE_BPatch(25);
    for (i = 0; i < v20; i++, v28++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 12;
        v28->f_8 = v24--;
        v28->f_c = 0x1009c + i;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28->f_40 = 127;
        v28->f_44 = 127;
        if (i >= 2 && i < 10)
        {
            v28->f_3c = 26;
            v28->f_44 = 0x6e + (wrand() % 40);
        }
        if (i >= 8)
        {
            if (i & 1)
            {
                v28->f_3c = 8;
            }
        }
    }
    v28--;
    v28->f_3c = 8;
    v28->f_18 = 2;
    v28->f_20 = 63;
    v28->f_24 = 28;
    v28->f_28 = 3;
    v28->f_1c = 60;
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    return 0;
}

int INTRO_Side2(void)
{
    int i, r;
    int v20 = 20;
    int v24 = v20 - 1;
    movie_t *v28 = frm;
    MOVIE_BPatch(17);
    for (i = 0; i < v20; i++, v28++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 18;
        v28->f_8 = v24--;
        v28->f_c = 0x1005f + i;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28->f_40 = 50;
        v28->f_44 = 127;
        v28->f_3c = -1;
    }
    r = MOVIE_Play(frm, 1, palette);
    if (r == 2)
        return 1;
    if (r == 1)
        return 0;
    v24 = v20 - 1;
    v28 = frm;
    for (i = 0; i < v20; i++, v28++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 18;
        v28->f_8 = v24--;
        v28->f_c = 0x10088 + i;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28->f_44 = 127;
        if (i > 1)
            v28->f_3c = 27;
    }
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    SND_StopPatches();
    return 0;
}

int INTRO_Base(void)
{
    int v1c, v20, v24;
    movie_t *v28;
    v20 = 30;
    v24 = v20 - 1;
    v28 = frm;
    if (!gameflag[1])
        return 1;
    for (v1c = 0; v1c < v20; v1c++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 10;
        v28->f_8 = v24--;
        v28->f_c = 0x2000e + v1c;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28++;
    }
    v28 = frm;
    v28->f_10 = 1;
    v28->f_14 = 128;
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    return 0;
}

int INTRO_Landing(void)
{
    int v1c, v20, v24;
    movie_t *v28;
    v20 = 33;
    v24 = v20 - 1;
    v28 = frm;
    for (v1c = 0; v1c < v20; v1c++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 10;
        v28->f_8 = v24--;
        v28->f_c = 0x100bd + v1c;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28++;
    }
    v28 = frm;
    v28->f_10 = 1;
    v28->f_14 = 64;
    v28->f_18 = 3;
    v28->f_1c = 64;
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    return 0;
}

int INTRO_Death2(void)
{
    int v1c, v20, v24;
    movie_t *v28;
    v20 = 6;
    v24 = v20 - 1;
    v28 = frm;
    for (v1c = 0; v1c < v20; v1c++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 3;
        v28->f_8 = v24--;
        v28->f_c = 0x100b2 + v1c;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28++;
    }
    if (MOVIE_Play(frm, 8, palette) == 2)
        return 1;
    GFX_FadeOut(0, 0, 0, 100);
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    return 0;
}

int INTRO_Death1(void)
{
    int v1c, v20, v24;
    movie_t* v28;
    v20 = 30;
    v24 = v20 - 1;
    v28 = frm;
    for (v1c = 0; v1c < v20; v1c++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 11;
        v28->f_8 = v24--;
        v28->f_c = 0x100de + v1c;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28++;
    }
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    return 0;
}

int INTRO_Death(void)
{
    if (INTRO_Death1())
        return 1;
    if (INTRO_Death2())
        return 1;
    return 0;
}

int INTRO_Game1End(void)
{
    int v1c, v20, v24;
    movie_t* v28;
    v20 = 5;
    v24 = v20 - 1;
    v28 = frm;
    for (v1c = 0; v1c < v20; v1c++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 4;
        v28->f_8 = v24--;
        v28->f_c = 0x100b8 + v1c;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        v28++;
    }
    if (MOVIE_Play(frm, 8, palette) == 2)
        return 1;
    GFX_FadeOut(0, 0, 0, 120);
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    return 0;
}

int INTRO_Game2End(void)
{
    int v1c, v20, v24;
    movie_t* v28;
    v20 = 25;
    v24 = v20 - 1;
    v28 = frm;
    for (v1c = 0; v1c < v20; v1c++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 4;
        v28->f_8 = v24--;
        v28->f_c = 0x2002c + v1c;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        if (v1c == 22)
        {
            v28->f_3c = 8;
            v28->f_44 = 127;
        }
        if (v1c == 24)
        {
            v28->f_3c = 8;
            v28->f_44 = 127;
        }
        v28++;
    }
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    GFX_FadeOut(0, 0, 0, 120);
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    return 0;
}

int INTRO_Game3End(void)
{
    int v1c, v20, v24;
    movie_t* v28;
    v20 = 39;
    v24 = v20 - 1;
    v28 = frm;
    MOVIE_BPatch(17);
    for (v1c = 0; v1c < v20; v1c++)
    {
        v28->f_2c = 0;
        v28->f_0 = 0;
        v28->f_4 = 8;
        v28->f_8 = v24--;
        v28->f_c = 0x300c5 + v1c;
        v28->f_10 = 0;
        v28->f_14 = 0;
        v28->f_18 = 0;
        v28->f_1c = 0;
        v28->f_20 = 0;
        v28->f_24 = 0;
        v28->f_28 = 0;
        v28->f_30 = -1;
        v28->f_34 = 0;
        v28->f_38 = 0;
        v28->f_3c = -1;
        if (v1c == 30)
        {
            v28->f_3c = 8;
            v28->f_44 = 100;
        }
        if (v1c == 32)
        {
            v28->f_3c = 8;
            v28->f_44 = 100;
        }
        v28++;
    }
    if (MOVIE_Play(frm, 1, palette) == 2)
        return 1;
    GFX_FadeOut(0, 0, 0, 120);
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    return 0;
}

void INTRO_EndGame(int a1)
{
    IMS_StartAck();
    if (!gameflag[a1])
        return;
    if (a1 > 2)
        a1 = 2;
    while (IMS_IsAck())
    {
    }
    IMS_StartAck();
    switch (a1)
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
    WIN_WinGame(a1);
    if (a1 < 1)
        WIN_Order();
}

void INTRO_Taiwan(void)
{
    int v28, v1c;
    texture_t *v24;
    char *v20;
    v28 = GFX_GetFrameCount();
    framecount = 0;
    v24 = (texture_t*)GLB_GetItem(0x1002f);
    v20 = GLB_GetItem(0x10030);
    GFX_PutImage(v24, 0, 0, 0);
    GFX_DisplayUpdate();
    GFX_FadeIn(v20, 64);
    for (v1c = 0; v1c < 0x2d; v1c++)
    {
        v28 = GFX_GetFrameCount();
        if (IMS_IsAck())
            break;
        while (GFX_GetFrameCount() - v28 < 4)
        {
        }
    }
    GFX_FadeOut(0, 0, 0, 63);
    GLB_FreeItem(0x10030);
    GLB_FreeItem(0x1002f);
}

int INTRO_Credits(void)
{
    texture_t *apogee, *cyg;
    char *apogeepal, *cygpal;
    int now, i;
    now = GFX_GetFrameCount();
    framecount = 0;
    apogee = (texture_t*)GLB_GetItem(0x1002b);
    apogeepal = GLB_GetItem(0x1002c);
    GFX_FadeOut(0, 0, 0, 5);
    GFX_PutImage(apogee, 0, 0, 0);
    GFX_DisplayUpdate();
    GFX_FadeIn(apogeepal, 64);
    if (bday_num != -1 && dig_flag)
        SND_Patch(0, 127);
    else
        SND_PlaySong(97, 0, 0);
    for (i = 0; i < 30; i++)
    {
        now = GFX_GetFrameCount();
        if (IMS_IsAck())
            break;
        while (GFX_GetFrameCount() - now < 4)
        {
        }
    }
    if (bday_num != -1 && dig_flag)
    {
        while (SND_IsPatchPlaying(0)) {
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
    GLB_FreeItem(0x1002c);
    GLB_FreeItem(0x1002b);
    SND_PlaySong(-1, 0, 1);
    memset(displayscreen, 0, 64000);
    memset(displaybuffer, 0, 64000);
    cyg = (texture_t*)GLB_GetItem(0x1002d);
    cygpal = GLB_GetItem(0x1002e);
    GFX_PutImage(cyg, 0, 0, 0);
    GFX_DisplayUpdate();
    GFX_FadeIn(apogeepal, 64); //??
    GLB_CacheItem(0x56);
    for (i = 0; i < 65; i++)
    {
        if (IMS_IsAck() && i > 0)
            break;
        if (i == 1 || i == 40)
            SND_Patch(23, 127);
        if (i == 45)
            SND_PlaySong(0x56, 1, 1);
        if (keyboard[1])
            break;
        now = GFX_GetFrameCount();
        while (GFX_GetFrameCount() - now < 3)
        {
        }
    }
    GFX_FadeOut(0, 0, 0, 63);
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    GLB_FreeItem(0x1002e);
    GLB_FreeItem(0x1002d);
    IMS_StartAck();
    return 0;
}

void INTRO_BaseLanding(void)
{
    if (!gameflag[1])
        return;
    if (INTRO_Base())
        return;
    if (INTRO_Landing())
        return;
}

int INTRO_PlayMain(void)
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

