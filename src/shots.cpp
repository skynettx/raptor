#include <string.h>
#include "common.h"
#include "shots.h"
#include "glbapi.h"
#include "fx.h"
#include "rap.h"
#include "enemy.h"
#include "anims.h"
#include "tile.h"
#include "eshot.h"
#include "objects.h"
#include "fileids.h"

shot_t shots[MAX_SHOTS];

shot_t first_shots, last_shots;

shot_t *free_shots;

int shotnum;
int shothigh;

texture_t *detpow[4];
texture_t *laspow[4];
texture_t *lashit[4];

shot_lib_t shot_lib[LAST_WEAPON + 1];

/***************************************************************************
SHOTS_Clear () * Clears out SHOTS Linklist
 ***************************************************************************/
void 
SHOTS_Clear(
    void
)
{
    int loop;
    
    shotnum = 0;
    
    first_shots.prev = NULL;
    first_shots.next = &last_shots;
    
    last_shots.prev = &first_shots;
    last_shots.next = NULL;
    
    free_shots = shots;
    
    memset(shots, 0, sizeof(shots));

    for (loop = 0; loop < MAX_SHOTS; loop++)
    {
        if (loop == 69) // FIXME
        {
            shots[loop].next = &first_shots;
            continue;
        }
        
        shots[loop].next = &shots[loop + 1];
    }
}

/*-------------------------------------------------------------------------*
SHOTS_Get () - gets a Free SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
shot_t 
*SHOTS_Get(
    void
)
{
    shot_t *news;
    
    if (!free_shots)
        return NULL;
    
    shotnum++;
    if (shotnum > shothigh)
        shothigh = shotnum;
    
    news = free_shots;
    free_shots = free_shots->next;
    
    memset(news, 0, sizeof(shot_t));
    
    news->next = &last_shots;
    news->prev = last_shots.prev;
    last_shots.prev = news;
    news->prev->next = news;
    
    return news;
}

/*-------------------------------------------------------------------------*
SHOTS_Remove () - Removes SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
shot_t 
*SHOTS_Remove(
    shot_t *sh
)
{
    shot_t *next;
    
    shotnum--;
    
    next = sh->prev;
    
    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;
    
    memset(sh, 0, sizeof(shot_t));
    
    sh->next = free_shots;
    
    free_shots = sh;
    
    return next;
}

/***************************************************************************
SHOTS_Init () - Inits SHOTS system and clears link list
 ***************************************************************************/
