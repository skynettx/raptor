#include <string.h>
#include "common.h"
#include "objects.h"
#include "glbapi.h"
#include "enemy.h"
#include "shots.h"
#include "loadsave.h"
#include "fx.h"
#include "anims.h"
#include "fileids.h"

int obj_cnt;
object_t first_objs, last_objs, objs[MAX_OBJS];
object_t *free_objs;
object_t *p_objs[S_LAST_OBJECT];

objlib_t obj_lib[S_LAST_OBJECT];

int objuse_flag;
int think_cnt;

#define  CHARGE_SHIELD  ( 24 * 4 )

/***************************************************************************
OBJS_Clear () - Clears out All Objects
 ***************************************************************************/
void 
OBJS_Clear(
    void
)
{
    int loop;
    
    obj_cnt = 0;
    
    first_objs.prev = NULL;
    first_objs.next = &last_objs;
    
    last_objs.prev = &first_objs;
    last_objs.next = NULL;
    
    free_objs = objs;
    
    memset(objs, 0, sizeof(objs));
    memset(p_objs, 0, sizeof(p_objs));
    
    for (loop = 0; loop < MAX_OBJS - 1; loop++)
        objs[loop].next = &objs[loop + 1];
}

/*-------------------------------------------------------------------------*
OBJS_Get () - Gets A Free OBJ from Link List
 *-------------------------------------------------------------------------*/
object_t 
*OBJS_Get(
    void
)
{
    object_t *newo;
    
    if (!free_objs)
        return 0;
    
    newo = free_objs;
    free_objs = free_objs->next;
    
    memset(newo, 0, sizeof(object_t));
    
    newo->next = &last_objs;
    newo->prev = last_objs.prev;
    last_objs.prev = newo;
    newo->prev->next = newo;
    
    obj_cnt++;
    
    return newo;
}

/*-------------------------------------------------------------------------*
OBJS_Remove () Removes OBJ from Link List
 *-------------------------------------------------------------------------*/
object_t 
*OBJS_Remove(
    object_t *sh
)
{
    object_t *next;
    
    next = sh->prev;
    
    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;
    
    memset(sh, 0, sizeof(object_t));
    
    sh->next = free_objs;
    
    free_objs = sh;
    
    obj_cnt--;
    
    return next;
}

/***************************************************************************
OBJS_CachePics () - PreLoad bonus/object pictures
 ***************************************************************************/
void 
OBJS_CachePics(
    void
)
{
    int loop, i;
    objlib_t *lib;
    
    for (loop = 0; loop < S_LAST_OBJECT; loop++)
    {
        lib = &obj_lib[loop];
        
        if (lib && lib->item != -1)
        {
            for (i = 0; i < lib->numframes; i++)
            {
                GLB_CacheItem(lib->item + i);
            }
        }
    }
    
    GLB_CacheItem(FILE1e0_SMSHIELD_PIC);
    GLB_CacheItem(FILE1df_SMBOMB_PIC);
}

/***************************************************************************
OBJS_FreePics () - Free bonus/object pictures
 ***************************************************************************/
void 
OBJS_FreePics(
    void
)
{
    int loop, i;
    objlib_t* lib;
    
    for (loop = 0; loop < S_LAST_OBJECT; loop++)
    {
        lib = &obj_lib[loop];
        
        if (lib && lib->item != -1)
        {
            for (i = 0; i < lib->numframes; i++)
            {
                GLB_FreeItem(lib->item + i);
            }
        }
    }
    
    GLB_FreeItem(FILE1e0_SMSHIELD_PIC);
    GLB_FreeItem(FILE1df_SMBOMB_PIC);
}

/***************************************************************************
OBJS_Init () - Sets up object stuff
 ***************************************************************************/
