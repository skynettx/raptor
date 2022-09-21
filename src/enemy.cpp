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

enemy_t *ENEMY_GetRandom(void)
{
    int v1c;
    if (!cur_visable)
        return NULL;
    v1c = wrand() % cur_visable;
    return onscreen[v1c];
}

enemy_t *ENEMY_GetRandomAir(void)
{
    int v1c;
    int v20;
    if (!cur_visable)
        return NULL;
    v1c = 0;
    for (v20 = 0; v20 < cur_visable; v20++)
    {
        if (onscreen[v20]->groundflag)
            continue;
        rscreen[v1c] = onscreen[v20];
        v1c++;
    }
    if (v1c > 0)
    {
        v1c = wrand() % v1c;
        return rscreen[v1c];
    }
    return NULL;
}

int ENEMY_DamageAll(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (a1 > v18->x&& a1 < v18->x2 && a2 > v18->y&& a2 < v18->y2)
        {
            v18->hits -= a3;
            return 1;
        }
    }
    return 0;
}

int ENEMY_DamageGround(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (!v18->groundflag)
            continue;
        if (a1 > v18->x && a1 < v18->x2 && a2 > v18->y&& a2 < v18->y2)
        {
            v18->hits -= a3;
            if (curplr_diff == 0)
                v18->hits -= a3;
            return 1;
        }
    }
    return 0;
}

int ENEMY_DamageAir(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (v18->groundflag)
            continue;
        if (a1 > v18->x && a1 < v18->x2 && a2 > v18->y&& a2 < v18->y2)
        {
            v18->hits -= a3;
            if (curplr_diff == 0)
                v18->hits -= a3;
            return 1;
        }
    }
    return 0;
}

enemy_t *ENEMY_DamageEnergy(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (v18->groundflag)
            continue;
        if (a1 > v18->x&& a1 < v18->x2 && a2 > v18->y&& a2 < v18->y2)
        {
            v18->hits--;
            if (v18->lib->f_24)
            {
                if (v18->suckagain > 0)
                {
                    v18->suckagain -= a3;
                }
                else
                {
                    v18->shoot_on = 0;
                    v18->shoot_disable = 1;
                    v18->shootagain = -1;
                    SND_3DPatch(14, v18->x + v18->hlx, v18->y + v18->hlx);
                }
            }
            return v18;
        }
    }
    return NULL;
}

