#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "tile.h"
#include "gfxapi.h"
#include "rap.h"
#include "glbapi.h"
#include "loadsave.h"
#include "fx.h"
#include "anims.h"

#define MAX_STILES ( MAP_ONSCREEN * MAP_COLS )
#define MAX_TILEDELAY ( ( MAP_ONSCREEN + 1 ) * MAP_COLS )

int eitems[MAP_SIZE];
int titems[MAP_SIZE];
short hits[MAP_SIZE];
short tdead[MAP_SIZE];
short money[MAP_SIZE];

tdel_t tdel[MAX_TILEDELAY];

tdel_t first_delay;
tdel_t last_delay;

tdel_t *free_delay;

int tileloopy;
char *tilepic;

char game_start[4][17] = {
   "STARTG1TILES",
   "STARTG2TILES",
   "STARTG3TILES",
   "STARTG4TILES"
};

int scroll_flag;
int last_tile;

char *tilestart;

int startflat[4];

int tilepos;
int tileyoff;

tspot_t tspots[MAX_STILES];
tspot_t *lastspot;
int spark_delay;
int flare_delay;

/*-------------------------------------------------------------------------*
TClear () - Clears TILE Delay Link List
 *-------------------------------------------------------------------------*/
void 
TClear(
    void
)
{
    int loop;
    
    first_delay.prev = NULL;
    first_delay.next = &last_delay;
    
    last_delay.prev = &first_delay;
    last_delay.next = NULL;
    
    free_delay = &tdel[0];
    
    memset(tdel, 0, sizeof(tdel));
    
    for (loop = 0; loop < MAX_TILEDELAY - 1; loop++)
    {
        tdel[loop].next = &tdel[loop + 1];
    }
}

/*-------------------------------------------------------------------------*
TGet () - Get a TDELAY object
 *-------------------------------------------------------------------------*/
tdel_t 
*TGet(
    void
)
{
    tdel_t *newtd;
    
    if (!free_delay)
        EXIT_Error("TILEDELAY_Get() - Max ");
    
    newtd = free_delay;
    free_delay = free_delay->next;
    
    memset(newtd, 0, sizeof(tdel_t));
    
    newtd->next = &last_delay;
    newtd->prev = last_delay.prev;
    last_delay.prev = newtd;
    newtd->prev->next = newtd;
    
    return newtd;
}

/*-------------------------------------------------------------------------*
TRemove () - Remove a TDELAY Object from link list
 *-------------------------------------------------------------------------*/
tdel_t 
*TRemove(
    tdel_t *sh
)
{
    tdel_t *next;
    
    next = sh->prev;
    
    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;
    
    memset(sh, 0, sizeof(tdel_t));
    
    sh->next = free_delay;
    
    free_delay = sh;
    
    return next;
}

/*--------------------------------------------------------------------------
TILE_DoDamage
 --------------------------------------------------------------------------*/
void 
TILE_DoDamage(
    int mapspot, 
    int damage
)
{
    static int mlookup[3] = {
        -1, -MAP_COLS, 1
    };
    static int xlookup[3] = {
        -1, 0, 1
    };
    int loop, ix, spot, x;
    ix = mapspot % MAP_COLS;
    
    for (loop = 0; loop < 3; loop++)
    {
        spot = mapspot + mlookup[loop];
        
        if (spot < 0 || spot >= MAP_SIZE)
            continue;
        
        if (eitems[spot] != titems[spot])
        {
            x = ix + xlookup[loop];
            
            if (x < 0 || x >= MAP_COLS)
                continue;
            
            hits[spot] -= damage;
        }
    }
}

/***************************************************************************
TILE_Put () - Draws 32 by 32 TILE Clips on y only
 ***************************************************************************/
void 
TILE_Put(
    char *inpic,          // INPUT : pointer to GFX_PIC ( norm )
    int x,                // INPUT : x position
    int y                 // INPUT : y position
)
{
    int flag;
    flag = 0;
    
    if (y + 32 <= 0 || y >= SCREENHEIGHT)
        return;
    
    tileloopy = 32;
    tilepic = inpic;
    
    if (y < 0)
    {
        tilepic += y * -32;
        tileloopy += y;
        y = 0;
        flag = 1;
    }
    
    if (y + tileloopy > SCREENHEIGHT)
    {
        tileloopy = SCREENHEIGHT - y;
        flag = 1;
    }
    
    tilestart = displaybuffer + ylookup[y] + x;
    
    if (flag)
        TILE_ClipDraw();
    else
        TILE_Draw();
}

