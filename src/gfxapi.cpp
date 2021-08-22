#include <string.h>
#include <stdint.h>
#include "SDL.h"
#include "common.h"
#include "gfxapi.h"
#include "i_video.h"

char *displaybuffer;
char *displayscreen;

int framecount;

int ylookup[200];

char *ltable, *dtable, *gtable;
char tpal1[768], tpal2[768];
int start_lookup = 0;
int end_lookup = 255;
char *gfx_inmem;
int gfx_xp, gfx_yp;
int gfx_lx, gfx_ly;
int gfx_imga;

int tablelen;
int stable[324];

int ud_x, ud_y, ud_lx, ud_ly;
int o_ud_x, o_ud_y, o_ud_lx, o_ud_ly;
int update_start;

void (*framehook)(void (*call)(void));

static int timer_init = 0;

int retraceflag = 1;

int G3D_viewx, G3D_viewy, G3D_viewz;
int G3D_x, G3D_y, G3D_z, G3D_screenx, G3D_screeny;

void GFX_InitTimer(void)
{
#if SDL_VERSION_ATLEAST(2, 0, 5)
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
#endif
    SDL_Init(SDL_INIT_TIMER);
    timer_init = 1;
}

#define GFX_RATE 70

void GFX_UpdateTimer(void)
{
    if (!timer_init)
        return;
    static int last;
    static int accm;
    int now = SDL_GetTicks();
    if (last == 0)
        last = now;
    for (int i = last; i < now;)
    {
        int step = (1000 - accm + (GFX_RATE - 1)) / GFX_RATE;
        if (step < 1)
            step = 1;
        if (step > now - i)
            step = now - i;
        accm += GFX_RATE * step;
        while (accm >= 1000)
        {
            accm -= 1000;
            framecount++;
        }
        i += step;
    }
    last = now;
}

int GFX_GetFrameCount(void)
{
    I_GetEvent();
    GFX_UpdateTimer();
    return framecount;
}

int GFX_ClipLines(char **a1, int *a2, int *a3, int *a4, int *a5)
{
    int vsi = 1;
    if (*a2 >= 320)
        return 0;
    if (*a3 >= 200)
        return 0;
    if (*a2 + *a4 <= 0)
        return 0;
    if (*a3 + *a5 <= 0)
        return 0;
    if (*a3 < 0)
    {
        vsi = 2;
        if (a1)
            *a1 += -(*a3) * *a4;
        *a5 += *a3;
        *a3 = 0;
    }
    if (*a3 + *a5 > 200)
    {
        vsi = 2;
        *a5 = 200 - *a3;
    }
    if (*a2 < 0)
    {
        vsi = 2;
        if (a1)
            *a1 += -(*a2);
        *a4 += *a2;
        *a2 = 0;
    }
    if (*a2 + *a4 > 320)
    {
        vsi = 2;
        *a4 = 320 - *a2;
    }
    return vsi;
}

void GFX_SetPalette(char *a1, int a2)
{
#if 0
    if (retraceflag)
    {
        I_GetEvent();
        GFX_UpdateTimer();
        int now = framecount;
        while (now == framecount)
            GFX_UpdateTimer();
    }
#endif
    I_SetPalette((uint8_t*)a1, a2);
}

void GFX_InitSystem(void)
{
    displaybuffer = (char*)malloc(64000);
    if (!displaybuffer)
    {
        EXIT_Error("GFX_Init() - malloc");
        return;
    }
    memset(displaybuffer, 0, 64000);
    GFX_InitTimer();
    for (int i = 0; i < 200; i++)
        ylookup[i] = i * 320;
    ltable = (char*)malloc(256);
    dtable = (char*)malloc(256);
    gtable = (char*)malloc(256);
    if (!ltable || !dtable || !gtable)
    {
        EXIT_Error("GFX_Init() - malloc");
        return;
    }
}

void GFX_InitVideo(char *pal)
{
    I_InitGraphics((uint8_t*)pal);
    displayscreen = (char*)I_VideoBuffer;
    GFX_MakeLightTable(pal, ltable, 9);
    GFX_MakeLightTable(pal, dtable, -9);
    GFX_MakeGreyTable(pal, gtable);
}

void GFX_EndSystem(void)
{
    I_ShutdownGraphics();
}

void GFX_GetPalette(char* a1)
{
    I_GetPalette((uint8_t*)a1);
}

