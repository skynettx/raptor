#include <string.h>
#include "common.h"
#include "tile.h"
#include "rap.h"
#include "gfxapi.h"
#include "i_video.h"

void TILE_Draw(void)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        memcpy(&tilestart[i * 320], &tilepic[i * 32], 32);
    }
}

void TILE_ClipDraw(void)
{
    int i;
    for (i = 0; i < tileloopy; i++)
    {
        memcpy(&tilestart[i * 320], &tilepic[i * 32], 32);
    }
}

void TILE_ShakeScreen(void)
{
    int i;
    char *src = displaybuffer + 12;
    char *dst = displayscreen + g_mapleft - 4;
    for (i = 0; i < 200; i++)
    {
        memcpy(dst, src, 296);
        dst += 320;
        src += 320;
    }
    I_FinishUpdate();
}

void TILE_DisplayScreen(void)
{
    int i;
    char *src = displaybuffer + 16;
    char *dst = displayscreen + 16;
    for (i = 0; i < 200; i++)
    {
        memcpy(dst, src, 288);
        dst += 320;
        src += 320;
    }
    I_FinishUpdate();
}