/***************************************************************************
TILE_Init () - Sets Up A level for Displaying
 ***************************************************************************/
void 
TILE_Init(
    void
)
{
    TClear();
    g_mapleft = MAP_LEFT;
    
    scroll_flag = 1;
    last_tile = 0;
    
    // FIND START OF GAME TILES ========
    startflat[0] = GLB_GetItemID(game_start[0]);
    startflat[0]++;
    
    startflat[1] = GLB_GetItemID(game_start[1]);
    startflat[1]++;
    
    startflat[2] = GLB_GetItemID(game_start[2]);
    startflat[2]++;
    
    startflat[3] = GLB_GetItemID(game_start[3]);
    startflat[3]++;
}

/***************************************************************************
TILE_CacheLevel () - Cache tiles in current level
 ***************************************************************************/
void 
TILE_CacheLevel(
    void
)
{
    int loop, game, item;
    flat_t *lib;
    
    TClear();
    g_mapleft = MAP_LEFT;
    
    // SET UP START POS ON MAP =========
    tilepos = (MAP_ROWS - MAP_ONSCREEN) * MAP_COLS;
    tileyoff = 200 - (MAP_ONSCREEN * MAP_BLOCKSIZE);
    lastspot = &tspots[MAX_STILES - 1];
    
    scroll_flag = 1;
    last_tile = 0;
    
    memset(titems, 0, sizeof(titems));
    memset(eitems, 0, sizeof(eitems));
    memset(hits, 0, sizeof(hits));
    memset(tdead, 0, sizeof(tdead));
    
    // == CACHE TILES =========================
    for (loop = 0; loop < MAP_SIZE; loop++)
    {
        game = mapmem->map[loop].fgame;
        lib = flatlib[game];
        
        money[loop] = lib[mapmem->map[loop].flats].bounty;
        
        item = startflat[game];
        item += mapmem->map[loop].flats;
        titems[loop] = item;
        GLB_CacheItem(item);
        
        item = startflat[game];
        item += lib[mapmem->map[loop].flats].linkflat;
        eitems[loop] = item;
        
        if (eitems[loop] != titems[loop])
            GLB_CacheItem(item);
        
        if (eitems[loop] != titems[loop])
            hits[loop] = lib[mapmem->map[loop].flats].bonus;
        else
            hits[loop] = 1;
    }
}

/***************************************************************************
TILE_FreeLevel () - Free tile level
 ***************************************************************************/
void 
TILE_FreeLevel(
    void
)
{
    int loop;
    
    for (loop = 0; loop < MAP_SIZE; loop++)
    {
        GLB_FreeItem(titems[loop]);
        
        if (eitems[loop] != titems[loop])
            GLB_FreeItem(eitems[loop]);
    }
}

/***************************************************************************
TILE_DamageAll () - Damages All tiles on screen
 ***************************************************************************/
void 
TILE_DamageAll(
    void
)
{
    tspot_t *ts;
    ts = tspots;
    
    do
    {
        if (eitems[ts->mapspot] != titems[ts->mapspot])
            hits[ts->mapspot] -= 20;
        
        if (ts == lastspot)
            break;
        
        ts++;
    } while (1);
}

/***************************************************************************
TILE_Think () - Does Position Calculations for tiles
 ***************************************************************************/
void 
TILE_Think(
    void
)
{
    int ty, tx, y, x, mapspot, loopy, loopx;
    tspot_t *ts;
    tdel_t *td;

    y = tileyoff;
    mapspot = tilepos;
    
    ts = tspots;
    
    for (loopy = 0; loopy < MAP_ONSCREEN; loopy++, y += 32)
    {
        x = MAP_LEFT;
        
        for (loopx = 0; loopx < MAP_COLS; loopx++, x += 32, mapspot++, ts++)
        {
            ts->mapspot = mapspot;
            ts->x = x;
            ts->y = y;
            ts->item = titems[mapspot];
            
            if (hits[mapspot] < 0 && !tdead[mapspot])
            {
                SND_3DPatch(FX_GEXPLO, x + 16, y + 16);
                
                TILE_DoDamage(ts->mapspot, 5);
                
                plr.score += money[mapspot];
                
                TILE_Explode(ts, 10);
                ANIMS_StartAnim(A_LARGE_GROUND_EXPLO1, x + 16, y + 16);
                
                tdead[mapspot] = 1;
            }
        }
    }
    
    for (td = first_delay.next; &last_delay != td; td = td->next)
    {
        if (td->mapspot - tilepos > (MAP_ONSCREEN * MAP_COLS))
        {
            td = TRemove(td);
            continue;
        }
        
        if (td->frames < 0)
        {
            tx = td->ts->x + 8 + (wrand() % 8);
            ty = td->ts->y + 26;
            
            TILE_DoDamage(td->mapspot, 20);
            
            spark_delay++;
            flare_delay++;
            
            if (spark_delay > 2)
            {
                ANIMS_StartAnim(A_GROUND_SPARKLE, tx, ty);
                spark_delay = 0;
            }
            
            if (flare_delay > 4)
            {
                ANIMS_StartAnim(A_GROUND_FLARE, tx, ty);
                flare_delay = 0;
            }
            
            ts = &tspots[td->mapspot - tilepos];
            titems[ts->mapspot] = td->item;
            eitems[ts->mapspot] = td->item;
            td = TRemove(td);
        }
        else
            td->frames--;
    }
}

