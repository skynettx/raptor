#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "common.h"
#include "gfxapi.h"

void GFX_ScaleLine(char *a1, char *a2)
{
    int i;
    for (i = tablelen - 1; i >= 0; i--)
    {
        *a1 = *(a2 + stable[i]);
        a1++;
    }
}

void GFX_CScaleLine(char *a1, char *a2)
{
    int i;
    for (i = tablelen - 1; i >= 0; i--)
    {
        char px = *(a2 + stable[i]);
        if (px)
            *a1 = px;
        a1++;
    }
}

void GFX_DisplayScreen(void)
{
    int i;
    char *src = &displaybuffer[ud_y * 320 + ud_x];
    char *dest = &displayscreen[ud_y * 320 + ud_x];
    if (ud_lx == 320)
    {
        memcpy(dest, src, 320 * ud_ly);
    }
    else
    {
        for (i = 0; i < ud_ly; i++)
        {
            memcpy(dest, src, ud_lx);
            dest += 320;
            src += 320;
        }
    }
    update_start = 0;
}

void GFX_ShadeSprite(char *p, texture_t *t, char *s)
{
    while ((int16_t)t->f_8 != -1)
    {
        char *d = p + (uint16_t)t->f_8;
        for (int i = 0; i < (uint16_t)t->width; i++, d++)
            *d = s[(uint8_t)*d];
        t = (texture_t*)((char*)&t->height + (uint16_t)t->width);
    }
}

void GFX_DrawSprite(char *a1, texture_t *a2)
{
    while ((int16_t)a2->f_8 != -1)
    {
        memcpy(a1 + (uint16_t)a2->f_8, (char*)&a2->height, (uint16_t)a2->width);
        a2 = (texture_t*)((char*)&a2->height + (uint16_t)a2->width);
    }
}

void GFX_DrawChar(char *a1, char *a2, int a3, int a4, int a5, int a6)
{
    do
    {
        for (int i = 0; i < a3; i++)
        {
            if (*a2)
                *a1 = a6 + *a2;
            a1++;
            a2++;
        }
        a2 += a5;
        a1 += 320 - a3;
    } while (--a4);
}

void GFX_Shade(char *a1, int a2, char *a3)
{
    for (int i = 0; i < a2; i++)
    {
        a1[i] = a3[(uint8_t)a1[i]];
    }
}

void GFX_PutPic(void)
{
    char *p = &displaybuffer[gfx_xp + gfx_yp * 320];
    char *src = gfx_inmem;
    for (int i = 0; i < gfx_ly; i++)
    {
        memcpy(p, src, gfx_lx);
        p += 320;
        src += gfx_lx + gfx_imga;
    }
}

void GFX_PutMaskPic(void)
{
    char *p = &displaybuffer[gfx_xp + gfx_yp * 320];
    char *src = gfx_inmem;
    for (int i = 0; i < gfx_ly; i++)
    {
        for (int j = 0; j < gfx_lx; j++)
        {
            if (src[j] != 0)
                p[j] = src[j];
        }
        p += 320;
        src += gfx_lx + gfx_imga;
    }
}