void GFX_FadeOut(int a1, int a2, int a3, int a4)
{
    char pal[768];
    char pal2[768];
    int i, j;
    GFX_GetPalette(pal);
    memcpy(pal2, pal, 768);
    int now = GFX_GetFrameCount();
    i = 0;
    while (GFX_GetFrameCount() - now < a4)
    {
        if (GFX_GetFrameCount() - now != i)
        {
            i = GFX_GetFrameCount() - now;
            for (j = 0; j < 256; j++)
            {
                pal2[j * 3 + 0] = i * (a1 - pal[j * 3 + 0]) / a4 + pal[j * 3 + 0];
                pal2[j * 3 + 1] = i * (a2 - pal[j * 3 + 1]) / a4 + pal[j * 3 + 1];
                pal2[j * 3 + 2] = i * (a3 - pal[j * 3 + 2]) / a4 + pal[j * 3 + 2];
            }
            GFX_SetPalette(pal2, 0);
            I_FinishUpdate();
        }
    }
    for (i = 0; i < 256; i++)
    {
        pal2[i * 3 + 0] = a1;
        pal2[i * 3 + 1] = a2;
        pal2[i * 3 + 2] = a3;
    }

    GFX_SetPalette(pal2, 0);
    I_FinishUpdate();
}

void GFX_FadeIn(char *a1, int a2)
{
    char pal[768];
    char pal2[768];
    int i, j;
    GFX_GetPalette(pal);
    memcpy(pal2, pal, 768);
    int now = GFX_GetFrameCount();
    i = 0;
    while (GFX_GetFrameCount() - now < a2)
    {
        if (GFX_GetFrameCount() - now != i)
        {
            i = GFX_GetFrameCount() - now;
            for (j = 0; j < 768; j++)
            {
                pal2[j] = i * (a1[j] - pal[j]) / a2 + pal[j];
            }
            GFX_SetPalette(pal2, 0);
            I_FinishUpdate();
        }
    }

    GFX_SetPalette(a1, 0);
    I_FinishUpdate();
}

void GFX_FadeStart(void)
{
    GFX_GetPalette(tpal1);
    memcpy(tpal2, tpal1, 768);
}

void GFX_FadeFrame(char* a1, int a2, int a3)
{
    int i;
    for (i = 0; i < 768; i++)
    {
        tpal2[i] = ((a1[i] - tpal1[i]) * a2) / a3 + tpal1[i];
    }
    GFX_SetPalette(tpal2, 0);
}

void GFX_SetPalRange(int a1, int a2)
{
    if (a1 < a2 && a2 < 256 && a1 >= 0)
    {
        start_lookup = a1;
        end_lookup = a2;
    }
}

void GFX_GetRGB(char *a1, int a2, int *a3, int *a4, int *a5)
{
    *a3 = a1[a2 * 3 + 0];
    *a4 = a1[a2 * 3 + 1];
    *a5 = a1[a2 * 3 + 2];
}

int GFX_Remap(char *a1, int a2, int a3, int a4)
{
    int i, r, g, b, d;
    int v5 = 769;
    int v10 = 0;
    for (i = start_lookup; i < end_lookup + 1; i++)
    {
        GFX_GetRGB(a1, i, &r, &g, &b);
        d = abs(r - a2) + abs(g - a3) + abs(b - a4);
        if (d <= v5)
        {
            v5 = d;
            v10 = i;
        }
    }
    return v10;
}

void GFX_MakeLightTable(char *a1, char *a2, int a3)
{
    int i, r, g, b;
    for (i = 0; i < 256; i++)
    {
        GFX_GetRGB(a1, i, &r, &g, &b);
        if (r >= 0)
            r += a3;
        else
            r = 0;
        if (g >= 0)
            g += a3;
        else
            g = 0;
        if (b >= 0)
            b += a3;
        else
            b = 0;
        if (a3 >= 0)
        {
            if (r > 63)
                r = 63;
            if (g > 63)
                g = 63;
            if (b > 63)
                b = 63;
        }
        else
        {
            if (r < 0)
                r = 0;
            if (g < 0)
                g = 0;
            if (b < 0)
                b = 0;
        }
        a2[i] = GFX_Remap(a1, r, g, b);
    }
}

