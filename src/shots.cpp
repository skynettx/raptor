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

int SHOTS_PlayerShoot(int a1)
{
    shot_lib_t *v20;
    shot_t *v1c;
    enemy_t *v24;

    v20 = &shot_lib[a1];
    if (a1 == -1)
        EXIT_Error("SHOTS_PlayerShoot() type = EMPTY  ");
    if (v20->cur_shoot)
        return 0;
    v20->cur_shoot = v20->shoot_rate;
    v1c = SHOTS_Get();
    if (!v1c)
        return 0;
    switch (a1)
    {
    default:
        EXIT_Error("SHOTS_PlayerShoot() - Invalid Shot type");
        break;
    case 0:
        if (!fx_gus)
            SND_Patch(16, 127);
        g_flash = 7;
        v1c->currentFrame = (wrand() % v20->numframes);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;

        v1c->x = player_cx + o_gun1[playerpic];
        v1c->y = player_cy;
        
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, o_gun1[playerpic], 0);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->currentFrame = (wrand() % v20->numframes);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;

        v1c->x = player_cx - o_gun1[playerpic] - 1;
        v1c->y = player_cy;

        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, -o_gun1[playerpic] - 1, 0);
        break;
    case 1:
        if (!fx_gus)
            SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->currentFrame = wrand() % v20->numframes;
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 2:
        if (!fx_gus)
            SND_Patch(16, 127);
        g_flash = 7;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx + o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx - o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 3:
        if (!fx_gus)
            SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x + (wrand() % 16) + 10;
        v1c->mobj.y2 = v1c->y + 5;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->mobj);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x - (wrand() % 16) - 10;
        v1c->mobj.y2 = v1c->y + 5;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->mobj);
        break;
    case 4:
        v24 = ENEMY_GetRandom();
        if (!v24)
            SHOTS_Remove(v1c);
        else
        {
            if (!fx_gus)
                SND_Patch(16, 127);
            v1c->currentFrame = wrand() % v20->numframes;
            v1c->shotLib = &shot_lib[a1];
            v1c->f_4c = v20->delayflag;
            v1c->speedY = v20->speed;
            v1c->x = player_cx;
            v1c->y = player_cy;
            v1c->mobj.x = v1c->x;
            v1c->mobj.y = v1c->y;
            v1c->mobj.x2 = (wrand() % v24->width) + v24->x - 1;
            v1c->mobj.y2 = (wrand() % v24->height) + v24->hly + v24->y - 1;
            v1c->f_50 = player_cx;
            v1c->f_54 = player_cy;
            InitMobj(&v1c->mobj);
        }
        break;
    case 5:
        v24 = ENEMY_GetRandomAir();
        if (!v24)
        {
            SHOTS_Remove(v1c);
            SND_Patch(34, 127);
        }
        else
        {
            SND_Patch(21, 127);
            v1c->shotLib = &shot_lib[a1];
            v24->hits -= v20->hits;
            v1c->currentFrame = 0;
            v1c->f_4c = v20->delayflag;
            v1c->speedY = v20->speed;
            v1c->x = player_cx;
            v1c->y = player_cy;
            v1c->mobj.x = (wrand() % v24->width) + v24->mobj.x - 1;
            v1c->mobj.y = (wrand() % v24->height) + v24->mobj.y - 1;
            v1c->mobj.x2 = player_cx;
            v1c->mobj.y2 = player_cy;
            v1c->f_50 = player_cx;
            v1c->f_54 = player_cy;
            InitMobj(&v1c->mobj);
            ANIMS_StartAnim(9, v1c->mobj.x, v1c->mobj.y);
        }
        break;
    case 6:
        SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx + o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, o_gun2[playerpic], 1);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx - o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, -o_gun2[playerpic] - 1, 1);
        break;
    case 7:
        SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx + o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx - o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 8:
        SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx + o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx - o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 9:
        SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 10:
        SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx - 4;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 11:
        SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = 160;
        v1c->mobj.y2 = 75;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->mobj);
        break;
    case 12:
        SND_Patch(35, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 13:
        SND_Patch(18, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx + o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = -24;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx - o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = -24;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 14:
        SND_Patch(18, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->delayflag;
        v1c->speedY = v20->speed;
        v1c->x = player_cx;
        v1c->y = player_cy - 24;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = -24;
        break;
    }
    return 1;
}


void SHOTS_Think(void)
{
    shot_lib_t *v1c;
    shot_t *v20;
    enemy_t *v24;
    int i;

    v1c = shot_lib;
    for (i = 0; i <= 14; i++, v1c++)
    {
        if (v1c->cur_shoot > 0)
            v1c->cur_shoot--;
    }

    for (v20 = first_shots.next; &last_shots != v20; v20 = v20->next)
    {
        v1c = v20->shotLib;
        switch (v1c->beam)
        {
        default:
            EXIT_Error("SHOTS_Think()");
            break;
        case 0:
            v20->TexturePtr = v1c->pic[v20->currentFrame];
            v20->x = v20->mobj.x - v1c->hlx;
            if (v1c->move_flag)
                v20->y = v20->mobj.y - v1c->hly;
            else
                v20->y = v20->mobj.y;
            if (v1c->smoke)
                ANIMS_StartAnim(11, v20->x + v1c->hlx, v20->y + v1c->hly * 2);
            break;
        case 1:
            v20->x = v20->mobj.x;
            v20->y = v20->mobj.y;
            break;
        case 2:
            v20->TexturePtr = v1c->pic[v20->currentFrame];
            v20->x = v20->mobj.x - v1c->hlx;
            v20->y = v20->mobj.y;
            for (v24 = first_enemy.next; &last_enemy != v24; v24 = v24->next)
            {
                if (v20->x > v24->x && v20->x < v24->x2 && v24->y < player_cy && v24->y > -30)
                {
                    v24->hits -= v1c->hits;
                    if (v24->hits != -1)
                    {
                        v20->mobj.y2 = v24->y + v24->hly;
                        break;
                    }
                }
            }
            break;
        }
        if (v1c->fplrx)
            v20->x += player_cx - v20->f_50;
        if (v1c->fplry)
            v20->y += player_cy - v20->f_54;
        //if (v20->f_10 + 16 < 0 || v20->f_c < 0 && v20->f_c > 320 && v20->f_10 > 200)
        if ((v20->y + 16 < 0) || (v20->x < 0) || (v20->x > 320) || (v20->y > 200))
        {
            if (v1c->move_flag)
            {
                v20->mobj.done = 1;
                goto LAB_00015d11;
            }
        }
        
        if (v20->f_4c == 0)
        {
            if (v20->speedY < v1c->maxspeed)
                v20->speedY++;
            v20->currentFrame++;
            if (v20->currentFrame >= v1c->numframes)
            {
                if (v1c->move_flag)
                    v20->currentFrame = v1c->startframe;
                else
                {
                    v20->mobj.done = 1;
                    goto LAB_00015d11;
                }
            }
        }
        if (v20->f_48)
        {
            v20->mobj.done = 1;
            goto LAB_00015d11;
        }
        if (v1c->meffect)
            goto LAB_00015d11;
        switch (v1c->ht)
        {
        default:
            v24 = ENEMY_DamageEnergy(v20->x, v20->y, v1c->hits);
            if (v24)
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(14, v20->x, v20->y);
                ANIMS_StartEAnim(v24, 19, v24->hlx, v24->hly);
            }
            break;
        case 3:
            if (ENEMY_DamageAll(v20->x, v20->y, v1c->hits))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->x, v20->y);
                else
                    ANIMS_StartAnim(15, v20->x, v20->y);
            }
            break;
        case 0:
            if (ENEMY_DamageAll(v20->x, v20->y, v1c->hits))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->x, v20->y);
                else
                    ANIMS_StartAnim(15, v20->x, v20->y);
            }
            else
            {
                if (TILE_IsHit(v1c->hits, v20->x, v20->y))
                {
                    v20->mobj.done = 1;
                }
            }
            break;
        case 1:
            if (ENEMY_DamageAir(v20->x, v20->y, v1c->hits))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->x, v20->y);
                else
                    ANIMS_StartAnim(15, v20->x, v20->y);
            }
            break;
        case 2:
            if (ENEMY_DamageGround(v20->x, v20->y, v1c->hits))
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(15, v20->x, v20->y);
            }
            else
            {
                if (TILE_IsHit(v1c->hits, v20->x, v20->y))
                {
                    v20->mobj.done = 1;
                }
            }
            break;
        case 4:
            if (TILE_Bomb(v1c->hits, v20->x, v20->y))
            {
                v20->mobj.done = 1;
            }
            if (ENEMY_DamageGround(v20->x, v20->y, 5))
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(1, v20->x, v20->y);
            }
            break;
        }
    LAB_00015d11:
        if (v20->mobj.done)
        {
            if (v20->f_4c)
            {
                v20->f_4c = 0;
                v20->mobj.x2 = v20->mobj.x + ((wrand() % 32) - 16);
                v20->mobj.y2 = 0;
                ANIMS_StartAnim(11, v20->mobj.x, v20->mobj.y);
                InitMobj(&v20->mobj);
            }
            else
            {
                switch (v1c->type)
                {
                case 11:
                    ESHOT_Clear();
                    TILE_DamageAll();
                    for (v24 = first_enemy.next; &last_enemy != v24; v24 = v24->next)
                    {
                        v24->hits -= v1c->hits;
                    }
                    startfadeflag = 1;
                    ANIMS_StartAnim(20, 0, 0);
                    v20 = SHOTS_Remove(v20);
                    continue;
                case 5:
                    break;
                default:
                    v20 = SHOTS_Remove(v20);
                    continue;
                }
            }
        }
        if (v1c->move_flag)
        {
            if (v1c->use_plot)
            {
                MoveSobj(&v20->mobj, v20->speedY);
            }
            else
            {
                v20->mobj.y -= v20->speedY;
                if (v20->mobj.y < 0)
                {
                    v20->mobj.done = 1;
                    v20->f_48 = 1;
                }
            }
        }
    }
}

