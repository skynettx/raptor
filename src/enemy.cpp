#include <stdio.h>
#include <string.h>
#include "common.h"
#include "enemy.h"
#include "gfxapi.h"
#include "rap.h"
#include "glbapi.h"
#include "tile.h"
#include "loadsave.h"
#include "fx.h"
#include "eshot.h"
#include "shadow.h"
#include "anims.h"
#include "objects.h"
#include "bonus.h"
#include "flame.h"
#include "joyapi.h"
#include "input.h"
#include "fileids.h"

#define NORM_SHOOT  -1
#define START_SHOOT   0

int spriteflag[4];
int spriteitm[4] = {
     FILE1fc_SPRITE1_ITM, FILE200_SPRITE2_ITM, FILE300_SPRITE3_ITM, FILE400_SPRITE4_ITM
};

slib_t *slib[4];

int cur_visable;
int boss_sound;
int g_numslibs;
int numslibs[4];

int numboss, numships;
int end_waveflag;

enemy_t ships[30];
enemy_t last_enemy, first_enemy;
enemy_t *free_enemy;

csprite_t *end_enemy, *cur_enemy;

int tiley;

enemy_t *onscreen[MAX_ONSCREEN], *rscreen[MAX_ONSCREEN];

/***************************************************************************
   MoveEobj() - gets next postion for an Object at speed
 ***************************************************************************/
int 
MoveEobj(
    mobj_t *cur,           // INPUT : pointer to MOVEOBJ
    int speed              // INPUT : speed to plot at
)      
{
    if (!speed)
        return 0;
    
    if (cur->delx >= cur->dely)
    {
        while (speed)
        {
            speed--;
            cur->maxloop--;
            
            if (cur->maxloop == 0)
            {
                cur->done = 1;
                return speed;
            }
            
            cur->x += cur->addx;
            cur->err += cur->dely;
            
            if (cur->err > 0)
            {
                cur->y += cur->addy;
                cur->err -= cur->delx;
            }
        }
    }
    else
    {
        while (speed)
        {
            speed--;
            cur->maxloop--;
            
            if (cur->maxloop == 0)
            {
                cur->done = 1;
                return speed;
            }
            
            cur->y += cur->addy;
            cur->err += cur->delx;
            
            if (cur->err > 0)
            {
                cur->x += cur->addx;
                cur->err -= cur->dely;
            }
        }
    }
    
    if (cur->maxloop < 1)
        cur->done = 1;
    
    return speed;
}

/***************************************************************************
ENEMY_FreeSprites () - Free Memory Used by Sprites use in last level
 ***************************************************************************/
void 
ENEMY_FreeSprites(
    void
)
{
    int loop, i;
    csprite_t *curfld;
    slib_t *curlib;

    for (loop = 0; loop < 4; loop++)
    {
        if (spriteflag[loop])
            GLB_FreeItem(spriteitm[loop]);
    }
    
    for (loop = 0; loop < mapmem->numsprites; loop++)
    {
        curfld = &csprite[loop];
        curlib = &slib[csprite[loop].game][csprite[loop].slib];
        
        if (cur_diff & curfld->level)
        {
            for (i = 0; i < curlib->num_frames; i++)
            {
                GLB_FreeItem(curlib->item + i);
            }
        }
    }
}

/***************************************************************************
ENEMY_LoadSprites() -
 ***************************************************************************/