void GFX_MakeGreyTable(char *a1, char *a2)
{
    int i, c, r, g, b;
    for (i = 0; i < 256; i++)
    {
        GFX_GetRGB(a1, i, &r, &g, &b);
        c = (r + g + b) / 3;
        a2[i] = GFX_Remap(a1, c, c, c);
    }
}

void GFX_GetScreen(char *a1, int a2, int a3, int a4, int a5)
{
    char *p;
    int i;
    if (!GFX_ClipLines(&a1, &a2, &a3, &a4, &a5))
        return;
    p = &displaybuffer[ylookup[a3] + a2];
    for (i = 0; i < a5; i++)
    {
        memcpy(a1, p, a4);
        a1 += a4;
        p += 320;
    }
}

void GFX_PutTexture(texture_t *a1, int a2, int a3, int a4, int a5)
{
    int i, j;
    int v34 = a4 + abs(a2);
    int v48 = a5 + abs(a3);
    int v30 = a5 + a3 - 1;
    int v28 = a4 + a2 - 1;
    int v1c, v20, v10, v14;
    char *v24;
    if (v28 >= 320)
        v28 = 319;
    if (v30 >= 200)
        v30 = 199;
    for (i = a3; i < v48; i += a1->f_10)
    {
        if (i <= v30 && i + a1->f_10 > 0)
        {
            if (i < 0)
            {
                v1c += i;
                v24 += (-i) * a1->f_c;
                v20 = 0;
            }
            else
                v20 = i;
            for (j = a2; j < v34; j += a1->f_c)
            {
                if (j <= v28 && j + a1->f_c > 0)
                {
                    v24 = a1->f_14;
                    v10 = a1->f_c;
                    v1c = a1->f_10;
                    if (j < 0)
                    {
                        v10 += j;
                        v24 += -j;
                        v14 = 0;
                    }
                    else
                        v14 = j;
                    if (v14 + v10 - 1 >= v28)
                        v10 = (v28 + 1) - v14;
                    if (v20 + v1c - 1 >= v30)
                        v1c = (v30 + 1) - v20;
                    gfx_inmem = v24;
                    gfx_xp = v14;
                    gfx_yp = v20;
                    gfx_lx = v10;
                    gfx_ly = v1c;
                    gfx_imga = a1->f_c - v10;
                    GFX_PutPic();
                    GFX_MarkUpdate(v14, v20, v10, v1c);
                }
            }
        }
    }
}

void GFX_ShadeArea(int a1, int a2, int a3, int a4, int a5)
{
    int i;
    char *p, *v14;
    if (!GFX_ClipLines(NULL, &a2, &a3, &a4, &a5))
        return;
    p = &displaybuffer[a2 + ylookup[a3]];
    switch (a1)
    {
    case 0:
        v14 = dtable;
        break;
    case 1:
        v14 = ltable;
        break;
    case 2:
        v14 = gtable;
        break;
    }
    GFX_MarkUpdate(a2, a3, a4, a5);
    for (i = 0; i < a5; i++)
    {
        GFX_Shade(p, a4, v14);
        p += 320;
    }
}

void GFX_ShadeShape(int a1, texture_t* a2, int a3, int a4)
{
    char c;
    char *v14;
    int v20 = a3;
    int v24 = a4;
    int v28 = a2->f_c;
    int v2c = a2->f_10;
    texture_t *vbp = (texture_t*)a2->f_14;

    c = GFX_ClipLines(NULL, &v20, &v24, &v28, &v2c);
    if (!c)
        return;
    switch (a1)
    {
    case 0:
        v14 = dtable;
        break;
    case 1:
        v14 = ltable;
        break;
    case 2:
        v14 = gtable;
        break;
    }
    switch (c)
    {
    case 1:
        GFX_ShadeSprite(&displaybuffer[a3 + ylookup[a4]], vbp, v14);
        break;
    case 2:
        while (vbp->f_8 != -1)
        {
            v20 = vbp->f_0 + a3;
            v24 = vbp->f_4 + a4;
            if (v24 > 200)
                return;
            v28 = vbp->f_c;
            v2c = 1;
            if (GFX_ClipLines(NULL, &v20, &v24, &v28, &v2c))
                GFX_Shade(&displaybuffer[v20 + ylookup[v24]], v28, v14);
            vbp = (texture_t*)((char*)vbp + 16 + vbp->f_c);
        }
        break;
    }
}