void 
OBJS_Init(
    void
)
{
    objlib_t *lib;
    
    OBJS_Clear();
    
    memset(obj_lib, 0, sizeof(obj_lib));
    memset(p_objs, 0, sizeof(p_objs));
    
    // == FORWARD GUNS ===  *
    lib = &obj_lib[S_FORWARD_GUNS];
    lib->item = FILE1b0_BONUS00_PIC;
    lib->numframes = 1;
    lib->cost = 12000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 0;
    lib->specialw = 0;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == PLASMA GUNS ===  *
    lib = &obj_lib[S_PLASMA_GUNS];
    lib->item = FILE1b1_BONUS01_PIC;
    lib->numframes = 2;
    lib->cost = 78800;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == MICRO MISSLES ===  *
    lib = &obj_lib[S_MICRO_MISSLE];
    lib->item = FILE1b3_BONUS02_PIC;
    lib->numframes = 2;
    lib->cost = 175600;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == DUMB MISSLES === *
    lib = &obj_lib[S_DUMB_MISSLE];
    lib->item = FILE1b5_BONUS03_PIC;
    lib->numframes = 1;
    lib->cost = 145200;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_MISSLE;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == MINI GUN ===
    lib = &obj_lib[S_MINI_GUN];
    lib->item = FILE1b6_BONUS04_PIC;
    lib->numframes = 4;
    lib->cost = 250650;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == LASER TURRET ===
    lib = &obj_lib[S_TURRET];
    lib->item = FILE1ba_BONUS05_PIC;
    lib->numframes = 4;
    lib->cost = 512850;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_LASER;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 0;
    lib->shieldflag = 1;

    // == MISSLE PODS ===
    lib = &obj_lib[S_MISSLE_PODS];
    lib->item = FILE1be_BONUS06_PIC;
    lib->numframes = 1;
    lib->cost = 204950;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == AIR TO AIR MISSLES === *
    lib = &obj_lib[S_AIR_MISSLE];
    lib->item = FILE1bf_BONUS07_PIC;
    lib->numframes = 1;
    lib->cost = 63500;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_MISSLE;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == AIR TO GROUND MISSLES === *
    lib = &obj_lib[S_GRD_MISSLE];
    lib->item = FILE1c0_BONUS08_PIC;
    lib->numframes = 1;
    lib->cost = 110000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_MISSLE;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == GROUND BOMB ===
    lib = &obj_lib[S_BOMB];
    lib->item = FILE1da_BONUS21_PIC;
    lib->numframes = 1;
    lib->cost = 98200;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_MISSLE;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 0;
    lib->shieldflag = 1;

    // == ENERGY GRAB ===
    lib = &obj_lib[S_ENERGY_GRAB];
    lib->item = FILE1c1_BONUS09_PIC;
    lib->numframes = 4;
    lib->cost = 300750;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 0;
    lib->shieldflag = 1;

    // == MEGA BOMB === *
    lib = &obj_lib[S_MEGA_BOMB];
    lib->item = FILE1c5_BONUS10_PIC;
    lib->numframes = 1;
    lib->cost = 32250;
    lib->start_cnt = 1;
    lib->max_cnt = 5;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 0;
    lib->onlyflag = 1;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == PULSE CANNON ===
    lib = &obj_lib[S_PULSE_CANNON];
    lib->item = FILE1c6_BONUS11_PIC;
    lib->numframes = 2;
    lib->cost = 725000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_GUN;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == FORWARD LASER ===
    lib = &obj_lib[S_FORWARD_LASER];
    lib->item = FILE1c8_BONUS12_PIC;
    lib->numframes = 4;
    lib->cost = 1750000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_LASER;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 0;
    lib->shieldflag = 1;

    // == DEATH RAY ===
    lib = &obj_lib[S_DEATH_RAY];
    lib->item = FILE1cc_BONUS13_PIC;
    lib->numframes = 4;
    lib->cost = 950000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->actf = SHOTS_PlayerShoot;
    lib->fxtype = FX_LASER;
    lib->forever = 1;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 1;
    lib->moneyflag = 0;
    lib->game1flag = 0;
    lib->shieldflag = 1;

    // == SUPER SHIELD ===
    lib = &obj_lib[S_SUPER_SHIELD];
    lib->item = FILE1d0_BONUS14_PIC;
    lib->numframes = 1;
    lib->cost = 78500;
    lib->start_cnt = MAX_SHIELD;
    lib->max_cnt = MAX_SHIELD;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 0;
    lib->loseit = 0;
    lib->specialw = 0;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == SHIP NORM ENERGY ===
    lib = &obj_lib[S_ENERGY];
    lib->item = FILE1d1_BONUS15_PIC;
    lib->numframes = 4;
    lib->cost = 400;
    lib->start_cnt = MAX_SHIELD / 4;
    lib->max_cnt = MAX_SHIELD;
    lib->fxtype = -1;
    lib->forever = 1;
    lib->onlyflag = 1;
    lib->loseit = 0;
    lib->specialw = 0;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == ENEMY DAMAGE DETECTOR ===
    lib = &obj_lib[S_DETECT];
    lib->item = FILE1d5_BONUS16_PIC;
    lib->numframes = 1;
    lib->cost = 10000;
    lib->start_cnt = 1;
    lib->max_cnt = 1;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 1;
    lib->loseit = 0;
    lib->specialw = 0;
    lib->moneyflag = 0;
    lib->game1flag = 1;
    lib->shieldflag = 0;

    // == BUY ITEM 1 ===
    lib = &obj_lib[S_ITEMBUY1];
    lib->item = FILE1d5_BONUS16_PIC;
    lib->numframes = 1;
    lib->cost = 93800;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 1;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == BUY ITEM 2 ===
    lib = &obj_lib[S_ITEMBUY2];
    lib->item = FILE1d6_BONUS17_PIC;
    lib->numframes = 1;
    lib->cost = 76000;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 1;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == BUY ITEM 3 ===
    lib = &obj_lib[S_ITEMBUY3];
    lib->item = FILE1d7_BONUS18_PIC;
    lib->numframes = 1;
    lib->cost = 55700;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 1;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == BUY ITEM 4 ===
    lib = &obj_lib[S_ITEMBUY4];
    lib->item = FILE1d8_BONUS19_PIC;
    lib->numframes = 1;
    lib->cost = 35200;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 1;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == BUY ITEM 5 ===
    lib = &obj_lib[S_ITEMBUY5];
    lib->item = FILE1d9_BONUS20_PIC;
    lib->numframes = 1;
    lib->cost = 122500;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 1;
    lib->game1flag = 1;
    lib->shieldflag = 1;

    // == BUY ITEM 6 ===
    lib = &obj_lib[S_ITEMBUY6];
    lib->item = FILE1db_BONUS22_PIC;
    lib->numframes = 4;
    lib->cost = 50;
    lib->start_cnt = lib->cost;
    lib->max_cnt = lib->cost;
    lib->fxtype = -1;
    lib->forever = 0;
    lib->onlyflag = 0;
    lib->loseit = 1;
    lib->specialw = 0;
    lib->moneyflag = 1;
    lib->game1flag = 1;
    lib->shieldflag = 0;
}

