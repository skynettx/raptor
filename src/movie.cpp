#include <string.h>
#include "common.h"
#include "movie.h"
#include "fx.h"
#include "gfxapi.h"
#include "imsapi.h"
#include "glbapi.h"
#include "i_video.h"
#include "kbdapi.h"

int back_patch = -1;

void MOVIE_BPatch(int a1)
{
    back_patch = a1;
    SND_Patch(back_patch, 127);
}

void MOVIE_ShowFrame(movanim_t *a1)
{
    if (a1)
    {
        ANIM_Render(a1);
        GFX_MarkUpdate(0, 0, 320, 200);
    }
}

int MOVIE_Play(movie_t *a1, int a2, char *a3)
{
    int i, v28;
    int v18 = 1;
    int v20 = 0;
    char v14;
    char *v1c;
    movie_t *v24;
    memset(displaybuffer, 0, 64000);
    v24 = a1;
    IMS_StartAck();
    while (1)
    {
        I_GetEvent();
        v1c = (char *)GLB_GetItem(v24->f_c);
        if (v18)
        {
            v14 = v1c[0];
            memset(displaybuffer, v14, 64000);
            v1c++;
        }
        switch (v24->f_10)
        {
        case 0:
        default:
            MOVIE_ShowFrame((movanim_t*)v1c);
            GFX_WaitUpdate(v24->f_4);
            break;
        case 2:
            GFX_FadeOut(v24->f_20, v24->f_24, v24->f_28, v24->f_14);
            [[fallthrough]];
        case 4:
            memset(displaybuffer, 0, 64000);
            GFX_MarkUpdate(0, 0, 320, 200);
            GFX_DisplayUpdate();
            break;
        case 1:
            if (v18)
            {
                GFX_FadeOut(0, 0, 0, 2);
                v18 = 0;
            }
            MOVIE_ShowFrame((movanim_t*)v1c);
            GFX_WaitUpdate(v24->f_4);
            GFX_FadeIn(a3, v24->f_14);
            break;
        }
        if (v24->f_2c)
        {
            for (i = 0; i < v24->f_2c; i++)
            {
                v28 = GFX_GetFrameCount();
                while (GFX_GetFrameCount() == v28)
                {
                    if (!SND_IsPatchPlaying(back_patch))
                        SND_Patch(back_patch, 127);
                }
            }
        }
        else
        {
            if (back_patch != -1)
            {
                if (!SND_IsPatchPlaying(back_patch))
                    SND_Patch(back_patch, 127);
            }
        }
        if (v24->f_3c != -1)
        {
            SND_Patch(v24->f_3c, v24->f_44);
        }
        switch (v24->f_18)
        {
        case 4:
            memset(displaybuffer, 0, 64000);
            GFX_MarkUpdate(0, 0, 320, 200);
            GFX_DisplayUpdate();
            break;
        case 2:
            GFX_FadeOut(v24->f_20, v24->f_24, v24->f_28, v24->f_1c);
            break;
        case 1:
            GFX_FadeIn(a3, v24->f_14);
            break;
        case 3:
            GFX_SetPalette(a3, 0);
            break;
        }
        GLB_FreeItem(v24->f_c);
        if (!v24->f_8)
        {
            if (--a2)
            {
                memset(displaybuffer, 0, 64000);
                v24 = a1;
                v18 = 1;
            }
            else
                break;
        }
        else
        {
            if (IMS_CheckAck())
            {
                IMS_StartAck();
                v20 = 2;
                break;
            }
            v18 = 0;
            v24++;
        }
    }
    if (v20)
    {
        KBD_Clear();
        GFX_FadeOut(0, 0, 0, 16);
        memset(displaybuffer, 0, 64000);
        GFX_MarkUpdate(0, 0, 320, 200);
        GFX_DisplayUpdate();
        GFX_SetPalette(a3, 0);
    }
    if (back_patch != -1)
    {
        SND_StopPatch(back_patch);
        back_patch = -1;
    }
    return v20;
}
