#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common.h"
#include "gfxapi.h"

/*==========================================================================
   GFX_ScaleLine() - Does the scale from scale table ( _stable )
 ==========================================================================*/
void 
GFX_ScaleLine(
    char *outmem, 
    char *inmem
)
{
    int loop;
    
    for (loop = tablelen - 1; loop >= 0; loop--)
    {
        *outmem = *(inmem + stable[loop]);
        outmem++;
    }
}

/*==========================================================================
   GFX_CScaleLine() - scale from scale table (_stable) color 0 transparent 
 ==========================================================================*/
void 
GFX_CScaleLine(
    char *outmem, 
    char *inmem
)
{
    int loop;
    
    for (loop = tablelen - 1; loop >= 0; loop--)
    {
        char px = *(inmem + stable[loop]);
        
        if (px)
            *outmem = px;
        
        outmem++;
    }
}

/*==========================================================================
   GFX_DisplayScreen() - Puts Display Buffer into Video memory
 ==========================================================================*/
void 
GFX_DisplayScreen(
    void
)
{
    int loop;
    char *src = &displaybuffer[ud_y * SCREENWIDTH + ud_x];
    char *dest = &displayscreen[ud_y * SCREENWIDTH + ud_x];
    
    if (ud_lx == SCREENWIDTH)
    {
        memcpy(dest, src, SCREENWIDTH * ud_ly);
    }
    else
    {
        for (loop = 0; loop < ud_ly; loop++)
        {
            memcpy(dest, src, ud_lx);
            
            dest += SCREENWIDTH;
            
            src += SCREENWIDTH;
        }
    }
    
    update_start = 0;
}

/*==========================================================================
   GFX_ShadeSprite() -
 ==========================================================================*/
void 
GFX_ShadeSprite(
    char *dest, 
    texture_t *inmem, 
    char *dtable
)
{
    while ((int16_t)inmem->offset != -1)
    {
        char *d = dest + (uint16_t)inmem->offset;
        
        for (int loop = 0; loop < (uint16_t)inmem->width; loop++, d++)
            *d = dtable[(uint8_t)*d];
        
        inmem = (texture_t*)((char*)&inmem->height + (uint16_t)inmem->width);
    }
}

/*==========================================================================
   GFX_DrawSprite() -
 ==========================================================================*/
void 
GFX_DrawSprite(
    char *dest, 
    texture_t *inmem
)
{
    while ((int16_t)inmem->offset != -1)
    {
        memcpy(dest + (uint16_t)inmem->offset, (char*)&inmem->height, (uint16_t)inmem->width);
        
        inmem = (texture_t*)((char*)&inmem->height + (uint16_t)inmem->width);
    }
}

/*==========================================================================
   GFX_DrawChar() -
 ==========================================================================*/
void 
GFX_DrawChar(
    char *dest, 
    char *inmem, 
    int width, 
    int height, 
    int addx, 
    int color
)
{
    do
    {
        for (int loop = 0; loop < width; loop++)
        {
            if (*inmem)
                *dest = color + *inmem;
            
            dest++;
            inmem++;
        }
        
        inmem += addx;
        dest += SCREENWIDTH - width;
    
    } while (--height);
}

/*==========================================================================
   GFX_Shade() - Remaps Bytes according to shade table 
 ==========================================================================*/
void 
GFX_Shade(
    char *outmem, 
    int maxlen, 
    char *dtable
)
{
    for (int loop = 0; loop < maxlen; loop++)
    {
        outmem[loop] = dtable[(uint8_t)outmem[loop]];
    }
}

/*==========================================================================
   GFX_PutPic() - Puts Picture into buffer 
 ==========================================================================*/
void 
GFX_PutPic(
    void
)
{
    char *p = &displaybuffer[gfx_xp + gfx_yp * SCREENWIDTH];
    char *src = gfx_inmem;
    
    for (int loop = 0; loop < gfx_ly; loop++)
    {
        memcpy(p, src, gfx_lx);
        
        p += SCREENWIDTH;
        
        src += gfx_lx + gfx_imga;
    }
}

/*==========================================================================
   GFX_PutMaskPic() - Puts Picture into buffer with color 0 see thru
 ==========================================================================*/
void 
GFX_PutMaskPic(
    void
)
{
    char *p = &displaybuffer[gfx_xp + gfx_yp * SCREENWIDTH];
    char *src = gfx_inmem;
    
    for (int loop = 0; loop < gfx_ly; loop++)
    {
        for (int i = 0; i < gfx_lx; i++)
        {
            if (src[i] != 0)
                p[i] = src[i];
        }
        
        p += SCREENWIDTH;
        
        src += gfx_lx + gfx_imga;
    }
}