/***************************************************************************
OBJS_DisplayStats() - Display game screen object stuff
 ***************************************************************************/
void 
OBJS_DisplayStats(
    void
)
{
    int item;
    int loop, x, maxloop;
    static int dpos;
    
    if (p_objs[S_DETECT])
    {
        loop = ENEMY_GetBaseDamage();
        if (loop > 0)
        {
            GFX_ColorBox(109, MAP_BOTTOM + 9, 102, 8, 74);
            GFX_ColorBox(110, MAP_BOTTOM + 10, loop, 6, 68);
        }
        else
        {
            GFX_VLine(dpos + 110, MAP_BOTTOM + 8, 3, 68);
            GFX_VLine(209 - dpos, MAP_BOTTOM + 8, 3, 68);
            dpos++;
            dpos %= 50;
        }
    }
    
    if (plr.sweapon != -1)
    {
        item = obj_lib[plr.sweapon].item;
        GFX_PutSprite((texture_t*)GLB_GetItem(item), MAP_RIGHT - 18, MAP_TOP);
    }
    
    if (p_objs[S_SUPER_SHIELD])
    {
        x = MAP_LEFT + 2;
        maxloop = OBJS_GetTotal(S_SUPER_SHIELD);
        for (loop = 0; loop < maxloop; loop++)
        {
            GFX_PutSprite((texture_t*)GLB_GetItem(FILE1e0_SMSHIELD_PIC), x, 1);
            x += 13;
        }
    }
    
    if (p_objs[S_MEGA_BOMB])
    {
        x = MAP_LEFT + 2;
        for (loop = 0; loop < p_objs[S_MEGA_BOMB]->num; loop++)
        {
            GFX_PutSprite((texture_t*)GLB_GetItem(FILE1df_SMBOMB_PIC), x, 186);
            x += 13;
        }
    }
}