void 
SHOTS_Init(
    void
)
{
    int i, item;
    shot_lib_t *slib;
    
    SHOTS_Clear();
    
    for (i = 0; i < 4; i++)
    {
        detpow[i] = (texture_t*)GLB_LockItem(FILE139_DETHPOW_BLK + i);
    }
    
    for (i = 0; i < 4; i++)
    {
        laspow[i] = (texture_t*)GLB_LockItem(FILE13d_LASERPOW_BLK + i);
    }
    
    for (i = 0; i < 4; i++)
    {
        lashit[i] = (texture_t*)GLB_LockItem(FILE1f1_DRAYHIT_BLK + i);
    }
    
    memset(shot_lib, 0, sizeof(shot_lib));
    
    // == FORWARD_GUNS =====================================
    slib = &shot_lib[S_FORWARD_GUNS];
    slib->lumpnum = FILE1c3_NMSHOT_BLK;
    slib->shadow = 0;
    slib->type = S_FORWARD_GUNS;
    slib->hits = 1;
    slib->speed = 8;
    slib->maxspeed = 16;
    slib->startframe = 0;
    slib->numframes = 4;
    slib->delayflag = 0;
    slib->shoot_rate = 2;
    slib->cur_shoot = 0;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_ALL;

    // == PLASMA_GUNS =====================================
    slib = &shot_lib[S_PLASMA_GUNS];
    slib->lumpnum = FILE1cf_PLASMA_BLK;
    slib->shadow = 0;
    slib->type = S_PLASMA_GUNS;
    slib->hits = 2;
    slib->speed = 4;
    slib->maxspeed = 8;
    slib->startframe = 0;
    slib->numframes = 2;
    slib->delayflag = 0;
    slib->shoot_rate = 10;
    slib->cur_shoot = 0;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_AIR;

    // == MICRO_MISSLE =====================================
    slib = &shot_lib[S_MICRO_MISSLE];
    slib->lumpnum = FILE1c1_MICROM_BLK;
    slib->shadow = 0;
    slib->type = S_MICRO_MISSLE;
    slib->hits = 2;
    slib->speed = 2;
    slib->maxspeed = 8;
    slib->startframe = 0;
    slib->numframes = 2;
    slib->delayflag = 0;
    slib->shoot_rate = 4;
    slib->cur_shoot = 0;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_GRALL;

    // == DUMB_MISSLE =====================================
    slib = &shot_lib[S_DUMB_MISSLE];
    slib->lumpnum = FILE1ba_MISDUM_BLK;
    slib->shadow = 1;
    slib->type = S_DUMB_MISSLE;
    slib->hits = 4;
    slib->speed = 2;
    slib->maxspeed = 12;
    slib->startframe = 1;
    slib->numframes = 3;
    slib->delayflag = 1;
    slib->shoot_rate = 10;
    slib->cur_shoot = 0;
    slib->use_plot = 1;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_ALL;

    // == MINI_GUN =====================================
    slib = &shot_lib[S_MINI_GUN];
    slib->lumpnum = FILE1c3_NMSHOT_BLK;
    slib->shadow = 1;
    slib->type = S_MINI_GUN;
    slib->hits = 1;
    slib->speed = 8;
    slib->maxspeed = 10;
    slib->startframe = 1;
    slib->numframes = 4;
    slib->delayflag = 0;
    slib->shoot_rate = 1;
    slib->cur_shoot = 0;
    slib->use_plot = 1;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_GRALL;

    // == LASER TURRET =====================================
    slib = &shot_lib[S_TURRET];
    slib->lumpnum = -1;
    slib->shadow = 0;
    slib->type = S_TURRET;
    slib->hits = 5;
    slib->speed = 0;
    slib->maxspeed = 0;
    slib->startframe = 0;
    slib->numframes = 0;
    slib->delayflag = 0;
    slib->shoot_rate = 6;
    slib->cur_shoot = 0;
    slib->use_plot = 0;
    slib->move_flag = 0;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_LINE;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    // slib->f_78 = slib->f_74->f_c >> 1;
    // slib->f_7c = slib->f_74->f_10 >> 1;
    slib->ht = S_ALL;

    // == MISSLE_PODS =====================================
    slib = &shot_lib[S_MISSLE_PODS];
    slib->lumpnum = FILE1bd_MISRAT_BLK;
    slib->shadow = 0;
    slib->type = S_MISSLE_PODS;
    slib->hits = 4;
    slib->speed = 1;
    slib->maxspeed = 16;
    slib->startframe = 0;
    slib->numframes = 2;
    slib->delayflag = 0;
    slib->shoot_rate = 5;
    slib->cur_shoot = 0;
    slib->smoke = 1;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_AIR;

    // == AIR TO AIR =====================================
    slib = &shot_lib[S_AIR_MISSLE];
    slib->lumpnum = FILE1bd_MISRAT_BLK;
    slib->shadow = 0;
    slib->type = S_MISSLE_PODS;
    slib->hits = 4;
    slib->speed = 1;
    slib->maxspeed = 12;
    slib->startframe = 0;
    slib->numframes = 2;
    slib->delayflag = 0;
    slib->shoot_rate = 10;
    slib->cur_shoot = 0;
    slib->smoke = 1;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_AIR;

    // == AIR TO GROUND =====================================
    slib = &shot_lib[S_GRD_MISSLE];
    slib->lumpnum = FILE1bf_MISGRD_BLK;
    slib->shadow = 0;
    slib->type = S_GRD_MISSLE;
    slib->hits = 20;
    slib->speed = 1;
    slib->maxspeed = 6;
    slib->startframe = 0;
    slib->numframes = 2;
    slib->delayflag = 0;
    slib->shoot_rate = 20;
    slib->cur_shoot = 0;
    slib->smoke = 1;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_GROUND;

    // == GROUND BOMB =====================================
    slib = &shot_lib[S_BOMB];
    slib->lumpnum = FILE1d3_BLDGBOMB_PIC;
    slib->shadow = 0;
    slib->type = S_BOMB;
    slib->hits = 50;
    slib->speed = 1;
    slib->maxspeed = 4;
    slib->startframe = 0;
    slib->numframes = 1;
    slib->delayflag = 0;
    slib->shoot_rate = 30;
    slib->cur_shoot = 0;
    slib->smoke = 0;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_GTILE;

    // == ENERGY GRAB =====================================
    slib = &shot_lib[S_ENERGY_GRAB];
    slib->lumpnum = FILE11d_POWDIS_BLK;
    slib->shadow = 0;
    slib->type = S_ENERGY_GRAB;
    slib->hits = 3;
    slib->speed = 4;
    slib->maxspeed = 8;
    slib->startframe = 0;
    slib->numframes = 6;
    slib->delayflag = 0;
    slib->shoot_rate = 2;
    slib->cur_shoot = 0;
    slib->smoke = 0;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_SUCK;

    // == MEGA BOMB =====================================
    slib = &shot_lib[S_MEGA_BOMB];
    slib->lumpnum = FILE1cb_MEGABM_BLK;
    slib->shadow = 1;
    slib->type = S_MEGA_BOMB;
    slib->hits = 50;
    slib->speed = 2;
    slib->maxspeed = 2;
    slib->startframe = 0;
    slib->numframes = 4;
    slib->delayflag = 0;
    slib->shoot_rate = 60;
    slib->cur_shoot = 0;
    slib->smoke = 0;
    slib->use_plot = 1;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 1;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_ALL;

    // == PULSE CANNON =====================================
    slib = &shot_lib[S_PULSE_CANNON];
    slib->lumpnum = FILE123_SHOKWV_BLK;
    slib->shadow = 0;
    slib->type = S_PULSE_CANNON;
    slib->hits = 5;
    slib->speed = 8;
    slib->maxspeed = 8;
    slib->startframe = 0;
    slib->numframes = 2;
    slib->delayflag = 0;
    slib->shoot_rate = 3;
    slib->cur_shoot = 0;
    slib->smoke = 0;
    slib->use_plot = 0;
    slib->move_flag = 1;
    slib->fplrx = 0;
    slib->fplry = 0;
    slib->meffect = 0;
    slib->beam = S_SHOOT;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_ALL;

    // == FORWARD LASER =====================================
    slib = &shot_lib[S_FORWARD_LASER];
    slib->lumpnum = FILE135_FRNTLAS_BLK;
    slib->shadow = 0;
    slib->type = S_FORWARD_LASER;
    slib->hits = 10;
    slib->speed = 0;
    slib->maxspeed = 0;
    slib->startframe = 0;
    slib->numframes = 4;
    slib->delayflag = 0;
    slib->shoot_rate = 7;
    slib->cur_shoot = 0;
    slib->smoke = 0;
    slib->use_plot = 0;
    slib->move_flag = 0;
    slib->fplrx = 1;
    slib->fplry = 1;
    slib->meffect = 1;
    slib->beam = S_BEAM;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_AIR;

    // == DEATH RAY =====================================
    slib = &shot_lib[S_DEATH_RAY];
    slib->lumpnum = FILE131_DETHRY_BLK;
    slib->shadow = 0;
    slib->type = S_DEATH_RAY;
    slib->hits = 6;
    slib->speed = 0;
    slib->maxspeed = 0;
    slib->startframe = 0;
    slib->numframes = 4;
    slib->delayflag = 0;
    slib->shoot_rate = 7;
    slib->cur_shoot = 0;
    slib->smoke = 0;
    slib->use_plot = 0;
    slib->move_flag = 0;
    slib->fplrx = 1;
    slib->fplry = 1;
    slib->meffect = 1;
    slib->beam = S_BEAM;
    for (i = 0; i < slib->numframes; i++)
    {
        item = slib->lumpnum + i;
        slib->pic[i] = (texture_t*)GLB_LockItem(item);
    }
    slib->h = slib->pic[0];
    slib->hlx = slib->h->width >> 1;
    slib->hly = slib->h->height >> 1;
    slib->ht = S_GRALL;
}