void 
ENEMY_LoadSprites(
    void
)
{
    int loop, i, item;
    csprite_t *curfld;
    slib_t *curlib;
    
    ENEMY_Clear();
    cur_visable = 0;
    boss_sound = 0;
    
    for (loop = 0; loop < mapmem->numsprites; loop++)
    {
        curfld = &csprite[loop];
        curlib = &slib[csprite[loop].game][csprite[loop].slib];
        curlib->item = GLB_GetItemID(curlib->iname);
        
        switch (curfld->level)
        {
        default:
            curfld->level = EB_NOT_USED;
            break;
        
        case E_SECRET_1:
            curfld->level = EB_SECRET_1;
            break;
        
        case E_SECRET_2:
            curfld->level = EB_SECRET_2;
            break;
        
        case E_SECRET_3:
            curfld->level = EB_SECRET_3;
            break;
        
        case E_EASY_LEVEL:
            curfld->level = EB_EASY_LEVEL;
            break;
        
        case E_MED_LEVEL:
            curfld->level = EB_MED_LEVEL;
            break;
        
        case E_HARD_LEVEL:
            curfld->level = EB_HARD_LEVEL;
            break;
        }
        
        if (cur_diff & curfld->level)
        {
            if (curlib->item != -1)
            {
                for (i = 0; i < curlib->num_frames; i++)
                {
                    item = curlib->item + i;
                    
                    GLB_CacheItem(item);
                }
            }
            else
            {
                curfld->level = EB_NOT_USED;
            }
        }
        else
            curfld->level = EB_NOT_USED;
    }
}

/***************************************************************************
ENEMY_LoadLib () - Loads and Locks spritelib's MUST becalled b4 LoadSprites
 ***************************************************************************/
void ENEMY_LoadLib(
    void
)
{
    int loop;
    
    memset(spriteflag, 0, sizeof(spriteflag));
    
    for (loop = 0; loop < mapmem->numsprites; loop++)
    {
        spriteflag[csprite[loop].game] = 1;
    }
    g_numslibs = 0;
    for (loop = 0; loop < 4; loop++)
    {
        slib[loop] = NULL;
        numslibs[loop] = 0;
        
        if (spriteflag[loop])
            g_numslibs++;
    }
    
    if (g_numslibs > 1 && !gameflag[2] && !gameflag[3])
        EXIT_Error("ENEMY_LoadSprites() - F:%d  G1:%d G2:%d G3:%d G4:%d", g_numslibs, spriteflag[0], spriteflag[1], spriteflag[2], spriteflag[3]);
    
    for (loop = 0; loop < 4; loop++)
    {
        if (spriteflag[loop])
        {
            slib[loop] = (slib_t*)GLB_LockItem(spriteitm[loop]);
            
            if (!slib[loop])
                EXIT_Error("ENEMY_LoadSprites() - memory");
            
            numslibs[loop] = GLB_GetItemSize(spriteitm[loop]);
            numslibs[loop] /= sizeof(slib_t);
        }
    }
}

/***************************************************************************
ENEMY_Clear()
 ***************************************************************************/
void ENEMY_Clear(
    void
)
{
    int loop;
    
    numboss = 0;
    numships = 0;
    end_waveflag = 0;
    
    first_enemy.prev = NULL;
    first_enemy.next = &last_enemy;
    
    last_enemy.prev = &first_enemy;
    last_enemy.next = NULL;
    
    free_enemy = ships;
    
    memset(ships, 0, sizeof(ships));
    
    for (loop = 0; loop < MAX_ONSCREEN - 1; loop++)
    {
        ships[loop].next = &ships[loop + 1];
    }
    if (mapmem->numsprites)
    {
        end_enemy = csprite + mapmem->numsprites - 1;
        cur_enemy = csprite;
    }
    else
    {
        end_enemy = NULL;
        cur_enemy = NULL;
    }
}

/*-------------------------------------------------------------------------*
ENEMY_Get() - Gets An Free Enemy from link list
 *-------------------------------------------------------------------------*/
enemy_t 
*ENEMY_Get(
    void
)
{
    enemy_t *sh;
    
    if (!free_enemy)
        EXIT_Error("ENEMY_Get() - Max Sprites");
    
    numships++;
    
    sh = free_enemy;
    free_enemy = free_enemy->next;
    
    memset(sh, 0, sizeof(enemy_t));
    
    sh->next = &last_enemy;
    sh->prev = last_enemy.prev;
    last_enemy.prev = sh;
    sh->prev->next = sh;
    
    return sh;
}