void GFX_VShadeLine(int a1, int a2, int a3, int a4)
{
    char *vs;
    char *p;
    int v10 = 1;
    if (a4 < 1)
        return;
    if (!GFX_ClipLines(NULL, &a2, &a3, &v10, &a4))
        return;
    switch (a1)
    {
    case 0:
        vs = dtable;
        break;
    case 1:
        vs = ltable;
        break;
    case 2:
        vs = gtable;
        break;
    }
    GFX_MarkUpdate(a2, a3, v10, a4);
    p = &displaybuffer[a2 + ylookup[a3]];
    while (a4--)
    {
        *p = vs[(uint8_t)*p];
        p += 320;
    }
}

void GFX_HShadeLine(int a1, int a2, int a3, int a4)
{
    char *vs;
    char *p;
    int v10 = 1;
    if (a4 < 1)
        return;
    if (!GFX_ClipLines(NULL, &a2, &a3, &a4, &v10))
        return;
    switch (a1)
    {
    case 0:
        vs = dtable;
        break;
    case 1:
        vs = ltable;
        break;
    case 2:
        vs = gtable;
        break;
    }
    GFX_MarkUpdate(a2, a3, a4, v10);
    p = &displaybuffer[a2 + ylookup[a3]];
    GFX_Shade(p, a4, vs);
}

void GFX_LightBox(int a1, int a2, int a3, int a4, int a5)
{
    if (a4 < 1 || a5 < 1)
        return;
    switch (a1)
    {
    case 0:
        GFX_HShadeLine(1, a2, a3, a4 - 1);
        GFX_VShadeLine(1, a2, a3 + 1, a5 - 2);
        GFX_HShadeLine(0, a2, a3 + a5 - 1, a4);
        GFX_VShadeLine(0, a2 + a4 - 1, a3 + 1, a5 - 2);
        break;
    case 1:
    default:
        GFX_HShadeLine(1, a2 + 1, a3, a4 - 1);
        GFX_VShadeLine(1, a2 + a4 - 1, a3 + 1, a5 - 2);
        GFX_HShadeLine(0, a2, a3 + a5 - 1, a4);
        GFX_VShadeLine(0, a2, a3, a5 - 1);
        break;
    case 2:
        GFX_HShadeLine(1, a2, a3 + a5 - 1, a4 - 1);
        GFX_VShadeLine(1, a2, a3 + 1, a5 - 2);
        GFX_HShadeLine(0, a2, a3, a4);
        GFX_VShadeLine(0, a2 + a4 - 1, a3 + 1, a5 - 1);
        break;
    case 3:
        GFX_HShadeLine(1, a2 + 1, a3, a4 - 1);
        GFX_VShadeLine(1, a2 + a4 - 1, a3 + 1, a5 - 2);
        GFX_HShadeLine(0, a2, a3, a4);
        GFX_VShadeLine(0, a2, a3 + 1, a5 - 2);
        break;
    }
}

void GFX_ColorBox(int a1, int a2, int a3, int a4, int a5)
{
    char *p;
    if (a3 < 1 || a4 < 1)
        return;
    if (!GFX_ClipLines(NULL, &a1, &a2, &a3, &a4))
        return;
    p = &displaybuffer[a1 + ylookup[a2]];
    GFX_MarkUpdate(a1, a2, a3, a4);
    if (a5 < 0)
    {
        while (a4--)
        {
            GFX_HLine(a1, a2, a3, a5);
            a2++;
        }
    }
    else
    {
        while (a4--)
        {
            memset(p, a5, a3);
            p += 320;
        }
    }
}

void GFX_HLine(int a1, int a2, int a3, int a4)
{
    char *p;
    int i;
    int v14 = 1;
    if (a3 < 1)
        return;
    if (!GFX_ClipLines(0, &a1, &a2, &a3, &v14))
        return;
    p = &displaybuffer[a1 + ylookup[a2]];
    GFX_MarkUpdate(a1, a2, a3, 1);
    if (a4 < 0)
    {
        for (i = 0; i < a3; i++)
        {
            *p ^= (a4 + 255);
            p++;
        }
    }
    else
        memset(p, a4, a3);
}