void SHOTS_Display(void)
{
    int v20, v24, v28;
    shot_t *v1c;
    texture_t * v2c;
    for (v1c = first_shots.next; v1c != &last_shots; v1c = v1c->next)
    {
        switch (v1c->shotLib->beam)
        {
        default:
            EXIT_Error("SHOTS_Display()");
            break;
        case 0:
            GFX_PutSprite(v1c->TexturePtr, v1c->x, v1c->y);
            break;
        case 1:
            GFX_Line(player_cx + 1, player_cy, v1c->mobj.x, v1c->mobj.y, 69);
            GFX_Line(player_cx - 1, player_cy, v1c->mobj.x, v1c->mobj.y, 69);
            GFX_Line(player_cx, player_cy, v1c->mobj.x, v1c->mobj.y, 64);
            v1c = SHOTS_Remove(v1c);
            break;
        case 2:
            for (v20 = v1c->mobj.y2; v20 < v1c->y; v20 += 3)
            {
                GFX_PutSprite(v1c->TexturePtr, v1c->x, v20);
            }
            if (v1c->shotLib->type == 14)
                GFX_PutSprite(detpow[v1c->f_5c], v1c->x - 4, v1c->y);
            else
                GFX_PutSprite(laspow[v1c->f_5c], v1c->x, v1c->y);
            v2c = lashit[v1c->f_5c];
            v24 = v1c->x - (v2c->width >> 2);
            v28 = v1c->mobj.y2 - 8;
            if (v28 > 0)
                GFX_PutSprite(v2c, v24, v28);
            v1c->f_5c++;
            v1c->f_5c = v1c->f_5c % 4;
            break;
        }
    }
}