/*-------------------------------------------------------------------------*
OBJS_Equip () - Equips an OBJ to be used by Player
 *-------------------------------------------------------------------------*/
int                        // RETURN: TRUE/FALSE
OBJS_Equip(
    int type               // INPUT: OBJ type
)
{
    object_t *cur;
    
    for (cur = first_objs.next; &last_objs != cur; cur = cur->next)
    {
        if (cur->type == type && !p_objs[type])
        {
            cur->inuse = 1;
            p_objs[type] = cur;
            return 1;
        }
    }
    
    return 0;
}

/***************************************************************************
OBJS_Load () - Adds new OBJ from OBJ
 ***************************************************************************/
int 
OBJS_Load(
    object_t *inobj         // INPUT : pointer to OBJ     
)
{
    object_t *cur;

    cur = OBJS_Get();
    if (!cur)
        return 0;

    cur->num = inobj->num;
    cur->type = inobj->type;
    cur->lib = &obj_lib[inobj->type];
    cur->inuse = inobj->inuse;
    
    if (cur->inuse)
        p_objs[inobj->type] = cur;
    
    return 1;
}

/***************************************************************************
OBJS_Add () - Adds OBJ ( type ) to players possesions
 ***************************************************************************/
int 
OBJS_Add(
    int type                // INPUT : OBJ type
)
{
    objlib_t *lib;
    object_t *cur;
    
    if (type >= S_LAST_OBJECT)
        return OBJ_ERROR;
    
    g_oldsuper = -1;
    g_oldshield = -1;
    
    lib = &obj_lib[type];
    
    if (lib->moneyflag)
    {
        plr.score += lib->cost;
        return OBJ_GOTIT;
    }
    
    if (!reg_flag && !lib->game1flag)
        return OBJ_GOTIT;
    
    if (lib->onlyflag)
    {
        for (cur = first_objs.next; &last_objs != cur; cur = cur->next)
        {
            if (cur->type == type)
            {
                if (cur->num >= lib->max_cnt)
                    return OBJ_SHIPFULL;
                
                cur->num += lib->start_cnt;
                if (cur->num > lib->max_cnt)
                    cur->num = lib->max_cnt;
                
                return OBJ_GOTIT;
            }
        }
    }
    
    cur = OBJS_Get();
    if (!cur)
        return OBJ_SHIPFULL;
    
    cur->num = lib->start_cnt;
    cur->type = type;
    cur->lib = lib;
    
    // == equip item if needed =====
    if (!p_objs[type])
    {
        cur->inuse = 1;
        p_objs[type] = cur;
        if (plr.sweapon == -1 && lib->specialw)
        {
            plr.sweapon = type;
        }
    }
    
    return OBJ_GOTIT;
}

/***************************************************************************
OBJS_Del () - Removes Object From User Posession
 ***************************************************************************/
void 
OBJS_Del(
    int type               //INPUT : OBJ type
)
{
    object_t *cur;
    cur = p_objs[type];
    
    if (cur)
    {
        OBJS_Remove(cur);
        p_objs[type] = NULL;
        OBJS_Equip(type);
        
        if (type == plr.sweapon)
            OBJS_GetNext();
    }
}

/***************************************************************************
OBJS_GetNext () - Sets plr.sweapon to next available weapon
 ***************************************************************************/