void GFX_VLine(int a1, int a2, int a3, int a4)
{
    char *p;
    int v14 = 1;
    if (a3 < 1)
        return;
    if (!GFX_ClipLines(0, &a1, &a2, &v14, &a3))
        return;
    p = &displaybuffer[a1 + ylookup[a2]];
    GFX_MarkUpdate(a1, a2, 1, a3);
    if (a4 < 0)
    {
        while (a3--)
        {
            *p ^= (a4 + 255);
            p += 320;
        }
    }
    else
    {
        while (a3--)
        {
            *p = a4;
            p += 320;
        }
    }
}

void GFX_Line(int a1, int a2, int a3, int a4, int a5)
{
    int v18 = 1;
    int v14 = 1;
    int v10 = a4 - a2;
    int vdi = a3 - a1;
    int t, vsi;
    if (vdi < 0)
    {
        vdi = -vdi;
        v18 = -v18;
    }
    if (v10 < 0)
    {
        v10 = -v10;
        v14 = -v14;
    }
    if (vdi >= v10)
    {
        t = -(v10 >> 1);
        vsi = vdi + 1;
    }
    else
    {
        t = (vdi >> 1);
        vsi = v10 + 1;
    }
    if (vdi >= v10)
    {
        while (vsi)
        {
            if (a1 >= 0 && a1 < 320 && a2 >= 0 && a2 < 200)
            {
                displaybuffer[a1 + ylookup[a2]] = a5;
            }
            vsi--;
            t += v10;
            a1 += v18;
            if (t > 0)
            {
                t -= vdi;
                a2 += v14;
            }
        }
    }
    else
    {
        while (vsi)
        {
            if (a1 >= 0 && a1 <= 320 && a2 >= 0 && a2 < 200)
            {
                displaybuffer[a1 + ylookup[a2]] = a5;
            }
            vsi--;
            t += vdi;
            a2 += v14;
            if (t > 0)
            {
                t -= v10;
                a1 += v18;
            }
        }
    }
}

void GFX_Rectangle(int a1, int a2, int a3, int a4, int a5)
{
    if (a4 < 1 || a3 < 1)
        return;
    GFX_HLine(a1, a2, a3, a5);
    GFX_HLine(a1, a2 + a4 - 1, a3, a5);
    GFX_VLine(a1, a2 + 1, a4 - 2, a5);
    GFX_VLine(a1 + a3 - 1, a2 + 1, a4 - 2, a5);
}

void GFX_ScalePic(texture_t *a1, int a2, int a3, int a4, int a5, int a6)
{
    char *p;
    int v10, vdi, i;
    char *v14 = a1->f_14;
    int v20 = (a1->f_c<<16) / a4;
    int v18 = (a1->f_10<<16) / a5;
    v10 = 0;
    vdi = 0;
    if (!a1->f_0)
    {
        GFX_PutSprite(a1, a2, a3);
        return;
    }
    if (a2 < 0)
    {
        v10 = v20 * (-a2);
        v14 += v10 >> 16;
        v10 &= 0xffff;
        a4 += a2;
        a2 = 0;
    }
    if (a3 < 0)
    {
        vdi = v18 * (-a3);
        v14 += (a1->f_c) * (vdi >> 16);
        vdi &= 0xffff;
        a5 += a3;
        a3 = 0;
    }
    if (a2 + a4 > 320)
    {
        a4 = 320 - a2;
    }
    if (a3 + a5 > 200)
    {
        a5 = 200 - a3;
    }
    p = &displaybuffer[a2 + ylookup[a3]];
    GFX_MarkUpdate(a2, a3, a4, a5);
    tablelen = a4;
    for (i = a4 - 1; i >= 0; i--)
    {
        stable[i] = (v10 >> 16);
        v10 += v20;
    }
    if (a6)
    {
        while (a5--)
        {
            GFX_CScaleLine(p, v14 + a1->f_c * (vdi>>16));
            vdi += v18;
            p += 320;
        }
    }
    else
    {
        while (a5--)
        {
            GFX_ScaleLine(p, v14 + a1->f_c * (vdi>>16));
            vdi += v18;
            p += 320;
        }
    }
}



