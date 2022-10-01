#include <string.h>
#include <stdlib.h>
#include "bonus.h"
#include "rap.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "objects.h"
#include "fx.h"
#include "fileids.h"

bonus_t bons[MAX_BONUS];
bonus_t first_bonus, last_bonus;
bonus_t *free_bonus;
static int energy_count;
static int glow[4];
static int glow_lx, glow_ly;
static int xpos[16] = {
    -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2
};

static int ypos[16] = {
    -3, -3, -3, -2, -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2
};

/***************************************************************************
BONUS_Clear () - Clears out All bonuses
 ***************************************************************************/
void 
BONUS_Clear(
    void
)
{
    int loop;
    
    energy_count = 0;
    
    first_bonus.prev = NULL;
    first_bonus.next = &last_bonus;
    
    last_bonus.prev = &first_bonus;
    last_bonus.next = NULL;
    
    free_bonus = bons;
    
    memset(bons, 0, sizeof(bons));
    
    for (loop = 0; loop < MAX_BONUS - 1; loop++)
        bons[loop].next = &bons[loop + 1];
}

/*-------------------------------------------------------------------------*
BONUS_Get () - Gets A Free BONUS from Link List
 *-------------------------------------------------------------------------*/
bonus_t 
*BONUS_Get(
    void
)
{
    bonus_t *newb;
    
    if (!free_bonus)
        return NULL;
    
    newb = free_bonus;
    free_bonus = free_bonus->next;
    
    memset(newb, 0, sizeof(bonus_t));
    
    newb->next = &last_bonus;
    newb->prev = last_bonus.prev;
    last_bonus.prev = newb;
    newb->prev->next = newb;
    
    return newb;
}

/*-------------------------------------------------------------------------*
BONUS_Remove () Removes BONUS from Link List
 *-------------------------------------------------------------------------*/
bonus_t 
*BONUS_Remove(
    bonus_t *sh
)
{
    bonus_t *next;
    
    if (sh->type == S_ITEMBUY6)
        energy_count--;
    
    next = sh->prev;
    
    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;
    
    memset(sh, 0, sizeof(bonus_t));
    
    sh->next = free_bonus;
    
    free_bonus = sh;
    
    return next;
}

/***************************************************************************
BONUS_Init () - Sets up Bonus stuff
 ***************************************************************************/
void 
BONUS_Init(
    void
)
{
    int loop;
    texture_t *h;

    for (loop = 0; loop < 4; loop++)
    {
        glow[loop] = FILE125_ICNGLW_BLK + loop;
    }
    
    h = (texture_t*)GLB_CacheItem(FILE125_ICNGLW_BLK);
    
    glow_lx = h->width;
    glow_ly = h->height;
    
    GLB_CacheItem(FILE126_ICNGLW_BLK);
    GLB_CacheItem(FILE127_ICNGLW_BLK);
    GLB_CacheItem(FILE128_ICNGLW_BLK);
    
    BONUS_Clear();
}

/***************************************************************************
BONUS_Add () - Adds A BONUS to Game so player can Try to pick it up
 ***************************************************************************/
void 
BONUS_Add(
    int type,              // INPUT : OBJECT TYPE
    int x,                 // INPUT : X POSITION
    int y                  // INPUT : Y POSITION
)
{
    bonus_t *cur;
    
    if (type >= S_LAST_OBJECT)
        return;
    
    if (type == S_ITEMBUY6 && energy_count > MAX_MONEY)
        return;
    
    cur = BONUS_Get();
    
    if (!cur)
        return;
    
    if (type == S_ITEMBUY6)
        energy_count++;
    
    cur->type = type;
    cur->lib = OBJS_GetLib(type);
    cur->curframe = 0;
    cur->x = MAP_LEFT + x;
    cur->y = y;
    cur->pos = wrand() % 16;
}

/***************************************************************************
BONUS_Think () - Does all BONUS Thinking
 ***************************************************************************/
void 
BONUS_Think(
    void
)
{
    int x, y, x2, y2;
    bonus_t *cur;
    static int gcnt;

    x = playerx;
    y = playery;
    x2 = playerx + PLAYERWIDTH;
    y2 = playery + PLAYERHEIGHT;
    
    for (cur = first_bonus.next; &last_bonus != cur; cur = cur->next)
    {
        cur->item = cur->lib->item + cur->curframe;
        
        cur->bx = cur->x - (BONUS_WIDTH / 2) + xpos[cur->pos];
        cur->by = cur->y - (BONUS_HEIGHT / 2) + ypos[cur->pos];
        
        cur->gx = cur->x - (glow_lx>>1) + xpos[cur->pos];
        cur->gy = cur->y - (glow_ly>>1) + ypos[cur->pos];
        
        cur->y++;
        
        if (gcnt & 1)
        {
            cur->pos++;
            
            if (cur->pos >= 16)
                cur->pos = 0;
            
            cur->curframe++;
            
            if (cur->curframe >= cur->lib->numframes)
                cur->curframe = 0;
        }
        
        cur->curglow++;
        
        if (cur->curglow >= 4)
            cur->curglow = 0;
        
        if (cur->x > x && cur->x < x2 && cur->y > y && cur->y < y2)
        {
            if (!cur->dflag && OBJS_GetAmt(S_ENERGY) > 0)
            {
                SND_Patch(FX_BONUS, 127);
                
                if (cur->type == S_ENERGY)
                    OBJS_AddEnergy(MAX_SHIELD / 4);
                else
                    OBJS_Add(cur->type);
                
                if (cur->lib->moneyflag)
                {
                    cur->dflag = 1;
                    cur->countdown = 50;
                }
                else
                {
                    cur = BONUS_Remove(cur);
                    continue;
                }
            }
        }
        
        if (cur->dflag)
        {
            cur->countdown--;
            if (cur->countdown <= 0)
            {
                cur = BONUS_Remove(cur);
                continue;
            }
        }
        
        if (cur->gy > 200)
        {
            cur = BONUS_Remove(cur);
            continue;
        }
    }
    
    gcnt++;
}

/***************************************************************************
BONUS_Display () - Displays Active Bonuses in game
 ***************************************************************************/
void 
BONUS_Display(
    void
)
{
    bonus_t *cur;
    
    for (cur = first_bonus.next; &last_bonus != cur; cur = cur->next)
    {
        if (!cur->dflag)
        {
            GFX_PutSprite((texture_t*)GLB_GetItem(cur->item), cur->bx, cur->by);
            GFX_ShadeShape(1, (texture_t*)GLB_GetItem(glow[cur->curglow]), cur->gx, cur->gy);
        }
        else
            GFX_PutSprite((texture_t*)GLB_GetItem(FILE10f_N$_PIC), cur->bx, cur->by);
    }
}