void 
OBJS_GetNext(
    void
)
{
    int loop, pos, setval;
    object_t *cur;

    setval = -1;
    
    if (plr.sweapon < S_DUMB_MISSLE)
        pos = S_DUMB_MISSLE;
    else
        pos = plr.sweapon + 1;
    
    for (loop = FIRST_SPECIAL; loop <= LAST_WEAPON; loop++)
    {
        if (pos > LAST_WEAPON)
            pos = FIRST_SPECIAL;
        
        cur = p_objs[pos];
        
        if (cur && cur->num && cur->lib->specialw)
        {
            setval = pos;
            break;
        }
        pos++;
    }
    
    plr.sweapon = setval;
}

/***************************************************************************
OBJS_Use () - Player Use An Object
 ***************************************************************************/
void 
OBJS_Use(
    int type              //INPUT : OBJ type 
)
{
    object_t *cur;
    objlib_t *lib;

    cur = p_objs[type];
    lib = &obj_lib[type];
    
    if (cur)
    {
        objuse_flag = 1;
        think_cnt = 0;
        
        if (lib->actf(type) && !lib->forever)
        {
            cur->num--;
        }
        
        if (cur->num <= 0 && !lib->forever)
        {
            OBJS_Remove(cur);
            p_objs[type] = NULL;
            OBJS_Equip(type);
            if (plr.sweapon == type)
                OBJS_GetNext();
        }
    }
}

/***************************************************************************
OBJS_Sell () - Sell Object from player posesion
 ***************************************************************************/
int                         // RETRUN: amount left
OBJS_Sell(
    int type                // INPUT : OBJ type
)
{
    object_t *cur;
    objlib_t *lib;
    int rval;
    
    cur = p_objs[type];
    lib = &obj_lib[type];
    
    if (!cur)
        return 0;
    
    plr.score += OBJS_GetResale(type);
    
    if (type == S_DETECT)
    {
        p_objs[type] = NULL;
        return 0;
    }
    
    if (lib->onlyflag)
    {
        cur->num -= lib->start_cnt;
        
        if (cur->num <= 0)
        {
            rval = 0;
            cur->num = 0;
            if (!lib->forever)
            {
                OBJS_Remove(cur);
                p_objs[type] = NULL;
                OBJS_Equip(type);
                if (plr.sweapon == type)
                    OBJS_GetNext();
            }
        }
        else
            rval = cur->num;
    }
    else
    {
        OBJS_Del(type);
        
        rval = OBJS_GetTotal(type);
    }
    
    return rval;
}

/***************************************************************************
OBJS_Buy () - Add Amount from TYPE that is equiped ( BUY )
 ***************************************************************************/
int                        // RETURN: see BUYSTUFF
OBJS_Buy(
    unsigned int type      // INPUT : OBJ type
)
{
    int rval, num;

    rval = OBJ_NOMONEY;
    
    if (type == S_SUPER_SHIELD)
    {
        num = OBJS_GetTotal(S_SUPER_SHIELD);
        if (num >= 5)
            return OBJ_SHIPFULL;
    }
    
    if ((unsigned int)OBJS_GetCost(type) <= plr.score)
    {
        rval = OBJS_Add(type);
        
        if (!rval)
            plr.score -= OBJS_GetCost(type);
    }
    
    return rval;
}

/***************************************************************************
OBJS_SubAmt () - Subtract Amount From Equiped Item
 ***************************************************************************/
int                        // RETURN: return nums in OBJ
OBJS_SubAmt(
    int type,              // INPUT : OBJ type
    int amt                // INPUT : amount to subtract
)
{
    object_t* cur;

    cur = p_objs[type];
    
    if (!cur)
        return 0;
    
    cur->num -= amt;
    
    if (cur->num < 0)
        cur->num = 0;
    
    return cur->num;
}

/***************************************************************************
OBJS_SetAmt() - Set the number of items within TYPE in Equiped Items
 ***************************************************************************/
int                        // RETURN: return nums in OBJ
OBJS_SetAmt(
    int type,               // INPUT : OBJ type
    int amt               // INPUT : Amount type
)
{
    object_t* cur;

    cur = p_objs[type];
    
    if (!cur)
        return 0;

    cur->num = amt;
    return cur->num;
}