void GFX_MarkUpdate(int a1, int a2, int a3, int a4)
{
    int vbp = a1 + a3 - 1;
    int vsi = a2 + a4 - 1;
    int vbx = ud_x + ud_lx - 1;
    int vcx = ud_y + ud_ly - 1;
    if (update_start)
    {
        if (a1 < ud_x)
            ud_x = a1;
        if (a2 < ud_y)
            ud_y = a2;
        if (vbp > vbx)
            vbx = vbp;
        if (vsi > vcx)
            vcx = vsi;
    }
    else
    {
        ud_x = a1;
        ud_y = a2;
        vbx = vbp;
        vcx = vsi;
        if (a3 + a4 > 0)
            update_start = 1;
    }
    if ((ud_x) & 3)
        ud_x = ud_x - (ud_x & 3);
    if (ud_x < 0)
        ud_x = 0;
    if (ud_y < 0)
        ud_y = 0;
    ud_lx = (vbx - ud_x + 1);
    ud_ly = (vcx - ud_y + 1);
    if ((ud_lx) & 3)
        ud_lx += 4 - (ud_lx & 3);
    if (ud_x + ud_lx > 320)
        ud_lx = 320 - ud_x;
    if (ud_y + ud_ly > 200)
        ud_ly = 200 - ud_y;
}

void GFX_ForceUpdate(int a1, int a2, int a3, int a4)
{
    ud_x = a1;
    ud_y = a2;
    ud_lx = a3;
    ud_ly = a4;
    o_ud_x = a1;
    o_ud_y = a2;
    o_ud_lx = a3;
    o_ud_ly = a4;
}

void GFX_SetFrameHook(void (*a1)(void (*)(void)))
{
    framehook = a1;
}

void GFX_WaitUpdate(int a1)
{
    static int now;
    int vsi, i;
    if (a1 > 70)
        a1 = 70;
    else if (a1 < 1)
        a1 = 70;
    vsi = 70 / a1;
    GFX_MarkUpdate(o_ud_x, o_ud_y, o_ud_lx, o_ud_ly);
    for (i = 0; i < vsi; i++)
    {
        while (now == GFX_GetFrameCount())
        {
            // if (DAT_00061c5c)
            //     break;
        }
        now = GFX_GetFrameCount();
    }
    if (update_start)
    {
        if (framehook)
            framehook(GFX_DisplayScreen);
        else
            GFX_DisplayScreen();
    }
    o_ud_x = ud_x;
    o_ud_y = ud_y;
    o_ud_lx = ud_lx;
    o_ud_ly = ud_ly;
    I_FinishUpdate();
}

void GFX_DisplayUpdate(void)
{
    static int DAT_00061c84;
    I_GetEvent();
    GFX_UpdateTimer();
    while (DAT_00061c84 == framecount)
    {
        GFX_UpdateTimer();
        //if (DAT_00061c5c)
        //    break;
    }
    GFX_MarkUpdate(o_ud_x, o_ud_y, o_ud_lx, o_ud_ly);
    if (update_start)
    {
        if (framehook)
            framehook(GFX_DisplayScreen);
        else
            GFX_DisplayScreen();
    }
    o_ud_x = ud_x;
    o_ud_y = ud_y;
    o_ud_lx = ud_lx;
    o_ud_ly = ud_ly;
    DAT_00061c84 = framecount;
    I_FinishUpdate();
}

void GFX_PutImage(texture_t *a1, int a2, int a3, int a4)
{
    char *v14;
    gfx_lx = a1->f_c;
    gfx_ly = a1->f_10;
    if (a1->f_0 == 0)
    {
        GFX_PutSprite(a1, a2, a3);
        return;
    }
    v14 = a1->f_14;
    if (!GFX_ClipLines(&v14, &a2, &a3, &gfx_lx, &gfx_ly))
        return;
    GFX_MarkUpdate(a2, a3, gfx_lx, gfx_ly);
    gfx_xp = a2;
    gfx_yp = a3;
    gfx_inmem = v14;
    gfx_imga = a1->f_c;
    if (a4 == 0)
    {
        gfx_imga -= gfx_lx;
        GFX_PutPic();
    }
    else
        GFX_PutMaskPic();
}

