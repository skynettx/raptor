#include <stdint.h>
#include "common.h"
#include "flame.h"
#include "rap.h"
#include "gfxapi.h"
#include "windows.h"

char stmem[4096];

static char *_stable[8];

void FLAME_Init(void)
{
    int v1c;
    for (v1c = 0; v1c < 8; v1c++)
    {
        _stable[v1c] = &stmem[v1c * 512];
        _stable[v1c] = (char*)(((intptr_t)_stable[v1c] + 255) & ~255);
        GFX_MakeLightTable(palette, _stable[v1c], (8 - v1c) * 2);
    }
}

void FLAME_InitShades(void)
{
    int v1c;
    for (v1c = 0; v1c < 8; v1c++)
    {
        GFX_MakeLightTable(palette, _stable[v1c], (8 - v1c) * 2);
    }
}

void FLAME_Up(int a1, int a2, int a3, int a4)
{
    char *v14;
    int v18, v10, v1c, v28;
    unsigned int v20;
    int v3c[2] = {
        5, 10
    };

    if (opt_detail < 1)
        return;

    a4 = a4 % 2;
    a2 -= v3c[a4] - 1;
    if (!GFX_ClipLines(0, &a1, &a2, &a3, &v3c[a4]))
        return;

    v18 = a2;
    v10 = 0x80000 / v3c[a4];
    v20 = v10 * (v3c[a4] - 1);
    for (v1c = 0; v1c < v3c[a4]; v1c++)
    {
        v14 = displaybuffer + a1 + ylookup[v18];
        v18++;
        v28 = v20 >> 16;
        if (v28 >= 8)
            EXIT_Error("flame > 8 %u", v20);

        if (v28 < 0)
            EXIT_Error("flame < 0");

        GFX_Shade(v14, a3, _stable[v28]);
        v20 -= v10;
    }
}

void FLAME_Down(int a1, int a2, int a3, int a4)
{
    char *v14;
    int v18, v1c, v10;
    unsigned int v20;
    int v38[2] = {
        8, 12
    };
    a4 %= 2;
    if (!GFX_ClipLines(0, &a1, &a2, &a3, &v38[a4]))
        return;
    v18 = a2;
    v20 = 0;
    v10 = 0x80000 / v38[a4];
    for (v1c = 0; v1c < v38[a4]; v1c++)
    {
        v14 = displaybuffer + a1 + ylookup[v18];
        v18++;
        GFX_Shade(v14, a3, _stable[v20 >> 16]);
        v20 += v10;
    }
}