/***************************************************************************
OBJS_GetAmt() - Returns number of items within TYPE in Equiped Items
 ***************************************************************************/
int                        // RETURN: return nums in OBJ
OBJS_GetAmt(
    int type               // INPUT : OBJ type
)
{
    object_t* cur;

    cur = p_objs[type];
    
    if (!cur)
        return 0;
    
    return cur->num;
}

/***************************************************************************
OBJS_GetTotal() - Returns number of items within TYPE in all OBJS
 ***************************************************************************/
int                        // RETURN: return nums in OBJ
OBJS_GetTotal(
    int type               // INPUT : OBJ type
)
{
    int total;
    object_t *cur;

    total = 0;
    
    for (cur = first_objs.next; &last_objs != cur; cur = cur->next)
    {
        if (type == cur->type)
            total++;
    }
    
    return total;
}

/***************************************************************************
OBJS_IsOnly () - Is Onlyflag set
 ***************************************************************************/
int                        // RETURN: TRUE/FALSE
OBJS_IsOnly(
    int type               // INPUT : OBJ type
)
{
    objlib_t *lib;

    lib = &obj_lib[type];
    
    return lib->onlyflag;
}

/***************************************************************************
OBJS_GetCost () - Returns The game COST of an object
 ***************************************************************************/
int                        // RETURN: cost of object
OBJS_GetCost(
    int type               // INPUT : OBJ type
)
{
    objlib_t* lib;
    int cost;

    lib = &obj_lib[type];
    
    if (!lib)
        return 99999999;
    
    if (lib->onlyflag)
        cost = lib->cost * lib->start_cnt;
    else
        cost = lib->cost;
    
    return cost;
}

/***************************************************************************
OBJS_GetResale () - Returns The game Resale Value of an object
 ***************************************************************************/
int                          // RETURN: cost of object
OBJS_GetResale(
    int type                 // INPUT : OBJ type
)
{
    object_t *cur;
    objlib_t* lib;
    int cost;

    cur = p_objs[type];
    lib = &obj_lib[type];
    
    if (!cur)
        return 0;
    
    if (lib->onlyflag)
        cost = lib->cost * lib->start_cnt;
    else
        cost = lib->cost;
    
    return cost >> 1;
}

/***************************************************************************
OBJS_CanBuy() - Returns TRUE if player can buy object
 ***************************************************************************/
int 
OBJS_CanBuy(
    int type               // INPUT : OBJ type
)
{
    int cost;
    objlib_t *lib;
    
    lib = &obj_lib[type];
    
    if (type >= S_LAST_OBJECT)
        return 0;
    
    if (type == S_FORWARD_GUNS && OBJS_IsEquip(type))
        return 0;

    if (!reg_flag && !lib->game1flag)
        return 0;

    cost = OBJS_GetCost(type);
    
    if (!cost)
        return 0;
    
    return 1;
}

/***************************************************************************
OBJS_CanSell() - Returns TRUE if player can Sell object
 ***************************************************************************/
int 
OBJS_CanSell(
    int type               // INPUT : OBJ type
)
{
    object_t *cur;
    objlib_t *lib;
    
    cur = p_objs[type];
    lib = &obj_lib[type];
    
    if (type >= S_LAST_OBJECT)
        return 0;
    
    if (!cur)
        return 0;
    
    if (lib->onlyflag && type == S_ENERGY && cur->num <= lib->start_cnt)
        return 0;
    
    if (cur->num < lib->start_cnt)
        return 0;
    
    return 1;
}

/***************************************************************************
OBJS_GetNum () - Returns number of Objects that player has
 ***************************************************************************/
int                        // RETURN: number of objects                     
OBJS_GetNum(
    void
)
{
    return obj_cnt;
}

/***************************************************************************
OBJS_GetLib () - Returns Pointer to Lib Object
 ***************************************************************************/
objlib_t 
*OBJS_GetLib(
    int type               // INPUT : OBJ type
)
{
    objlib_t *lib;
    
    lib = &obj_lib[type];
    
    return lib;
}