void ENEMY_Think(void)
{
    csprite_t *v24;
    enemy_t *v1c;
    slib_t *v20;
    int v28, v2c, v30, v34, v3c, v38;
    if (boss_sound)
    {
        if (!SND_IsPatchPlaying(23))
            SND_Patch(23, 127);
    }
    tiley = tilepos / 9 - 3;
    while (!end_waveflag && cur_enemy->y == tiley)
    {
        do
        {
            v24 = cur_enemy;
            if (cur_enemy->level != 64)
                ENEMY_Add(cur_enemy);
            if (cur_enemy == end_enemy)
            {
                end_waveflag = 1;
                break;
            }
            cur_enemy++;
        } while (v24->f_0 != -1 && v24->f_0 != 1);
    }
    cur_visable = 0;
    for (v1c = first_enemy.next; &last_enemy != v1c; v1c = v1c->next)
    {
        v20 = v1c->lib;
        if (v20->num_frames > 1)
        {
            v1c->item = v20->item + v1c->f_8c;
            if (v1c->frame_rate < 1)
            {
                v1c->frame_rate = v20->frame_rate;
                if (v1c->anim_on)
                {
                    v1c->f_8c++;
                    if ((unsigned int)v1c->f_8c >= (unsigned int)v1c->num_frames)
                    {
                        v1c->f_8c -= v20->rewind;
                        switch (v20->animtype)
                        {
                        default:
                            EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE1");
                        case 0:
                            break;
                        case 1:
                            v1c->anim_on = 0;
                            v1c->shoot_on = 1;
                            break;
                        case 2:
                            switch (v1c->f_b0)
                            {
                            case 1:
                                v1c->num_frames = v20->num_frames;
                                v1c->f_b0 = 2;
                                break;
                            case 2:
                                v1c->shoot_on = 1;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
                v1c->frame_rate--;
            if (v1c->countdown < 1)
            {
                switch (v20->animtype)
                {
                default:
                    EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE2");
                case 0:
                    v1c->shoot_on = 1;
                    break;
                case 1:
                    v1c->anim_on = 1;
                    break;
                case 2:
                    if (!v1c->f_b0)
                        v1c->f_b0 = 1;
                    break;
                }
            }
            else
                v1c->countdown -= v20->movespeed;
        }
        else
        {
            if (v1c->countdown < 1)
            {
                v1c->countdown = -1;
                v1c->shoot_on = 1;
            }
            else
                v1c->countdown -= v20->movespeed;
        }
        switch (v20->flighttype)
        {
        case 0:
            v1c->x = v1c->mobj.x;
            v1c->y = v1c->mobj.y;
            v1c->x2 = v1c->x + v1c->width - 1;
            v1c->y2 = v1c->y + v1c->height - 1;
            v28 = v20->movespeed;
            v28 = MoveEobj(&v1c->mobj, v28);
            if (v1c->mobj.done)
            {
                v1c->mobj.x = v1c->mobj.x2;
                v1c->mobj.y = v1c->mobj.y2;
                v1c->mobj.x2 = v1c->sx + v20->flightx[v1c->movepos];
                v1c->mobj.y2 = v1c->sy + v20->flighty[v1c->movepos];
                InitMobj(&v1c->mobj);
                MoveMobj(&v1c->mobj);
                v28 = MoveEobj(&v1c->mobj, v28);
                if (!v1c->edir)
                {
                    v1c->movepos++;
                    if (v1c->movepos >= v20->f_5c)
                    {
                        v1c->edir = 1;
                        v1c->movepos = v20->f_5c - 1;
                    }
                }
                else
                {
                    v1c->movepos--;
                    if (v1c->movepos <= v20->f_60)
                    {
                        v1c->edir = 0;
                        v1c->movepos = v20->f_60;
                    }
                }
            }
            break;
        case 2:
            v1c->x = v1c->mobj.x;
            v1c->y = v1c->mobj.y;
            v1c->x2 = v1c->x + v1c->width - 1;
            v1c->y2 = v1c->y + v1c->height - 1;
            v28 = v20->movespeed;
            v28 = MoveEobj(&v1c->mobj, v28);
            if (v1c->kami == 2)
            {
                if (v1c->mobj.y > 0xc9)
                    v1c->f_58 = 1;
                if (v1c->hlx + 320 < v1c->mobj.x)
                    v1c->f_58 = 1;
                if (v1c->mobj.y + v1c->width < 0)
                    v1c->f_58 = 1;
                if (v1c->mobj.x + v1c->width < 0)
                    v1c->f_58 = 1;
            }
            if (v1c->mobj.done && v1c->kami != 2)
            {
                v1c->mobj.x = v1c->mobj.x2;
                v1c->mobj.y = v1c->mobj.y2;
                v1c->x2 = v1c->x + v1c->width - 1;
                v1c->y2 = v1c->y + v1c->height - 1;
                if (v1c->kami == 1)
                {
                    v1c->mobj.x2 = player_cx;
                    v1c->mobj.y2 = player_cy;
                    v1c->kami = 2;
                }
                else
                {
                    v1c->mobj.x2 = v1c->sx + v20->flightx[v1c->movepos];
                    v1c->mobj.y2 = v1c->sy + v20->flighty[v1c->movepos];
                }
                InitMobj(&v1c->mobj);
                MoveMobj(&v1c->mobj);
                v28 = MoveEobj(&v1c->mobj, v28);
                if (v20->f_5c - 1 > v1c->movepos)
                    v1c->movepos++;
                else if (v1c->kami == 0)
                    v1c->kami = 1;
            }
            break;
        case 1:
            v1c->x = v1c->mobj.x;
            v1c->y = v1c->mobj.y;
            v1c->x2 = v1c->x + v1c->width - 1;
            v1c->y2 = v1c->y + v1c->height - 1;
            v28 = v20->movespeed;
            v28 = MoveEobj(&v1c->mobj, v28);
            if (v1c->mobj.done)
            {
                v1c->mobj.x = v1c->mobj.x2;
                v1c->mobj.y = v1c->mobj.y2;
                v1c->mobj.x2 = v1c->sx + v20->flightx[v1c->movepos];
                v1c->mobj.y2 = v1c->sy + v20->flighty[v1c->movepos];
                InitMobj(&v1c->mobj);
                MoveMobj(&v1c->mobj);
                v28 = MoveEobj(&v1c->mobj, v28);
                v1c->movepos++;
                if (v1c->movepos > v20->f_5c)
                    v1c->f_58 = 1;
            }
            break;
        case 3:
            if (scroll_flag)
                v1c->y++;
            if (v1c->y > v1c->mobj.y2)
                v1c->f_58 = 1;
            v1c->x2 = v1c->x + v1c->width - 1;
            v1c->y2 = v1c->y + v1c->height - 1;
            break;
        case 5:
            if (scroll_flag)
                v1c->y++;
            if (v1c->y >= 0)
            {
                v1c->x += v20->movespeed;
                if (v1c->x > v1c->mobj.x2)
                    v1c->f_58 = 1;
                else if (v1c->y > v1c->mobj.y2)
                    v1c->f_58 = 1;
            }
            v1c->x2 = v1c->x + v1c->width - 1;
            v1c->y2 = v1c->y + v1c->height - 1;
            break;
        case 4:
            if (scroll_flag)
                v1c->y++;
            if (v1c->y >= 0)
            {
                v1c->x -= v20->movespeed;
                if (v1c->x < v1c->mobj.x2)
                    v1c->f_58 = 1;
                else if (v1c->y > v1c->mobj.y2)
                    v1c->f_58 = 1;
            }
            v1c->x2 = v1c->x + v1c->width - 1;
            v1c->y2 = v1c->y + v1c->height - 1;
            break;
        }
        if (v1c->shoot_on)
        {
            switch (v1c->shootagain)
            {
            case -1:
                v1c->shootflag--;
                if (v1c->shootflag < 0)
                {
                    v1c->shootflag = v20->f_1c;
                    if (v1c->shoot_disable == 0)
                    {
                        for (v2c = 0; v2c < v20->f_68; v2c++)
                        {
                            ESHOT_Shoot(v1c, v2c);
                        }
                    }
                    v1c->shootcount--;
                    if (v1c->shootcount < 1)
                        v1c->shootagain = v20->f_54;
                }
                break;
            case 0:
                v1c->shootagain = -1;
                v1c->shootcount = v20->shootcnt;
                v1c->shootflag = v20->f_1c;
                break;
            default:
                v1c->shootagain--;
                break;
            }
        }
        if (v1c->f_58)
        {
            v1c = ENEMY_Remove(v1c);
            continue;
        }
        if (v20->f_3c)
        {
            if (v1c->groundflag)
            {
                SHADOW_GAdd(v1c->item, v1c->x, v1c->y);
            }
            else
            {
                SHADOW_Add(v1c->item, v1c->x, v1c->y);
            }
        }
        if (v20->bossflag)
        {
            numboss++;
            if (v1c->hits < 50 && (gl_cnt & 2) != 0)
            {
                v30 = v1c->x + (wrand() % v1c->width);
                v34 = v1c->y + (wrand() % v1c->height);
                ANIMS_StartAnim(6, v30, v34);
            }
        }
        if (!v1c->groundflag)
        {
            if (player_cx > v1c->x && player_cx < v1c->x2)
            {
                if (player_cy > v1c->y && player_cy < v1c->y2)
                {
                    if ((haptic) && (control == 2))
                    {
                        IPT_CalJoyRumbleMedium();                                                            //Rumble when enemy hit
                    }
                    v1c->hits -= 16;
                    if (v1c->width > v1c->height)
                        v3c = v1c->width;
                    else
                        v3c = v1c->height;
                    OBJS_SubEnergy(v3c >> 2);
                    v30 = player_cx + (wrand() % 8) - 4;
                    v34 = player_cy + (wrand() % 8) - 4;
                    ANIMS_StartAnim(6, v30, v34);
                    SND_Patch(11, 127);
                }
            }
        }
        if (v1c->hits <= 0)
        {
            player.score += v20->f_48;
            SND_3DPatch(8, v1c->x + v1c->hlx, v1c->x + v1c->hlx);
            switch (v20->f_18)
            {
            case 8:
                ANIMS_StartAnim(8, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                BONUS_Add(23, v1c->x, v1c->y);
                break;
            case 0:
                ANIMS_StartAnim(5, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                break;
            case 10:
                ANIMS_StartAnim(7, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                break;
            case 1:
                ANIMS_StartAnim(4, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                break;
            case 2:
                ANIMS_StartAnim(4, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                v38 = (v1c->width>>4) * (v1c->height>>4);
                for (v2c = 0; v2c < v38; v2c++)
                {
                    v30 = v1c->x + (wrand() % v1c->width);
                    v34 = v1c->y + (wrand() % v1c->height);
                    if (v2c & 1)
                        ANIMS_StartAnim(5, v30, v34);
                    else
                        ANIMS_StartAAnim(7, v30, v34);
                }
                break;
            case 3:
                ANIMS_StartAnim(1, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                break;
            case 4:
                v30 = v1c->x + (wrand() % v1c->width);
                v34 = v1c->y + (wrand() % v1c->height);
                if ((wrand() % 2) == 0)
                    ANIMS_StartAnim(18, v30, v34);
                else
                    ANIMS_StartAnim(17, v30, v34);
                ANIMS_StartAnim(0, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                break;
            case 5:
                ANIMS_StartAnim(0, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                v38 = (v1c->width>>4) * (v1c->height>>4);
                for (v2c = 0; v2c < v38; v2c++)
                {
                    v30 = v1c->x + (wrand() % v1c->width);
                    v34 = v1c->y + (wrand() % v1c->height);
                    if ((wrand()%2) == 0)
                        ANIMS_StartAnim(17, v30, v34);
                    else
                        ANIMS_StartAnim(18, v30, v34);
                    ANIMS_StartAnim(1, v30, v34);
                }
                break;
            case 6:
                ANIMS_StartAnim(4, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                v38 = (v1c->width>>4) * (v1c->height>>4);
                for (v2c = 0; v2c < v38; v2c++)
                {
                    v30 = v1c->x + (wrand() % v1c->width);
                    v34 = v1c->y + (wrand() % v1c->height);
                    ANIMS_StartAAnim(17, v30, v34);
                    if (v2c & 1)
                        ANIMS_StartAnim(4, v30, v34);
                    else
                        ANIMS_StartAnim(7, v30, v34);
                }
                break;
            case 7:
                ANIMS_StartAnim(2, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                break;
            case 9:
                ANIMS_StartAnim(3, v1c->x + v1c->hlx, v1c->y + v1c->hly);
                break;
            }
            if (v20->f_14 != -1)
                BONUS_Add(v20->f_14, v1c->x, v1c->y);
            v1c = ENEMY_Remove(v1c);
            continue;
        }
        v34 = v1c->y + v1c->height;
        if (v34 > 0 && v1c->y < 200)
        {
            v30 = v1c->x + v1c->width;
            if (v30 > 0 && v1c->x < 320)
            {
                onscreen[cur_visable] = v1c;
                cur_visable++;
            }
        }
    }
}

void ENEMY_DisplayGround(void)
{
    enemy_t *v1c;
    for (v1c = first_enemy.next; &last_enemy != v1c; v1c = v1c->next)
    {
        if (!v1c->groundflag)
            continue;
        GFX_PutSprite((texture_t*)GLB_GetItem(v1c->item), v1c->x, v1c->y);
    }
}

void ENEMY_DisplaySky(void)
{
    int v20;
    enemy_t *v1c;
    for (v1c = first_enemy.next; &last_enemy != v1c; v1c = v1c->next)
    {
        if (v1c->groundflag)
            continue;
        GFX_PutSprite((texture_t*)GLB_GetItem(v1c->item), v1c->x, v1c->y);
        for (v20 = 0; v20 < v1c->lib->f_6c; v20++)
        {
            FLAME_Up(v1c->x + v1c->lib->f_a8[v20], v1c->y + v1c->lib->f_d8[v20], v1c->lib->f_108[v20], v1c->f_90);
        }
        v1c->f_90 ^= 1;
    }
}

int ENEMY_GetBaseDamage(void)
{
    static int nums;
    int v1c, v24;
    enemy_t *v20;

    v1c = 0;
    nums = 0;
    for (v20 = first_enemy.next; &last_enemy != v20; v20 = v20->next)
    {
        if (!v20->lib->bossflag)
            continue;
        if (v20->y + v20->hly >= 0)
        {
            v24 = (v20->hits * 100) / v20->lib->hits;
            v1c += v24;
            nums++;
        }
    }
    if (nums)
        v1c /= nums;
    return v1c;
}
