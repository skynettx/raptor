#include <stdint.h>
#include "common.h"
#include "gfxapi.h"
#include "rap.h"
#include "windows.h"
#include "glbapi.h"
#include "entypes.h"

char sdtablemem[516];
char *sdtable;

typedef struct 
{
    int item;
    int x;
    int y;
}SHADOW;

#define MAX_SHADOWS 50
#define MAX_GSHADOWS 25

SHADOW shads[MAX_SHADOWS];
SHADOW gshads[MAX_GSHADOWS];
int num_shadows, num_gshadows;

#define MAXZ  1280;

/***************************************************************************
SHADOW_Draw () - Draws AIR shadows in 3D perspective
 ***************************************************************************/
void 
SHADOW_Draw(
    char *pic,             // INPUT : pointer to sprite data
    int x,                 // INPUT : x position of sprite
    int y                  // INPUT : y position of sprite
)
{
    GFX_PIC *h = (GFX_PIC*)pic;
    GFX_SPRITE *ah;
    int lx, ly, oldsy, sx, sy, ox, oy, x2, y2, oldy, drawflag;

    x -= 10;
    y += 20;
    
    oldy = oldsy = -1;
    
    G3D_x = x;
    G3D_y = y;
    G3D_z = MAXZ;
    GFX_3DPoint();
    ox = G3D_screenx;
    oy = G3D_screeny;
    
    G3D_x = ox + LE_LONG(h->width) - 1;
    G3D_y = oy + LE_LONG(h->height) - 1;
    G3D_z = MAXZ;
    GFX_3DPoint();
    lx = G3D_screenx - ox + 1;
    ly = G3D_screenx - oy + 1;
    
    if (!GFX_ClipLines(0, &ox, &oy, &lx, &ly))
        return;
    
    pic += sizeof(GFX_PIC);
    
    ah = (GFX_SPRITE*)pic;
    
    while (LE_LONG(ah->offset) != -1)
    {
        pic += sizeof(GFX_SPRITE);
        
        ox = LE_LONG(ah->x) + x;
        oy = LE_LONG(ah->y) + y;
        
        x2 = ox + LE_LONG(ah->length) - 1;
        y2 = oy + 1;
        
        G3D_x = ox;
        G3D_y = oy;
        G3D_z = MAXZ;
        GFX_3DPoint();
        sx = G3D_screenx;
        sy = G3D_screeny;
        
        G3D_x = x2;
        G3D_y = y2;
        G3D_z = MAXZ;
        GFX_3DPoint();
        lx = G3D_screenx - sx + 1;
        
        if (sy > SCREENHEIGHT)
            return;
        
        drawflag = 1;
        
        if (LE_LONG(ah->y) != oldy && oldsy == sy)
            drawflag = 0;
        
        if (drawflag)
        {
            ly = 1;
            
            if (GFX_ClipLines(0, &sx, &sy, &lx, &ly))
                GFX_Shade(displaybuffer + sx + ylookup[sy], lx, sdtable);
            
            oldy = LE_LONG(ah->y);
        }
        
        oldsy = sy;
        
        pic += LE_LONG(ah->length);
        
        ah = (GFX_SPRITE*)pic;
    }
}

/***************************************************************************
SHADOW_Init() - Allocate memory and set 3D view
 ***************************************************************************/
void 
SHADOW_Init(
    void
)
{
    sdtable = sdtablemem;
    sdtable = (char*)(((intptr_t)sdtable + 255) & ~255);
    
    GFX_3D_SetView(160, 100, 1000);
}

/***************************************************************************
SHADOW_MakeShades() - Make shade tables
 ***************************************************************************/
void 
SHADOW_MakeShades(
    void
)
{
    GFX_MakeLightTable(palette, sdtable, -6);
}

/***************************************************************************
SHADOW_Add() - Add a Air ship shadow
 ***************************************************************************/
void 
SHADOW_Add(
    int item,               // INPUT : GLB item
    int x,                  // INPUT : x position
    int y                   // INPUT : y position
)
{
    SHADOW *cur;
    cur = &shads[num_shadows];
    
    if (num_shadows < MAX_SHADOWS)
    {
        cur->item = item;
        cur->x = x;
        cur->y = y;
        
        num_shadows++;
    }
}

/***************************************************************************
SHADOW_GAdd() - Adds Ground shadow
 ***************************************************************************/
void 
SHADOW_GAdd(
    int item,              // INPUT : GLB item
    int x,                 // INPUT : x position
    int y                  // INPUT : y position
)
{
    SHADOW *cur;
    cur = &gshads[num_gshadows];
    
    if (num_gshadows < MAX_GSHADOWS)
    {
        cur->item = item;
        cur->x = x - 3;
        cur->y = y + 4;
        
        num_gshadows++;
    }
}

/***************************************************************************
SHADOW_DisplaySky () - Display Sky Shadows
 ***************************************************************************/
void 
SHADOW_DisplaySky(
    void
)
{
    char *pic;
    SHADOW *cur;
    cur = shads;
    
    if (opt_detail < 1)
        return;
    
    while (--num_shadows != -1)
    {
        pic = GLB_GetItem(cur->item);
        SHADOW_Draw(pic, cur->x, cur->y);
        cur++;
    }
    
    num_shadows = 0;
}

/***************************************************************************
SHADOW_DisplayGround() - Display Ground Shadows
 ***************************************************************************/
void 
SHADOW_DisplayGround(
    void
)
{
    char *pic;
    SHADOW *cur;
    cur = gshads;
    
    if (opt_detail < 1)
        return;
    
    while (--num_gshadows != -1)
    {
        pic = GLB_GetItem(cur->item);
        GFX_ShadeShape(DARK, (char*)pic, cur->x, cur->y);
        cur++;
    }
    
    num_gshadows = 0;
}