/***************************************************************************
OBJS_IsEquip() - Returns TRUE if item is Equiped
 ***************************************************************************/
int                        // RETURN: return nums in OBJ
OBJS_IsEquip(
    int type               // INPUT : OBJ type
)
{
    if (p_objs[type])
        return 1;
    
    return 0;
}

/***************************************************************************
OBJS_SubEnergy()
 ***************************************************************************/
int                            // RETURN: return nums in OBJ
OBJS_SubEnergy(
    int amt                    // INPUT : amount to subtract
)
{
    object_t *cur;

    if (godmode)
        return 0;
    
    if (startendwave != -1)
        return 0;

    cur = p_objs[S_SUPER_SHIELD];
    
    if (curplr_diff == DIFF_0 && amt > 1)
        amt >>= 1;
    
    if (cur)
    {
        ANIMS_StartAnim(A_SUPER_SHIELD, 0, 0);
        
        SND_Patch(FX_SHIT, 127);
        
        cur->num -= amt;
        
        if (cur->num < 0)
            OBJS_Del(S_SUPER_SHIELD);
    }
    else
    {
        cur = p_objs[S_ENERGY];
        if (!cur)
            return 0;
        
        SND_Patch(FX_HIT, 127);
        
        cur->num -= amt;
        
        if (cur->num < 0)
            cur->num = 0;
    }
    
    return cur->num;
}

/***************************************************************************
OBJS_AddEnergy()
 ***************************************************************************/
int                           // RETURN: return nums in OBJ
OBJS_AddEnergy(
    int amt                   // INPUT : amount to add
)
{
    object_t *cur;
    
    cur = p_objs[S_ENERGY];
    
    if (!cur)
        return 0;
    
    if (cur->num < cur->lib->max_cnt)
    {
        cur = p_objs[S_ENERGY];
        
        if (!cur)
            return 0;
        
        cur->num += amt;
        
        if (cur->num > cur->lib->max_cnt)
            cur->num = cur->lib->max_cnt;
    }
    else
    {
        cur = p_objs[S_SUPER_SHIELD];
        
        if (!cur)
            return 0;
        
        if (!cur->num)
            return 0;
        
        cur->num += (amt >> 2);
        
        if (cur->num > cur->lib->max_cnt)
            cur->num = cur->lib->max_cnt;
    }
    
    return cur->num;
}

/***************************************************************************
OBJS_LoseObj() - Lose random object
 ***************************************************************************/
int 
OBJS_LoseObj(
    void
)
{
    int rval, type;
    objlib_t *lib;
    rval = 1;
    
    if (plr.sweapon == -1)
    {
        for (type = S_LAST_OBJECT - 1; type >= 0; type--)
        {
            lib = &obj_lib[type];
            if (p_objs[type] && lib->loseit)
            {
                OBJS_Del(type);
                rval = 1;
                break;
            }
        }
    }
    else
    {
        OBJS_Del(plr.sweapon);
        rval = 1;
    }
    
    return rval;
}

/***************************************************************************
OBJS_Think () - Does all in game thinking ( recharing )
 ***************************************************************************/
void 
OBJS_Think(
    void
)
{
    if (curplr_diff < DIFF_3)
    {
        if (objuse_flag)
            objuse_flag = 0;
        else
        {
            think_cnt++;
            
            if (think_cnt > CHARGE_SHIELD)
            {
                if (startendwave == -1)
                    OBJS_AddEnergy(1);
                think_cnt = 0;
            }
        }
    }
}

/***************************************************************************
OBJS_MakeSpecial() - Makes the selected weapon the current special
 ***************************************************************************/
int 
OBJS_MakeSpecial(
    int type               // INPUT : OBJ type
)
{
    object_t *cur;
    objlib_t *lib;

    cur = p_objs[type];
    lib = &obj_lib[type];
    
    if (type >= S_LAST_OBJECT)
        return 0;
    
    if (!cur)
        return 0;
    
    if (!lib->specialw)
        return 0;
    
    plr.sweapon = type;
    
    return 1;
}

