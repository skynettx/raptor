#include <stdint.h>
#include "common.h"
#include "flame.h"
#include "rap.h"
#include "gfxapi.h"
#include "windows.h"

#define MAX_SHADES 8

char stmem[MAX_SHADES * 512];

static char *_stable[MAX_SHADES];

/***************************************************************************
FLAME_Init () - Inits Flame Tables and stuff
 ***************************************************************************/
void 
FLAME_Init(
    void
)
{
    int loop;
    
    for (loop = 0; loop < MAX_SHADES; loop++)
    {
        _stable[loop] = &stmem[loop * 512];
        
        _stable[loop] = (char*)(((intptr_t)_stable[loop] + 255) & ~255);
        
        GFX_MakeLightTable(palette, _stable[loop], (MAX_SHADES - loop) * 2);
    }
}

/***************************************************************************
FLAME_InitShades () - Inits shading stuff
 ***************************************************************************/
void 
FLAME_InitShades(
    void
)
{
    int loop;
    
    for (loop = 0; loop < MAX_SHADES; loop++)
    {
        GFX_MakeLightTable(palette, _stable[loop], (MAX_SHADES - loop) * 2);
    }
}

/***************************************************************************
FLAME_Up () - Shows Flame shooting upward
 ***************************************************************************/
void 
FLAME_Up(
    int ix,               // INPUT : x position
    int iy,               // INPUT : y position
    int width,            // INPUT : width of shade
    int frame             // INPUT : frame
)
{
    char *outbuf;
    int y, addx, loop, num;
    unsigned int curs;
    int height[2] = {
        5, 10
    };

    if (opt_detail < 1)
        return;

    frame = frame % 2;
    
    iy -= height[frame] - 1;
    
    if (!GFX_ClipLines(0, &ix, &iy, &width, &height[frame]))
        return;

    y = iy;
    
    addx = (MAX_SHADES << 16) / height[frame];
    curs = addx * (height[frame] - 1);
    
    for (loop = 0; loop < height[frame]; loop++)
    {
        outbuf = displaybuffer + ix + ylookup[y];
        
        y++;
        
        num = curs >> 16;
        
        if (num >= 8)
            EXIT_Error("flame > 8 %u", curs >> 16);

        if (num < 0)
            EXIT_Error("flame < 0");

        GFX_Shade(outbuf, width, _stable[num]);
        
        curs -= addx;
    }
}

/***************************************************************************
FLAME_Down () - Shows Flame shooting downward
 ***************************************************************************/
void 
FLAME_Down(
    int ix,                // INPUT : x position
    int iy,                // INPUT : y position
    int width,             // INPUT : width of shade
    int frame              // INPUT : frame
)
{
    char *outbuf;
    int y, loop, addx;
    unsigned int curs;
    int height[2] = {
        8, 12
    };
    
    frame = frame % 2;
    
    if (!GFX_ClipLines(0, &ix, &iy, &width, &height[frame]))
        return;
    
    y = iy;
    
    curs = 0;
    addx = (MAX_SHADES << 16) / height[frame];
    
    for (loop = 0; loop < height[frame]; loop++)
    {
        outbuf = displaybuffer + ix + ylookup[y];
        
        y++;
        
        GFX_Shade(outbuf, width, _stable[curs >> 16]);
        
        curs += addx;
    }
}