/*-------------------------------------------------------------------------*
ENEMY_Remove () - Removes an Enemy OBJECT from linklist
 *-------------------------------------------------------------------------*/
enemy_t 
*ENEMY_Remove(
    enemy_t *sh
)
{
    enemy_t *next;
    
    if (sh->lib->bossflag)
        numboss--;
    
    numships--;
    
    if (end_waveflag && numships < 1)
        startendwave = END_DURATION;
    
    next = sh->prev;
    
    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;
    
    memset(sh, 0, sizeof(enemy_t));
    sh->item = -1;
    
    sh->next = free_enemy;
    
    free_enemy = sh;
    
    return next;
}

/*-------------------------------------------------------------------------*
ENEMY_Add () - Adds Enemy to attack player
 *-------------------------------------------------------------------------*/
void 
ENEMY_Add(
    csprite_t *sprite
)
{
    slib_t *curlib;
    enemy_t *newe;
    char *pic;
    texture_t *h;
    curlib = &slib[sprite->game][sprite->slib];
    
    newe = ENEMY_Get();
    pic = GLB_GetItem(curlib->item);
    h = (texture_t*)pic;
    
    newe->item = curlib->item;
    newe->width = h->width;
    newe->height = h->height;
    newe->hlx = h->width >> 1;
    newe->hly = h->height >> 1;
    
    newe->kami = KAMI_FLY;
    newe->hits = curlib->hits;
    newe->lib = &slib[sprite->game][sprite->slib];
    newe->y = tileyoff - (tiley - sprite->y) * 32 - 97;
    newe->x = sprite->x * 32 + MAP_LEFT;
    
    newe->edir = E_FORWARD;
    newe->x += 16;
    newe->y += 16;
    newe->x -= newe->hlx;
    newe->y -= newe->hly;
    newe->x2 = newe->x + newe->width;
    newe->y2 = newe->y + newe->height;
    newe->mobj.x = newe->sx = newe->x;
    newe->mobj.y = newe->sy = newe->y;
    newe->frame_rate = curlib->frame_rate;
    newe->speed = curlib->movespeed;
    
    newe->countdown = curlib->countdown - newe->mobj.y;
    newe->shoot_disable = 0;
    newe->shoot_on = 0;
    newe->shootagain = NORM_SHOOT;
    newe->shootcount = curlib->shootcnt;
    newe->shootflag = curlib->shootstart;
    
    if (curlib->bossflag && curplr_diff <= DIFF_1)
    {
        newe->hits -= newe->hits >> 1;
        newe->shootcount -= newe->shootcount >> 2;
    }
    
    switch (curlib->animtype)
    {
    default:
        EXIT_Error("ENEMY_Add() - Invalid ANIMTYPE");
        break;
    case GANIM_NORM:
        newe->anim_on = 1;
        newe->num_frames = curlib->num_frames;
        break;
    
    case GANIM_SHOOT:
        newe->anim_on = 0;
        newe->num_frames = curlib->num_frames;
        break;
    
    case GANIM_MULTI:
        newe->anim_on = 1;
        newe->num_frames = curlib->rewind;
        break;
    }
    
    switch (curlib->flighttype)
    {
    case F_REPEAT:
    case F_LINEAR:
    case F_KAMI:
        newe->groundflag = 0;
        newe->sy = 100 - newe->hly;
        newe->mobj.x2 = newe->sx + curlib->flightx[0];
        newe->mobj.y2 = newe->sy + curlib->flighty[0];
        newe->movepos = 1;
        InitMobj(&newe->mobj);
        MoveMobj(&newe->mobj);
        break;
    
    case F_GROUND:                                          
        newe->groundflag = 1;
        newe->mobj.x2 = newe->x;
        newe->mobj.y2 = 211;
        break;
    
    case F_GROUNDRIGHT:                                           
        newe->x -= newe->width;
        newe->mobj.x = newe->sx = newe->x;
        newe->groundflag = 1;
        newe->mobj.x2 = 335;
        newe->mobj.y2 = 211;
        break;
    
    case F_GROUNDLEFT:                                           
        newe->x += newe->width;
        newe->mobj.x = newe->sx = newe->x;
        newe->groundflag = 1;
        newe->mobj.x2 = -newe->hlx;
        newe->mobj.y2 = 211;
        break;
    }
    
    newe->suckagain = curlib->hits >> 4;
    
    if (curlib->song != -1)
        boss_sound = 1;
}

