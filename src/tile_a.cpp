#include <string.h>
#include "common.h"
#include "tile.h"
#include "rap.h"
#include "gfxapi.h"
#include "i_video.h"

#define TILEWIDTH 32
#define TILEHEIGHT 32

/***************************************************************************
TILE_Draw() - 32 by 32 Tile draw
 ***************************************************************************/
void 
TILE_Draw(
    void
)
{
    int loop;
    
    for (loop = 0; loop < TILEHEIGHT; loop++)
    {
        memcpy(&tilestart[loop * SCREENWIDTH], &tilepic[loop * TILEWIDTH], TILEWIDTH);
    }
}

/***************************************************************************
TILE_ClipDraw() - 
 ***************************************************************************/
void 
TILE_ClipDraw(
    void
)
{
    int loop;
    
    for (loop = 0; loop < tileloopy; loop++)
    {
        memcpy(&tilestart[loop * SCREENWIDTH], &tilepic[loop * TILEWIDTH], TILEWIDTH);
    }
}

/***************************************************************************
TILE_ShakeScreen() -
 ***************************************************************************/
void 
TILE_ShakeScreen(
    void
)
{
    int loop;
    char *src = displaybuffer + 12;
    char *dst = displayscreen + g_mapleft - 4;
    
    for (loop = 0; loop < SCREENHEIGHT; loop++)
    {
        memcpy(dst, src, 296);
        dst += SCREENWIDTH;
        src += SCREENWIDTH;
    }
    
    I_FinishUpdate();
}

/***************************************************************************
TILE_DisplayScreen() -
 ***************************************************************************/
void 
TILE_DisplayScreen(
    void
)
{
    int loop;
    char *src = displaybuffer + MAP_LEFT;
    char *dst = displayscreen + MAP_LEFT;
    
    for (loop = 0; loop < SCREENHEIGHT; loop++)
    {
        memcpy(dst, src, 288);
        dst += SCREENWIDTH;
        src += SCREENWIDTH;
    }
    
    I_FinishUpdate();
}