/***************************************************************************
TILE_Display () - Displays Tiles
 ***************************************************************************/
void 
TILE_Display(
    void
)
{
    char *pic;
    tspot_t *ts;

    ts = tspots;
    
    do
    {
        pic = GLB_GetItem(ts->item);
        pic += 20;
        TILE_Put(pic, ts->x, ts->y);
        
        if (ts == lastspot)
            break;
        
        ts++;
    } while (1);
    
    tileyoff++;
    
    if (tileyoff > 0)
    {
        if (last_tile && tileyoff >= 0)
        {
            tileyoff = 0;
            scroll_flag = 0;
        }
        else
        {
            tileyoff -= MAP_BLOCKSIZE;
            tilepos -= MAP_COLS;
        }
        
        if (tilepos <= 0)
        {
            tilepos = 0;
            last_tile = 1;
        }
    }
}

/***************************************************************************
TILE_IsHit () - Checks to see if a shot hits an explodable tile
 ***************************************************************************/
int                        // RETURNS : TRUE = Tile Hit
TILE_IsHit(
    int damage,            // INPUT : damage to tile
    int x,                 // INOUT : x screen pos, out tile x
    int y                  // INOUT : y screen pos, out tile y
)
{
    tspot_t *ts = tspots;
    
    while (ts != lastspot)
    {
        if (x >= ts->x && ts->x + 32 > x && y >= ts->y && ts->y + 32 > y)
        {
            if (eitems[ts->mapspot] != titems[ts->mapspot])
            {
                hits[ts->mapspot] -= damage;
                
                switch (wrand() % 2)
                {
                case 0:
                    ANIMS_StartGAnim(A_BLUE_SPARK, x, y);
                    break;
                case 1:
                    ANIMS_StartGAnim(A_ORANGE_SPARK, x, y);
                    break;
                }
                
                return 1;
            }
        }
        
        ts++;
    }
    
    return 0;
}

/***************************************************************************
TILE_Bomb () - Checks to see if a BOMB hits an explodable tile
 ***************************************************************************/
int                        // RETURNS : TRUE = Tile Hit
TILE_Bomb(
    int damage,            // INPUT : damage to tile 
    int x,                 // INOUT : x screen pos, out tile x
    int y                  // INOUT : y screen pos, out tile y
)
{
    tspot_t *ts = tspots;
    
    while (ts != lastspot)
    {
        if (x >= ts->x && ts->x + 32 > x && y >= ts->y && ts->y + 32 > y)
        {
            if (eitems[ts->mapspot] != titems[ts->mapspot])
            {
                hits[ts->mapspot] -= damage;
                
                TILE_DoDamage(ts->mapspot, damage);
                
                if (ts->mapspot > MAP_COLS)
                    TILE_DoDamage(ts->mapspot - MAP_COLS, damage >> 1);
                
                return 1;
            }
        }
        
        ts++;
    }
    
    return 0;
}

/***************************************************************************
TILE_Explode () - Sets the Tile to show explosion tile
 ***************************************************************************/
void 
TILE_Explode(
    tspot_t *ts,           // INPUT : tilespot of explosion
    int delay              // INPUT : frames to delay
)
{
    tdel_t *td;
    
    if (ts->mapspot != -1 && ts->mapspot >= tilepos)
    {
        if (delay)
        {
            td = TGet();
            
            if (td)
            {
                td->ts = ts;
                td->mapspot = ts->mapspot;
                td->frames = delay;
                td->item = eitems[ts->mapspot];
            }
            
            eitems[ts->mapspot] = titems[ts->mapspot];
        }
        else
        {
            ts->item = eitems[ts->mapspot];
            titems[ts->mapspot] = eitems[ts->mapspot];
        }
    }
}

