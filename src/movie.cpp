#include <string.h>
#include "common.h"
#include "rap.h"
#include "movie.h"
#include "fx.h"
#include "gfxapi.h"
#include "imsapi.h"
#include "glbapi.h"
#include "i_video.h"
#include "kbdapi.h"

int back_patch = -1;

/*************************************************************************
MOVIE_BPatch() Plays Sound FX in background for one anim
 *************************************************************************/
void 
MOVIE_BPatch(
    int soundfx
)
{
    back_patch = soundfx;
    SND_Patch(back_patch, 127);
}

/*************************************************************************
 MOVIE_ShowFrame () - Shows an animation frame
 *************************************************************************/
void 
MOVIE_ShowFrame(
    movanim_t *inpic       // INPUT : pointer to animpic
)
{
    if (inpic)
    {
        ANIM_Render(inpic);
        GFX_MarkUpdate(0, 0, 320, 200);
    }
}

/*************************************************************************
 MOVIE_Play () - Playes an Animation
 *************************************************************************/
int 
MOVIE_Play(
    movie_t *frame,         // INPUT : pointer to array of frame defs     
    int numplay,            // INPUT : number of times to play
    char *palette           // INPUT : pointer to palette
)
{
    int loop, hold;
    int flag = 1;
    int opt = K_OK;
    char fill;
    char *pic;
    movie_t *curfld;
    
    memset(displaybuffer, 0, 64000);
    
    curfld = frame;
    
    IMS_StartAck();
    
    while (1)
    {
        I_GetEvent();
        pic = (char *)GLB_GetItem(curfld->item);
        
        if (flag)
        {
            fill = pic[0];
            memset(displaybuffer, fill, 64000);
            pic++;
        }
        
        switch (curfld->startf)
        {
        case M_NORM:
        default:
            MOVIE_ShowFrame((movanim_t*)pic);
            GFX_WaitUpdate(curfld->framerate);
            break;
        
        case M_FADEOUT:
            GFX_FadeOut(curfld->red, curfld->green, curfld->blue, curfld->startsteps);
            [[fallthrough]];
        case M_ERASE:
            memset(displaybuffer, 0, 64000);
            GFX_MarkUpdate(0, 0, 320, 200);
            GFX_DisplayUpdate();
            break;
        
        case M_FADEIN:
            if (flag)
            {
                GFX_FadeOut(0, 0, 0, 2);
                flag = 0;
            }
            MOVIE_ShowFrame((movanim_t*)pic);
            GFX_WaitUpdate(curfld->framerate);
            GFX_FadeIn(palette, curfld->startsteps);
            break;
        }
        
        if (curfld->holdframe)
        {
            for (loop = 0; loop < curfld->holdframe; loop++)
            {
                hold = GFX_GetFrameCount();
                while (GFX_GetFrameCount() == hold)
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
        
        if (curfld->soundfx != -1)
        {
            SND_Patch(curfld->soundfx, curfld->fx_xpos);
        }
        
        switch (curfld->endf)
        {
        case M_ERASE:
            memset(displaybuffer, 0, 64000);
            GFX_MarkUpdate(0, 0, 320, 200);
            GFX_DisplayUpdate();
            break;
        
        case M_FADEOUT:
            GFX_FadeOut(curfld->red, curfld->green, curfld->blue, curfld->endsteps);
            break;
        
        case M_FADEIN:
            GFX_FadeIn(palette, curfld->startsteps);
            break;
        
        case M_PALETTE:
            GFX_SetPalette(palette, 0);
            break;
        }
        
        GLB_FreeItem(curfld->item);
        
        if (!curfld->numframes)
        {
            if (--numplay)
            {
                memset(displaybuffer, 0, 64000);
                curfld = frame;
                flag = 1;
            }
            else
                break;
        }
        else
        {
            if (IMS_CheckAck())
            {
                IMS_StartAck();
                opt = K_SKIPALL;
                break;
            }
            
            flag = 0;
            
            curfld++;
        }
    }
    
    if (opt)
    {
        KBD_Clear();
        GFX_FadeOut(0, 0, 0, 16);
        memset(displaybuffer, 0, 64000);
        GFX_MarkUpdate(0, 0, 320, 200);
        GFX_DisplayUpdate();
        GFX_SetPalette(palette, 0);
    }
    
    if (back_patch != -1)
    {
        SND_StopPatch(back_patch);
        back_patch = -1;
    }
    
    return opt;
}