void GFX_PutSprite(texture_t *a1, int a2, int a3)
{
    char c;
    char *vbp;
    texture_t *v14;
    char *v20;
    int v24 = a2;
    int v28 = a3;
    int v2c = a1->f_c;
    int v30 = a1->f_10;
    c = GFX_ClipLines(NULL, &v24, &v28, &v2c, &v30);
    if (!c)
        return;
    switch (c)
    {
    case 1:
        GFX_DrawSprite(&displaybuffer[v24 + ylookup[v28]], (texture_t*)a1->f_14);
        break;
    case 2:
        v14 = (texture_t*)a1->f_14;
        while (v14->f_8 != -1)
        {
            v24 = a2 + v14->f_0;
            v28 = a3 + v14->f_4;
            vbp = (char*)v14 + 16;
            if (v28 > 200)
                break;
            v2c = v14->f_c;
            v30 = 1;
            v20 = vbp;
            if (GFX_ClipLines(&v20, &v24, &v28, &v2c, &v30))
                memcpy(&displaybuffer[v24 + ylookup[v28]], v20, v2c);
            v14 = (texture_t*)(vbp + v14->f_c);
        }
        break;
    }
}

void GFX_OverlayImage(texture_t *a1, texture_t *a2, int a3, int a4)
{
    char *vsi, *vdx;
    int vbp, i, j;
    int v14 = a3 + a2->f_c - 1;
    int v10 = a4 + a2->f_10 - 1;
    if (a3 < 0 || a4 < 0)
        return;
    if (v14 >= a1->f_c || v10 >= a1->f_10)
        return;
    vsi = &a1->f_14[a3 + a1->f_c * a4];
    vbp = a2->f_c - a1->f_c;
    vdx = a2->f_14;
    for (i = 0; i < a2->f_10; i++)
    {
        for (j = 0; j < a2->f_c; j++)
        {
            if (j != 255)
                *vsi = *vdx;
            vsi++;
            vdx++;
        }
        vsi += vbp;
    }
}

int fontspacing = 1;

int GFX_StrPixelLen(font_t *a1, char *a2, int a3)
{
    int i;
    int l = 0;
    for (i = 0; i < a3; i++)
    {
        l += a1->f_204[a2[i]] + fontspacing;
    }
    return l;
}

int GFX_PutChar(int a1, int a2, char a3, font_t* a4, int a5)
{
    char *p;
    int w = a4->f_204[a3];
    int wo;
    int h = a4->f_0;
    char* v18 = &a4->f_304[a4->f_4[a3]];
    wo = w;
    if (!GFX_ClipLines(&v18, &a1, &a2, &w, &h))
        return 0;
    p = &displaybuffer[a1 + ylookup[a2]];
    GFX_MarkUpdate(a1, a2, w, h);
    GFX_DrawChar(p, v18, w, h, wo - w, a5);
    return w;
}

int GFX_Print(int a1, int a2, char *a3, font_t* a4, int a5)
{
    char c;
    int l, v10, w;
    a5--;
    l = strlen(a3);
    v10 = 0;
    if (!l)
        return 0;
    while ((c = *a3++) != 0)
    {
        if (a4->f_4[c] == -1)
            continue;
        w = GFX_PutChar(a1, a2, c, a4, a5);
        v10 += w + fontspacing;
        a1 += a4->f_204[c] + fontspacing;
    }
    return v10;
}

void GFX_3D_SetView(int a1, int a2, int a3)
{
    G3D_viewx = a1;
    G3D_viewy = a2;
    G3D_viewz = a3;
}

void GFX_3DPoint(void)
{
    G3D_x -= G3D_viewx;
    G3D_y -= G3D_viewy;
    G3D_z -= G3D_viewz;
    G3D_screenx = ((G3D_x * (200<<11)) / G3D_z) >> 11;
    G3D_screeny = ((G3D_y * (200<<11)) / G3D_z) >> 11;
    G3D_screenx += G3D_viewx;
    G3D_screeny += G3D_viewy;
}

void GFX_3D_PutImage(texture_t *a1, int a2, int a3, int a4, int a5)
{
    int v10, v14, vd, vb;
    if (a4 == 200)
    {
        GFX_MarkUpdate(a2, a3, a1->f_c, a1->f_10);
        GFX_PutImage(a1, a2, a3, a5);
        return;
    }
    G3D_x = a2;
    G3D_y = a3;
    G3D_z = a4;
    GFX_3DPoint();
    v10 = G3D_screenx;
    v14 = G3D_screeny;
    G3D_x = a2 + a1->f_c - 1;
    G3D_y = a3 + a1->f_10 - 1;
    G3D_z = a4;
    GFX_3DPoint();
    vd = G3D_screenx - v10;
    vb = G3D_screeny - v14;
    GFX_MarkUpdate(v10, v14, vd, vb);
    GFX_ScalePic(a1, v10, v14, vd, vb, a5);
}