/***************************************************************************
SHOTS_PlayerShoot() - Shoots the specified weapon
 ***************************************************************************/
int 
SHOTS_PlayerShoot(
    int type               // INPUT : OBJECT TYPE
)
{
    shot_lib_t *lib;
    shot_t *cur;
    enemy_t *enemy;

    lib = &shot_lib[type];
    
    if (type == -1)
        EXIT_Error("SHOTS_PlayerShoot() type = EMPTY  ");
    
    if (lib->cur_shoot)
        return 0;
    
    lib->cur_shoot = lib->shoot_rate;
    
    cur = SHOTS_Get();
    
    if (!cur)
        return 0;
    
    switch (type)
    {
    default:
        EXIT_Error("SHOTS_PlayerShoot() - Invalid Shot type");
        break;
    
    case S_FORWARD_GUNS:
        if (!fx_gus)
            SND_Patch(FX_GUN, 127);
        g_flash = 7;
        cur->curframe = (wrand() % lib->numframes);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx + o_gun1[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        ANIMS_StartAnim(A_PLAYER_SHOOT, o_gun1[playerpic], 0);
        
        cur = SHOTS_Get();
        if (!cur)
            return 0;
        
        cur->curframe = (wrand() % lib->numframes);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx - o_gun1[playerpic] - 1;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        ANIMS_StartAnim(A_PLAYER_SHOOT, -o_gun1[playerpic] - 1, 0);
        break;
    
    case S_PLASMA_GUNS:
        if (!fx_gus)
            SND_Patch(FX_GUN, 127);
        cur->lib = &shot_lib[type];
        cur->curframe = wrand() % lib->numframes;
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_MICRO_MISSLE:
        if (!fx_gus)
            SND_Patch(FX_GUN, 127);
        g_flash = 7;
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx + o_gun3[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        
        cur = SHOTS_Get();
        if (!cur)
            return 0;
        
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx - o_gun3[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_DUMB_MISSLE:
        if (!fx_gus)
            SND_Patch(FX_MISSLE, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x + (wrand() % 16) + 10;
        cur->move.y2 = cur->y + 5;
        cur->startx = player_cx;
        cur->starty = player_cy;
        InitMobj(&cur->move);
        
        cur = SHOTS_Get();
        if (!cur)
            return 0;
        
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x - (wrand() % 16) - 10;
        cur->move.y2 = cur->y + 5;
        cur->startx = player_cx;
        cur->starty = player_cy;
        InitMobj(&cur->move);
        break;
    
    case S_MINI_GUN:
        enemy = ENEMY_GetRandom();
        if (!enemy)
            SHOTS_Remove(cur);
        else
        {
            if (!fx_gus)
                SND_Patch(FX_GUN, 127);
            cur->curframe = wrand() % lib->numframes;
            cur->lib = &shot_lib[type];
            cur->delayflag = lib->delayflag;
            cur->speed = lib->speed;
            cur->x = player_cx;
            cur->y = player_cy;
            cur->move.x = cur->x;
            cur->move.y = cur->y;
            cur->move.x2 = (wrand() % enemy->width) + enemy->x - 1;
            cur->move.y2 = (wrand() % enemy->height) + enemy->hly + enemy->y - 1;
            cur->startx = player_cx;
            cur->starty = player_cy;
            InitMobj(&cur->move);
        }
        break;
    
    case S_TURRET:
        enemy = ENEMY_GetRandomAir();
        if (!enemy)
        {
            SHOTS_Remove(cur);
            SND_Patch(FX_NOSHOOT, 127);
        }
        else
        {
            SND_Patch(FX_TURRET, 127);
            cur->lib = &shot_lib[type];
            enemy->hits -= lib->hits;
            cur->curframe = 0;
            cur->delayflag = lib->delayflag;
            cur->speed = lib->speed;
            cur->x = player_cx;
            cur->y = player_cy;
            cur->move.x = (wrand() % enemy->width) + enemy->move.x - 1;
            cur->move.y = (wrand() % enemy->height) + enemy->move.y - 1;
            cur->move.x2 = player_cx;
            cur->move.y2 = player_cy;
            cur->startx = player_cx;
            cur->starty = player_cy;
            InitMobj(&cur->move);
            ANIMS_StartAnim(A_LASER_BLAST, cur->move.x, cur->move.y);
        }
        break;
    
    case S_MISSLE_PODS:
        SND_Patch(FX_GUN, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx + o_gun2[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        ANIMS_StartAnim(A_PLAYER_SHOOT, o_gun2[playerpic], 1);
        
        cur = SHOTS_Get();
        if (!cur)
            return 0;
        
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx - o_gun2[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        ANIMS_StartAnim(A_PLAYER_SHOOT, -o_gun2[playerpic] - 1, 1);
        break;
    
    case S_AIR_MISSLE:
        SND_Patch(FX_MISSLE, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx + o_gun2[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        
        cur = SHOTS_Get();
        if (!cur)
            return 0;
        
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx - o_gun2[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_GRD_MISSLE:
        SND_Patch(FX_MISSLE, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx + o_gun2[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        
        cur = SHOTS_Get();
        if (!cur)
            return 0;
        
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx - o_gun2[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_BOMB:
        SND_Patch(FX_MISSLE, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_ENERGY_GRAB:
        SND_Patch(FX_GUN, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx - 4;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_MEGA_BOMB:
        SND_Patch(FX_GUN, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = 160;
        cur->move.y2 = 75;
        cur->startx = player_cx;
        cur->starty = player_cy;
        InitMobj(&cur->move);
        break;
    
    case S_PULSE_CANNON:
        SND_Patch(FX_PULSE, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx;
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = 0;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_FORWARD_LASER:
        SND_Patch(FX_LASER, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx + o_gun3[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = -24;
        cur->startx = player_cx;
        cur->starty = player_cy;
        
        cur = SHOTS_Get();
        if (!cur)
            return 0;
        
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx - o_gun3[playerpic];
        cur->y = player_cy;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->move.x2 = cur->x;
        cur->move.y2 = -24;
        cur->startx = player_cx;
        cur->starty = player_cy;
        break;
    
    case S_DEATH_RAY:
        SND_Patch(FX_LASER, 127);
        cur->lib = &shot_lib[type];
        cur->delayflag = lib->delayflag;
        cur->speed = lib->speed;
        cur->x = player_cx;
        cur->y = player_cy - 24;
        cur->move.x = cur->x;
        cur->move.y = cur->y;
        cur->startx = player_cx;
        cur->starty = player_cy;
        cur->move.x2 = cur->x;
        cur->move.y2 = -24;
        break;
    }
    
    return 1;
}

/***************************************************************************
SHOTS_Think () - Does All Thinking for shot system
 ***************************************************************************/
void 
SHOTS_Think(
    void
)
{
    shot_lib_t *lib;
    shot_t *shot;
    enemy_t *enemy;
    int i;

    lib = shot_lib;
    for (i = 0; i <= LAST_WEAPON; i++, lib++)
    {
        if (lib->cur_shoot > 0)
            lib->cur_shoot--;
    }

    for (shot = first_shots.next; &last_shots != shot; shot = shot->next)
    {
        lib = shot->lib;
        
        switch (lib->beam)
        {
        default:
            EXIT_Error("SHOTS_Think()");
            break;
        
        case S_SHOOT:
            shot->pic = lib->pic[shot->curframe];
            shot->x = shot->move.x - lib->hlx;
            if (lib->move_flag)
                shot->y = shot->move.y - lib->hly;
            else
                shot->y = shot->move.y;
            if (lib->smoke)
                ANIMS_StartAnim(A_SMALL_SMOKE_DOWN, shot->x + lib->hlx, shot->y + lib->hly * 2);
            break;
        
        case S_LINE:
            shot->x = shot->move.x;
            shot->y = shot->move.y;
            break;
        
        case S_BEAM:
            shot->pic = lib->pic[shot->curframe];
            shot->x = shot->move.x - lib->hlx;
            shot->y = shot->move.y;
            
            for (enemy = first_enemy.next; &last_enemy != enemy; enemy = enemy->next)
            {
                if (shot->x > enemy->x && shot->x < enemy->x2 && enemy->y < player_cy && enemy->y > -30)
                {
                    enemy->hits -= lib->hits;
                    
                    if (enemy->hits != -1)
                    {
                        shot->move.y2 = enemy->y + enemy->hly;
                        break;
                    }
                }
            }
            break;
        }
        
        if (lib->fplrx)
            shot->x += player_cx - shot->startx;
        
        if (lib->fplry)
            shot->y += player_cy - shot->starty;
        
        if ((shot->y + 16 < 0) || (shot->x < 0) || (shot->x > 320) || (shot->y > 200))
        {
            if (lib->move_flag)
            {
                shot->move.done = 1;
                goto shot_done;
            }
        }
        
        if (shot->delayflag == 0)
        {
            if (shot->speed < lib->maxspeed)
                shot->speed++;
            
            shot->curframe++;
            
            if (shot->curframe >= lib->numframes)
            {
                if (lib->move_flag)
                    shot->curframe = lib->startframe;
                else
                {
                    shot->move.done = 1;
                    goto shot_done;
                }
            }
        }
        
        if (shot->doneflag)
        {
            shot->move.done = 1;
            goto shot_done;
        }
        
        if (lib->meffect)
            goto shot_done;
        
        switch (lib->ht)
        {
        default:
            enemy = ENEMY_DamageEnergy(shot->x, shot->y, lib->hits);
            if (enemy)
            {
                shot->doneflag = 1;
                
                ANIMS_StartAnim(A_BLUE_SPARK, shot->x, shot->y);
                ANIMS_StartEAnim(enemy, A_ENERGY_GRAB, enemy->hlx, enemy->hly);
            }
            break;
        
        case S_GRALL:
            if (ENEMY_DamageAll(shot->x, shot->y, lib->hits))
            {
                shot->doneflag = 1;
                
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(A_BLUE_SPARK, shot->x, shot->y);
                else
                    ANIMS_StartAnim(A_ORANGE_SPARK, shot->x, shot->y);
            }
            break;
        
        case S_ALL:
            if (ENEMY_DamageAll(shot->x, shot->y, lib->hits))
            {
                shot->doneflag = 1;
                
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(A_BLUE_SPARK, shot->x, shot->y);
                else
                    ANIMS_StartAnim(A_ORANGE_SPARK, shot->x, shot->y);
            }
            else
            {
                if (TILE_IsHit(lib->hits, shot->x, shot->y))
                {
                    shot->move.done = 1;
                }
            }
            break;
        
        case S_AIR:
            if (ENEMY_DamageAir(shot->x, shot->y, lib->hits))
            {
                shot->doneflag = 1;
                
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(A_BLUE_SPARK, shot->x, shot->y);
                else
                    ANIMS_StartAnim(A_ORANGE_SPARK, shot->x, shot->y);
            }
            break;
        
        case S_GROUND:
            if (ENEMY_DamageGround(shot->x, shot->y, lib->hits))
            {
                shot->doneflag = 1;
                
                ANIMS_StartAnim(A_ORANGE_SPARK, shot->x, shot->y);
            }
            else
            {
                if (TILE_IsHit(lib->hits, shot->x, shot->y))
                {
                    shot->move.done = 1;
                }
            }
            break;
        
        case S_GTILE:
            if (TILE_Bomb(lib->hits, shot->x, shot->y))
            {
                shot->move.done = 1;
            }
            if (ENEMY_DamageGround(shot->x, shot->y, 5))
            {
                shot->doneflag = 1;
                
                ANIMS_StartAnim(A_SMALL_GROUND_EXPLO, shot->x, shot->y);
            }
            break;
        }
    
    shot_done:
        
        if (shot->move.done)
        {
            if (shot->delayflag)
            {
                shot->delayflag = 0;
                shot->move.x2 = shot->move.x + ((wrand() % 32) - 16);
                shot->move.y2 = 0;
                ANIMS_StartAnim(A_SMALL_SMOKE_DOWN, shot->move.x, shot->move.y);
                InitMobj(&shot->move);
            }
            else
            {
                switch (lib->type)
                {
                case S_MEGA_BOMB:
                    ESHOT_Clear();
                    TILE_DamageAll();
                    for (enemy = first_enemy.next; &last_enemy != enemy; enemy = enemy->next)
                    {
                        enemy->hits -= lib->hits;
                    }
                    startfadeflag = 1;
                    ANIMS_StartAnim(A_SUPER_SHIELD, 0, 0);
                    shot = SHOTS_Remove(shot);
                    continue;
                
                case S_TURRET:
                    break;
                
                default:
                    shot = SHOTS_Remove(shot);
                    continue;
                }
            }
        }
        
        if (lib->move_flag)
        {
            if (lib->use_plot)
            {
                MoveSobj(&shot->move, shot->speed);
            }
            else
            {
                shot->move.y -= shot->speed;
                if (shot->move.y < 0)
                {
                    shot->move.done = 1;
                    shot->doneflag = 1;
                }
            }
        }
    }
}

/***************************************************************************
SHOTS_Display () - Displays All active Shots
 ***************************************************************************/
void 
SHOTS_Display(
    void
)
{
    int loop, x, y;
    shot_t *shot;
    texture_t *h;
    
    for (shot = first_shots.next; shot != &last_shots; shot = shot->next)
    {
        switch (shot->lib->beam)
        {
        default:
            EXIT_Error("SHOTS_Display()");
            break;
        
        case S_SHOOT:
            GFX_PutSprite(shot->pic, shot->x, shot->y);
            break;
        
        case S_LINE:
            GFX_Line(player_cx + 1, player_cy, shot->move.x, shot->move.y, 69);
            GFX_Line(player_cx - 1, player_cy, shot->move.x, shot->move.y, 69);
            GFX_Line(player_cx, player_cy, shot->move.x, shot->move.y, 64);
            shot = SHOTS_Remove(shot);
            break;
        
        case S_BEAM:
            for (loop = shot->move.y2; loop < shot->y; loop += 3)
            {
                GFX_PutSprite(shot->pic, shot->x, loop);
            }
            
            if (shot->lib->type == S_DEATH_RAY)
                GFX_PutSprite(detpow[shot->cnt], shot->x - 4, shot->y);
            else
                GFX_PutSprite(laspow[shot->cnt], shot->x, shot->y);
            
            h = lashit[shot->cnt];
            
            x = shot->x - (h->width >> 2);
            y = shot->move.y2 - 8;
            
            if (y > 0)
                GFX_PutSprite(h, x, y);
            
            shot->cnt++;
            shot->cnt = shot->cnt % 4;
            break;
        }
    }
}