/***************************************************************************
ENEMY_GetRandom () - Returns a random ship thats visable
 ***************************************************************************/
enemy_t 
*ENEMY_GetRandom(
    void
)
{
    int pos;
    
    if (!cur_visable)
        return NULL;
    
    pos = wrand() % cur_visable;
    
    return onscreen[pos];
}

/***************************************************************************
ENEMY_GetRandomAir () - Returns a random ship thats visable
 ***************************************************************************/
enemy_t 
*ENEMY_GetRandomAir(
    void
)
{
    int pos;
    int loop;
    
    if (!cur_visable)
        return NULL;
    
    pos = 0;
    for (loop = 0; loop < cur_visable; loop++)
    {
        if (onscreen[loop]->groundflag)
            continue;
        
        rscreen[pos] = onscreen[loop];
        pos++;
    }
    
    if (pos > 0)
    {
        pos = wrand() % pos;
        return rscreen[pos];
    }
    
    return NULL;
}

/***************************************************************************
ENEMY_DamageAll () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
int 
ENEMY_DamageAll(
    int x,           // INPUT : x position            
    int y,           // INPUT : y position
    int damage       // INPUT : damage
)
{
    int loop;
    enemy_t *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits -= damage;
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************
ENEMY_DamageGround () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
int 
ENEMY_DamageGround(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int damage             // INPUT : damage
)
{
    int loop;
    enemy_t *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (!cur->groundflag)
            continue;
        
        if (x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits -= damage;
            if (curplr_diff == DIFF_0)
                cur->hits -= damage;
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************
ENEMY_DamageAir () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
int 
ENEMY_DamageAir(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int damage             // INPUT : damage
)
{
    int loop;
    enemy_t *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (cur->groundflag)
            continue;
        
        if (x > cur->x && x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits -= damage;
            if (curplr_diff == DIFF_0)
                cur->hits -= damage;
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************
ENEMY_DamageEnergy () - Tests to see if hit occured at x/y and applys damage
 ***************************************************************************/
enemy_t 
*ENEMY_DamageEnergy(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int damage             // INPUT : damage
)
{
    int loop;
    enemy_t *cur;
    
    for (loop = 0; loop < cur_visable; loop++)
    {
        cur = onscreen[loop];
        
        if (cur->groundflag)
            continue;
        
        if (x > cur->x&& x < cur->x2 && y > cur->y && y < cur->y2)
        {
            cur->hits--;
            
            if (cur->lib->suck)
            {
                if (cur->suckagain > 0)
                {
                    cur->suckagain -= damage;
                }
                else
                {
                    cur->shoot_on = 0;
                    cur->shoot_disable = 1;
                    cur->shootagain = NORM_SHOOT;
                    SND_3DPatch(FX_EGRAB, cur->x + cur->hlx, cur->y + cur->hlx);
                }
            }
            return cur;
        }
    }
    
    return NULL;
}

/***************************************************************************
ENEMY_Think() - Does all thinking for enemy ships ( ground/air )
 ***************************************************************************/
