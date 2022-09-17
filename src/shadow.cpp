#include <stdint.h>
#include "common.h"
#include "gfxapi.h"
#include "rap.h"
#include "windows.h"
#include "glbapi.h"

char sdtablemem[516];
char *sdtable;

struct shad_t {
    int f_0;
    int f_4;
    int f_8;
};

shad_t shads[50];
shad_t gshads[25];
int num_shadows, num_gshadows;

void SHADOW_Draw(char *a1, int a2, int a3)
{
    texture_t *v14;
    texture_t *v20;
    int v18, v1c, v40, v24, v28, v2c, v30, v34, v38, v3c, v44;

    v14 = (texture_t*)a1;
    a2 -= 10;
    a3 += 20;
    v3c = v40 = -1;
    G3D_x = a2;
    G3D_y = a3;
    G3D_z = 0x500;
    GFX_3DPoint();
    v2c = G3D_screenx;
    v30 = G3D_screeny;
    G3D_x = v2c + v14->width - 1;
    G3D_y = v30 + v14->height - 1;
    G3D_z = 0x500;
    GFX_3DPoint();
    v18 = G3D_screenx - v2c + 1;
    v1c = G3D_screenx - v30 + 1;
    if (!GFX_ClipLines(0, &v2c, &v30, &v18, &v1c))
        return;
    a1 += 0x14;
    v20 = (texture_t*)a1;
    while (v20->f_8 != -1)
    {
        a1 += 16;
        v2c = v20->f_0 + a2;
        v30 = v20->f_4 + a3;
        v34 = v2c + v20->width - 1;
        v38 = v30 + 1;
        G3D_x = v2c;
        G3D_y = v30;
        G3D_z = 0x500;
        GFX_3DPoint();
        v24 = G3D_screenx;
        v28 = G3D_screeny;
        G3D_x = v34;
        G3D_y = v38;
        G3D_z = 0x500;
        GFX_3DPoint();
        v18 = G3D_screenx - v24 + 1;
        if (v28 > 200)
            return;
        v44 = 1;
        if (v20->f_4 != v3c && v40 == v28)
            v44 = 0;
        if (v44)
        {
            if (GFX_ClipLines(0, &v24, &v28, &v18, &v1c))
                GFX_Shade(displaybuffer + v24 + ylookup[v28], v18, sdtable);
            v3c = v20->f_4;
        }
        v40 = v28;
        a1 += v20->width;
        v20 = (texture_t*)a1;
    }
}

void SHADOW_Init(void)
{
    sdtable = sdtablemem;
    sdtable = (char*)(((intptr_t)sdtable + 255) & ~255);
    GFX_3D_SetView(160, 100, 1000);
}

void SHADOW_MakeShades(void)
{
    GFX_MakeLightTable(palette, sdtable, -6);
}

void SHADOW_Add(int a1, int a2, int a3)
{
    shad_t *v14;
    v14 = &shads[num_shadows];
    if (num_shadows < 50)
    {
        v14->f_0 = a1;
        v14->f_4 = a2;
        v14->f_8 = a3;
        num_shadows++;
    }
}

void SHADOW_GAdd(int a1, int a2, int a3)
{
    shad_t *v14;
    v14 = &gshads[num_gshadows];
    if (num_gshadows < 25)
    {
        v14->f_0 = a1;
        v14->f_4 = a2 - 3;
        v14->f_8 = a3 + 4;
        num_gshadows++;
    }
}

void SHADOW_DisplaySky(void)
{
    char *v20;
    shad_t *v1c;
    v1c = shads;
    if (opt_detail < 1)
        return;
    while (--num_shadows != -1)
    {
        v20 = GLB_GetItem(v1c->f_0);
        SHADOW_Draw(v20, v1c->f_4, v1c->f_8);
        v1c++;
    }
    num_shadows = 0;
}

void SHADOW_DisplayGround(void)
{
    char *v20;
    shad_t *v1c;
    v1c = gshads;
    if (opt_detail < 1)
        return;
    while (--num_gshadows != -1)
    {
        v20 = GLB_GetItem(v1c->f_0);
        GFX_ShadeShape(0, (texture_t*)v20, v1c->f_4, v1c->f_8);
        v1c++;
    }
    num_gshadows = 0;
}