void ENEMY_Think(
    void
)
{
    csprite_t *old_enemy;
    enemy_t *sprite;
    slib_t *curlib;
    int speed, loop, x, y, suben, area;
    
    if (boss_sound)
    {
        if (!SND_IsPatchPlaying(FX_BOSS1))
            SND_Patch(FX_BOSS1, 127);
    }
    
    tiley = tilepos / MAP_COLS - 3;
    
    while (!end_waveflag && cur_enemy->y == tiley)
    {
        do
        {
            old_enemy = cur_enemy;
            
            if (cur_enemy->level != EB_NOT_USED)
                ENEMY_Add(cur_enemy);
            
            if (cur_enemy == end_enemy)
            {
                end_waveflag = 1;
                break;
            }
            
            cur_enemy++;
        } while (old_enemy->link != -1 && old_enemy->link != 1);
    }
    
    cur_visable = 0;
    
    for (sprite = first_enemy.next; &last_enemy != sprite; sprite = sprite->next)
    {
        curlib = sprite->lib;
        
        if (curlib->num_frames > 1)
        {
            sprite->item = curlib->item + sprite->curframe;
            
            if (sprite->frame_rate < 1)
            {
                sprite->frame_rate = curlib->frame_rate;
                
                if (sprite->anim_on)
                {
                    sprite->curframe++;
                    if ((unsigned int)sprite->curframe >= (unsigned int)sprite->num_frames)
                    {
                        sprite->curframe -= curlib->rewind;
                        switch (curlib->animtype)
                        {
                        default:
                            EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE1");
                        case GANIM_NORM:
                            break;
                        
                        case GANIM_SHOOT:
                            sprite->anim_on = 0;
                            sprite->shoot_on = 1;
                            break;
                        
                        case GANIM_MULTI:
                            switch (sprite->multi)
                            {
                            case MULTI_START:
                                sprite->num_frames = curlib->num_frames;
                                sprite->multi = 2;
                                break;
                            
                            case MULTI_END:
                                sprite->shoot_on = 1;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
                sprite->frame_rate--;
            
            if (sprite->countdown < 1)
            {
                switch (curlib->animtype)
                {
                default:
                    EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE2");
                case GANIM_NORM:
                    sprite->shoot_on = 1;
                    break;
                
                case GANIM_SHOOT:
                    sprite->anim_on = 1;
                    break;
                
                case GANIM_MULTI:
                    if (!sprite->multi)
                        sprite->multi = MULTI_START;
                    break;
                }
            }
            else
                sprite->countdown -= curlib->movespeed;
        }
        else
        {
            if (sprite->countdown < 1)
            {
                sprite->countdown = -1;
                sprite->shoot_on = 1;
            }
            else
                sprite->countdown -= curlib->movespeed;
        }
        
        switch (curlib->flighttype)
        {
        case F_REPEAT:
            sprite->x = sprite->mobj.x;
            sprite->y = sprite->mobj.y;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            
            speed = curlib->movespeed;
            
            speed = MoveEobj(&sprite->mobj, speed);
            
            if (sprite->mobj.done)
            {
                sprite->mobj.x = sprite->mobj.x2;
                sprite->mobj.y = sprite->mobj.y2;
                sprite->mobj.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                sprite->mobj.y2 = sprite->sy + curlib->flighty[sprite->movepos];
                
                InitMobj(&sprite->mobj);
                MoveMobj(&sprite->mobj);
                
                speed = MoveEobj(&sprite->mobj, speed);
                
                if (!sprite->edir)
                {
                    sprite->movepos++;
                    if (sprite->movepos >= curlib->numflight)
                    {
                        sprite->edir = E_BACKWARD;
                        sprite->movepos = curlib->numflight - 1;
                    }
                }
                else
                {
                    sprite->movepos--;
                    if (sprite->movepos <= curlib->repos)
                    {
                        sprite->edir = E_FORWARD;
                        sprite->movepos = curlib->repos;
                    }
                }
            }
            break;
        
        case F_KAMI:
            sprite->x = sprite->mobj.x;
            sprite->y = sprite->mobj.y;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            
            speed = curlib->movespeed;
            speed = MoveEobj(&sprite->mobj, speed);
            
            if (sprite->kami == KAMI_END)
            {
                if (sprite->mobj.y > 0xc9)
                    sprite->doneflag = 1;
                
                if (sprite->hlx + 320 < sprite->mobj.x)
                    sprite->doneflag = 1;
                
                if (sprite->mobj.y + sprite->width < 0)
                    sprite->doneflag = 1;
                
                if (sprite->mobj.x + sprite->width < 0)
                    sprite->doneflag = 1;
            }
            
            if (sprite->mobj.done && sprite->kami != KAMI_END)
            {
                sprite->mobj.x = sprite->mobj.x2;
                sprite->mobj.y = sprite->mobj.y2;
                sprite->x2 = sprite->x + sprite->width - 1;
                sprite->y2 = sprite->y + sprite->height - 1;
                
                if (sprite->kami == KAMI_CHASE)
                {
                    sprite->mobj.x2 = player_cx;
                    sprite->mobj.y2 = player_cy;
                    sprite->kami = KAMI_END;
                }
                else
                {
                    sprite->mobj.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                    sprite->mobj.y2 = sprite->sy + curlib->flighty[sprite->movepos];
                }
                
                InitMobj(&sprite->mobj);
                MoveMobj(&sprite->mobj);
                speed = MoveEobj(&sprite->mobj, speed);
                
                if (curlib->numflight - 1 > sprite->movepos)
                    sprite->movepos++;
                else if (sprite->kami == KAMI_FLY)
                    sprite->kami = KAMI_CHASE;
            }
            break;
        
        case F_LINEAR:
            sprite->x = sprite->mobj.x;
            sprite->y = sprite->mobj.y;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            
            speed = curlib->movespeed;
            
            speed = MoveEobj(&sprite->mobj, speed);
            
            if (sprite->mobj.done)
            {
                sprite->mobj.x = sprite->mobj.x2;
                sprite->mobj.y = sprite->mobj.y2;
                sprite->mobj.x2 = sprite->sx + curlib->flightx[sprite->movepos];
                sprite->mobj.y2 = sprite->sy + curlib->flighty[sprite->movepos];
                
                InitMobj(&sprite->mobj);
                MoveMobj(&sprite->mobj);
                
                speed = MoveEobj(&sprite->mobj, speed);
                
                sprite->movepos++;
                
                if (sprite->movepos > curlib->numflight)
                    sprite->doneflag = 1;
            }
            break;
        
        case F_GROUND:
            if (scroll_flag)
                sprite->y++;
            if (sprite->y > sprite->mobj.y2)
                sprite->doneflag = 1;
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            break;
        
        case F_GROUNDRIGHT:
            if (scroll_flag)
                sprite->y++;
            if (sprite->y >= 0)
            {
                sprite->x += curlib->movespeed;
                if (sprite->x > sprite->mobj.x2)
                    sprite->doneflag = 1;
                else if (sprite->y > sprite->mobj.y2)
                    sprite->doneflag = 1;
            }
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            break;
        
        case F_GROUNDLEFT:
            if (scroll_flag)
                sprite->y++;
            if (sprite->y >= 0)
            {
                sprite->x -= curlib->movespeed;
                if (sprite->x < sprite->mobj.x2)
                    sprite->doneflag = 1;
                else if (sprite->y > sprite->mobj.y2)
                    sprite->doneflag = 1;
            }
            sprite->x2 = sprite->x + sprite->width - 1;
            sprite->y2 = sprite->y + sprite->height - 1;
            break;
        }
        
        if (sprite->shoot_on)
        {
            switch (sprite->shootagain)
            {
            case NORM_SHOOT:
                sprite->shootflag--;
                
                if (sprite->shootflag < 0)
                {
                    sprite->shootflag = curlib->shootspace;
                    
                    if (sprite->shoot_disable == 0)
                    {
                        for (loop = 0; loop < curlib->numguns; loop++)
                        {
                            ESHOT_Shoot(sprite, loop);
                        }
                    }
                    sprite->shootcount--;
                    if (sprite->shootcount < 1)
                        sprite->shootagain = curlib->shootframe;
                }
                break;
            
            case START_SHOOT:
                sprite->shootagain = NORM_SHOOT;
                sprite->shootcount = curlib->shootcnt;
                sprite->shootflag = curlib->shootspace;
                break;
            
            default:
                sprite->shootagain--;
                break;
            }
        }
        
        if (sprite->doneflag)
        {
            sprite = ENEMY_Remove(sprite);
            continue;
        }
        
        if (curlib->shadow)
        {
            if (sprite->groundflag)
            {
                SHADOW_GAdd(sprite->item, sprite->x, sprite->y);
            }
            else
            {
                SHADOW_Add(sprite->item, sprite->x, sprite->y);
            }
        }
        
        if (curlib->bossflag)
        {
            numboss++;
            if (sprite->hits < 50 && (gl_cnt & 2) != 0)
            {
                x = sprite->x + (wrand() % sprite->width);
                y = sprite->y + (wrand() % sprite->height);
                ANIMS_StartAnim(A_SMALL_AIR_EXPLO, x, y);
            }
        }
        
        if (!sprite->groundflag)
        {
            if (player_cx > sprite->x && player_cx < sprite->x2)
            {
                if (player_cy > sprite->y && player_cy < sprite->y2)
                {
                    if ((haptic) && (control == 2))
                    {
                        IPT_CalJoyRumbleMedium();                                                            //Rumble when enemy hit
                    }
                    sprite->hits -= (PLAYERWIDTH / 2);
                    if (sprite->width > sprite->height)
                        suben = sprite->width;
                    else
                        suben = sprite->height;
                    
                    OBJS_SubEnergy(suben >> 2);
                    x = player_cx + (wrand() % 8) - 4;
                    y = player_cy + (wrand() % 8) - 4;
                    ANIMS_StartAnim(A_SMALL_AIR_EXPLO, x, y);
                    SND_Patch(FX_CRASH, 127);
                }
            }
        }
        
        if (sprite->hits <= 0)
        {
            player.score += curlib->money;
            
            SND_3DPatch(FX_AIREXPLO, sprite->x + sprite->hlx, sprite->x + sprite->hlx);
            
            switch (curlib->exptype)
            {
            case EXP_ENERGY:
                ANIMS_StartAnim(A_ENERGY_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                BONUS_Add(S_ITEMBUY6, sprite->x, sprite->y);
                break;
            
            case EXP_AIRSMALL1:
                ANIMS_StartAnim(A_MED_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_AIRSMALL2:
                ANIMS_StartAnim(A_MED_AIR_EXPLO2, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_AIRMED:
                ANIMS_StartAnim(A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_AIRLARGE:
                ANIMS_StartAnim(A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                area = (sprite->width>>4) * (sprite->height>>4);
                for (loop = 0; loop < area; loop++)
                {
                    x = sprite->x + (wrand() % sprite->width);
                    y = sprite->y + (wrand() % sprite->height);
                    if (loop & 1)
                        ANIMS_StartAnim(A_MED_AIR_EXPLO, x, y);
                    else
                        ANIMS_StartAAnim(A_MED_AIR_EXPLO2, x, y);
                }
                break;
            
            case EXP_GRDSMALL:
                ANIMS_StartAnim(A_SMALL_GROUND_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_GRDMED:
                x = sprite->x + (wrand() % sprite->width);
                y = sprite->y + (wrand() % sprite->height);
                if ((wrand() % 2) == 0)
                    ANIMS_StartAnim(A_GROUND_SPARKLE, x, y);
                else
                    ANIMS_StartAnim(A_GROUND_FLARE, x, y);
                ANIMS_StartAnim(A_LARGE_GROUND_EXPLO1, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_GRDLARGE:
                ANIMS_StartAnim(A_LARGE_GROUND_EXPLO1, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                area = (sprite->width>>4) * (sprite->height>>4);
                for (loop = 0; loop < area; loop++)
                {
                    x = sprite->x + (wrand() % sprite->width);
                    y = sprite->y + (wrand() % sprite->height);
                    
                    if ((wrand()%2) == 0)
                        ANIMS_StartAnim(A_GROUND_FLARE, x, y);
                    else
                        ANIMS_StartAnim(A_GROUND_SPARKLE, x, y);
                    
                    ANIMS_StartAnim(A_SMALL_GROUND_EXPLO, x, y);
                }
                break;
            
            case EXP_BOSS:
                ANIMS_StartAnim(A_LARGE_AIR_EXPLO, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                area = (sprite->width>>4) * (sprite->height>>4);
                for (loop = 0; loop < area; loop++)
                {
                    x = sprite->x + (wrand() % sprite->width);
                    y = sprite->y + (wrand() % sprite->height);
                    ANIMS_StartAAnim(A_GROUND_FLARE, x, y);
                    if (loop & 1)
                        ANIMS_StartAnim(A_LARGE_AIR_EXPLO, x, y);
                    else
                        ANIMS_StartAnim(A_MED_AIR_EXPLO2, x, y);
                }
                break;
            
            case EXP_PERSON:
                ANIMS_StartAnim(A_PERSON, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            
            case EXP_PLATOON:
                ANIMS_StartAnim(A_PLATOON, sprite->x + sprite->hlx, sprite->y + sprite->hly);
                break;
            }
            
            if (curlib->bonus != -1)
                BONUS_Add(curlib->bonus, sprite->x, sprite->y);
            
            sprite = ENEMY_Remove(sprite);
            
            continue;
        }
        
        y = sprite->y + sprite->height;
        
        if (y > 0 && sprite->y < 200)
        {
            x = sprite->x + sprite->width;
            
            if (x > 0 && sprite->x < 320)
            {
                onscreen[cur_visable] = sprite;
                cur_visable++;
            }
        }
    }
}

/***************************************************************************
ENEMY_DisplayGround () - Displays Ground ENEMY pics
 ***************************************************************************/
void 
ENEMY_DisplayGround(
    void
)
{
    enemy_t *spt;
    
    for (spt = first_enemy.next; &last_enemy != spt; spt = spt->next)
    {
        if (!spt->groundflag)
            continue;
        GFX_PutSprite((texture_t*)GLB_GetItem(spt->item), spt->x, spt->y);
    }
}

/***************************************************************************
ENEMY_DisplaySky () - Displays AIR ENEMY SHIPS
 ***************************************************************************/
void 
ENEMY_DisplaySky(
    void
)
{
    int i;
    enemy_t *spt;
    
    for (spt = first_enemy.next; &last_enemy != spt; spt = spt->next)
    {
        if (spt->groundflag)
            continue;
        
        GFX_PutSprite((texture_t*)GLB_GetItem(spt->item), spt->x, spt->y);
        
        for (i = 0; i < spt->lib->numengs; i++)
        {
            FLAME_Up(spt->x + spt->lib->engx[i], spt->y + spt->lib->engy[i], spt->lib->englx[i], spt->eframe);
        }
        
        spt->eframe ^= 1;
    }
}

/***************************************************************************
ENEMY_GetBaseDamage() - Gets Base Ship damage
 ***************************************************************************/
int 
ENEMY_GetBaseDamage(
    void
)
{
    static int nums;
    int total, damage;
    enemy_t *spt;

    total = 0;
    nums = 0;
    
    for (spt = first_enemy.next; &last_enemy != spt; spt = spt->next)
    {
        if (!spt->lib->bossflag)
            continue;
        if (spt->y + spt->hly >= 0)
        {
            damage = (spt->hits * 100) / spt->lib->hits;
            total += damage;
            nums++;
        }
    }
    
    if (nums)
        total /= nums;
    
    return total;
}
