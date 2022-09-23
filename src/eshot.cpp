#include <string.h>
#include "common.h"
#include "eshot.h"
#include "rap.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "enemy.h"
#include "fx.h"
#include "shots.h"
#include "objects.h"
#include "anims.h"
#include "joyapi.h"
#include "input.h"
#include "fileids.h"

#define MAX_ESHOT 80

enum LIB_PIC
{
    LIB_NORMAL,
    LIB_ATPLAY,
    LIB_MISSLE,
    LIB_LASER,
    LIB_MINES,
    LIB_PLASMA,
    LIB_COCO,
    LIB_LASTPIC
};

int eshotnum, eshothigh;
eshot_t first_eshot, last_eshot;
eshot_t eshots[MAX_ESHOT];
eshot_t *free_eshot;

texture_t *elaspow[4];

plib_t plib[LIB_LASTPIC];

static int xpos[16] = {
    -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2
};

static int ypos[16] = {
    -3, -3, -3, -2, -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2
};

static int monkeys[6] = {
    FX_MON1, FX_MON2, FX_MON3, FX_MON4, FX_MON5, FX_MON6
};

/***************************************************************************
ESHOT_Clear () - Clears out ESHOT Linklist
 ***************************************************************************/
void 
ESHOT_Clear(
    void
)
{
    int loop;
    
    eshotnum = 0;
    
    first_eshot.prev = NULL;
    first_eshot.next = &last_eshot;
    
    last_eshot.prev = &first_eshot;
    last_eshot.next = NULL;
    
    free_eshot = &eshots[0];
    
    memset(eshots, 0, sizeof(eshots));
    
    for (loop = 0; loop < MAX_ESHOT - 1; loop++)
        eshots[loop].next = &eshots[loop + 1];
}

/*-------------------------------------------------------------------------*
ESHOT_Get () - gets a Free ESHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
eshot_t 
*ESHOT_Get(
    void
)
{
    eshot_t *newes;
    
    if (!free_eshot)
        return NULL;
    
    eshotnum++;
    if (eshothigh < eshotnum)
        eshothigh = eshotnum;
    
    newes = free_eshot;
    free_eshot = free_eshot->next;
    
    memset(newes, 0, sizeof(eshot_t));
    
    newes->next = &last_eshot;
    newes->prev = last_eshot.prev;
    last_eshot.prev = newes;
    newes->prev->next = newes;
    
    return newes;
}

/*-------------------------------------------------------------------------*
ESHOT_Remove () - Removes SHOT OBJECT from linklist
 *-------------------------------------------------------------------------*/
eshot_t 
*ESHOT_Remove(
    eshot_t *sh
)
{
    eshot_t *next;
    
    eshotnum--;
    
    next = sh->prev;
    
    sh->next->prev = sh->prev;
    sh->prev->next = sh->next;
    
    memset(sh, 0, sizeof(eshot_t));
    
    sh->next = free_eshot;
    
    free_eshot = sh;
    
    return next;
}

/***************************************************************************
ESHOT_Init () - Inits ESHOT system and clears link list
 ***************************************************************************/
void 
ESHOT_Init(
    void
)
{
    plib_t *cur;
    texture_t *h;
    int loop;
    
    for (loop = 0; loop < 4; loop++)
        elaspow[loop] = (texture_t*)GLB_LockItem(FILE145_ELASEPOW_BLK + loop);
    
    ESHOT_Clear();
    
    memset(plib, 0, sizeof(plib));
    
    cur = &plib[LIB_NORMAL];                          
    cur->hits = 2;
    cur->item = FILE1d6_ESHOT_BLK;
    cur->num_frames = 2;
    cur->smokeflag = 0;
    cur->speed = 6;
    for (loop = 0; loop < cur->num_frames; loop++)
        cur->pic[loop] = (texture_t*)GLB_LockItem(cur->item + loop);
    h = cur->pic[0];
    cur->xoff = h->width >> 1;
    cur->yoff = h->height >> 1;

    cur = &plib[LIB_ATPLAY];                          
    cur->hits = 1;
    cur->item = FILE1d6_ESHOT_BLK;
    cur->num_frames = 2;
    cur->smokeflag = 0;
    cur->speed = 6;
    for (loop = 0; loop < cur->num_frames; loop++)
        cur->pic[loop] = (texture_t*)GLB_LockItem(cur->item + loop);
    h = cur->pic[0];
    cur->xoff = h->width >> 1;
    cur->yoff = h->height >> 1;

    cur = &plib[LIB_MISSLE];                         
    cur->hits = 4;
    cur->item = FILE1d4_EMISLE_BLK;
    cur->num_frames = 2;
    cur->smokeflag = 1;
    cur->speed = 10;
    for (loop = 0; loop < cur->num_frames; loop++)
        cur->pic[loop] = (texture_t*)GLB_LockItem(cur->item + loop);
    h = cur->pic[0];
    cur->xoff = h->width >> 1;
    cur->yoff = h->height >> 1;

    cur = &plib[LIB_MINES];                        
    cur->hits = 0x10;
    cur->item = FILE1d1_MINE_BLK;
    cur->num_frames = 2;
    cur->smokeflag = 0;
    cur->speed = 0;
    for (loop = 0; loop < cur->num_frames; loop++)
        cur->pic[loop] = (texture_t*)GLB_LockItem(cur->item + loop);
    h = cur->pic[0];
    cur->xoff = h->width >> 1;
    cur->yoff = h->height >> 1;

    cur = &plib[LIB_LASER];                        
    cur->hits = 0xc;
    cur->item = FILE141_ELASER_BLK;
    cur->num_frames = 4;
    cur->smokeflag = 0;
    cur->speed = 6;
    for (loop = 0; loop < cur->num_frames; loop++)
        cur->pic[loop] = (texture_t*)GLB_LockItem(cur->item + loop);
    h = cur->pic[0];
    cur->xoff = h->width >> 1;
    cur->yoff = h->height >> 1;

    cur = &plib[LIB_PLASMA];                       
    cur->hits = 0xf;
    cur->item = FILE151_EPLASMA_PIC;
    cur->num_frames = 1;
    cur->smokeflag = 0;
    cur->speed = 10;
    for (loop = 0; loop < cur->num_frames; loop++)
        cur->pic[loop] = (texture_t*)GLB_LockItem(cur->item + loop);
    h = cur->pic[0];
    cur->xoff = h->width >> 1;
    cur->yoff = h->height >> 1;

    cur = &plib[LIB_COCO];                       
    cur->hits = 1;
    cur->item = FILE14d_COCONUT_PIC;
    cur->num_frames = 4;
    cur->smokeflag = 0;
    cur->speed = 6;
    for (loop = 0; loop < cur->num_frames; loop++)
        cur->pic[loop] = (texture_t*)GLB_LockItem(cur->item + loop);
    h = cur->pic[0];
    cur->xoff = h->width >> 1;
    cur->yoff = h->height >> 1;
}

/***************************************************************************
ESHOT_Shoot() - Shoots ENEMY GUNS
 ***************************************************************************/
void 
ESHOT_Shoot(
    enemy_t *enemy,        // INPUT : pointer to Enemy stuff
    int gun_num            // INPUT : gun number to shoot
)
{
    int x;
    int y;
    int g_shoot_type;
    eshot_t *cur;
    
    x = enemy->x + enemy->lib->shootx[gun_num];
    y = enemy->y + enemy->lib->shooty[gun_num];
    
    if (((x >= 0) && (x < 320)) && ((y >= 0) && (y < 200)))
    {
        cur = ESHOT_Get();
        
        if (!cur)
            return;
        
        cur->move.x = x;
        cur->move.y = y;
        cur->en = enemy;
        cur->gun_num = gun_num;
        g_shoot_type = enemy->lib->shoot_type[gun_num];
        
        switch (g_shoot_type)
        {
        default:
            EXIT_Error("ESHOT_Shoot() - Invalid EShot type");
            break;
        
        case ES_ATPLAYER:                                              
            SND_3DPatch(FX_ENEMYSHOT, x, y);
            cur->lib = &plib[LIB_ATPLAY];
            cur->move.x -= cur->lib->xoff;
            cur->move.y -= cur->lib->yoff;
            cur->move.x2 = player_cx;
            cur->move.y2 = player_cy;
            cur->speed = 1;
            cur->type = ES_ATPLAYER;
            break;
        
        case ES_ATDOWN:                                             
            SND_3DPatch(FX_ENEMYSHOT, x, y);
            cur->lib = &plib[LIB_NORMAL];
            cur->move.x -= cur->lib->xoff;
            cur->move.y -= cur->lib->yoff;
            cur->move.x2 = cur->move.x;
            cur->move.y2 = 200;
            cur->speed = cur->lib->speed >> 1;
            cur->type = ES_ATDOWN;
            break;
        
        case ES_ANGLELEFT:                                              
            SND_3DPatch(FX_ENEMYSHOT, x, y);
            cur->lib = &plib[LIB_NORMAL];
            cur->move.x -= cur->lib->xoff;
            cur->move.y -= cur->lib->yoff;
            cur->move.x2 = cur->move.x - 32;
            cur->move.y2 = cur->move.y + 32;
            cur->speed = cur->lib->speed >> 1;
            cur->type = ES_ANGLELEFT;
            break;
        
        case ES_ANGLERIGHT:                                              
            SND_3DPatch(FX_ENEMYSHOT, x, y);
            cur->lib = &plib[LIB_NORMAL];
            cur->move.x -= cur->lib->xoff;
            cur->move.y -= cur->lib->yoff;
            cur->move.x2 = cur->move.x + 32;
            cur->move.y2 = cur->move.y + 32;
            cur->speed = cur->lib->speed >> 1;
            cur->type = ES_ANGLERIGHT;
            break;
        
        case ES_MISSLE:                                              
            SND_3DPatch(FX_ENEMYMISSLE, x, y);
            cur->lib = &plib[LIB_MISSLE];
            cur->move.x -= cur->lib->xoff;
            cur->move.x2 = cur->move.x;
            cur->move.y2 = 200;
            cur->speed = enemy->speed + 1;
            cur->type = ES_MISSLE;
            break;
            
        case ES_LASER:                                              
            SND_3DPatch(FX_ENEMYLASER, x, y);
            cur->lib = &plib[LIB_LASER];
            cur->move.x -= cur->lib->xoff;
            cur->move.x2 = cur->move.x;
            cur->move.y2 = 200;
            cur->speed = enemy->speed;
            cur->type = ES_LASER;
            break;
        
        case ES_MINES:                                              
            SND_3DPatch(FX_ENEMYSHOT, x, y);
            cur->lib = &plib[LIB_MINES];
            cur->x = cur->move.x;
            cur->y = cur->move.y;
            cur->move.x2 = 320;
            cur->move.y2 = 200;
            cur->speed = 150;
            cur->pos = wrand() % 16;
            cur->type = ES_MINES;
            break;
        
        case ES_PLASMA:                                              
            SND_3DPatch(FX_ENEMYPLASMA, x, y);
            cur->lib = &plib[LIB_PLASMA];
            cur->move.x -= cur->lib->xoff;
            cur->move.x2 = cur->move.x;
            cur->move.y2 = 200;
            cur->speed = 8;
            cur->type = ES_PLASMA;
            break;
        
        case ES_COCONUTS:                                              
            SND_3DPatch(monkeys[wrand() % 6], x, y);           
            cur->lib = &plib[LIB_COCO];
            cur->move.x -= cur->lib->xoff;
            cur->move.y -= cur->lib->yoff;
            cur->move.x2 = player_cx;
            cur->move.y2 = player_cy;
            cur->speed = 1;
            cur->type = ES_COCONUTS;
            break;
        }
        
        InitMobj(&cur->move);
        MoveSobj(&cur->move, 1);
        
        if (cur->move.x < 0 || cur->move.x >= 320)
            cur->move.done = 1;
        
        if (cur->move.y < 0 || cur->move.y >= 200)
            cur->move.done = 1;
        
        if (cur->move.done)
            ESHOT_Remove(cur);
    }
}

/***************************************************************************
ESHOT_Think () - Does All Thinking for shot system
 ***************************************************************************/
void 
ESHOT_Think(
    void
)
{
    eshot_t *shot;
    plib_t *lib;
    int dx, dy;

    for (shot = first_eshot.next; shot!=&last_eshot; shot = shot->next)
    {
        lib = shot->lib;
        
        shot->pic = lib->pic[shot->curframe];
        
        shot->curframe++;
        
        switch (shot->type)
        {
        case ES_LASER:
            if (shot->en && shot->en->lib && shot->curframe < lib->num_frames)
            {
                shot->x = shot->en->x + shot->en->lib->shootx[shot->gun_num] - 4;
                shot->y = shot->en->y + shot->en->lib->shooty[shot->gun_num];
                shot->move.y2 = 200;  
                
                dx = abs(shot->x - player_cx);
                
                if (dx < (PLAYERWIDTH / 2) && shot->y < player_cy)
                {
                    shot->move.y2 = player_cy + (wrand() % 4) - 2;
                    OBJS_SubEnergy(lib->hits);
                    if ((haptic) && (control == 2))
                    {
                        IPT_CalJoyRumbleLow();                                            //Rumble when Laser eshot is hit
                    }
                }
            }
            else
            {
                shot->doneflag = 1;
            }
            break;
        
        default:
            if (shot->curframe >= lib->num_frames)
                shot->curframe = 0;
            
            if (lib->speed)
            {
                shot->x = shot->move.x;
                shot->y = shot->move.y;
                
                MoveSobj(&shot->move, shot->speed);
                
                if (shot->speed < lib->speed)
                    shot->speed++;
            }
            else
            {
                shot->speed--;
                
                if (shot->speed)
                {
                    shot->x = shot->move.x + xpos[shot->pos];
                    shot->y = shot->move.y + ypos[shot->pos];
                    shot->move.y++;
                    
                    shot->pos++;
                    
                    if (shot->pos >= 16)
                        shot->pos = 0;
                }
                else
                {
                    shot->doneflag = 1;
                    ANIMS_StartAnim(A_SMALL_AIR_EXPLO, shot->x + 4, shot->y + 4);
                }
            }
            
            if (shot->y >= 200 || shot->y < 0)
                shot->doneflag = 1;
            
            if (shot->x >= 320 || shot->x < 0)
                shot->doneflag = 1;
            
            dx = abs(shot->x - player_cx);
            dy = abs(shot->y - player_cy);
            
            if (dx < (PLAYERWIDTH / 2) && dy < (PLAYERWIDTH / 2))
            {
                ANIMS_StartAnim(A_SMALL_AIR_EXPLO, shot->x, shot->y);
                shot->doneflag = 1;
                OBJS_SubEnergy(lib->hits);
                if ((haptic) && (control == 2))
                {
                    IPT_CalJoyRumbleLow();                                                                 //Rumble when eshot is hit
                }
            }
            break;
        }
        
        if (shot->doneflag)
        {
            shot = ESHOT_Remove(shot);
            continue;
        }
        
        shot->cnt++;
        
        if (lib->smokeflag && (shot->cnt & 1) != 0)
        {
            ANIMS_StartAAnim(A_SMALL_SMOKE_UP, shot->x + lib->xoff, shot->y);
        }
    }
}

/***************************************************************************
ESHOT_Display () - Displays All active Shots
 ***************************************************************************/
void 
ESHOT_Display(
    void
)
{
    eshot_t *shot;
    int loop, y;
    texture_t *h;
    
    for (shot = first_eshot.next; shot !=&last_eshot; shot = shot->next)
    {
        if (shot->type == ES_LASER)
        {
            for (loop = shot->y; loop < shot->move.y2; loop += 3)
                GFX_PutSprite(shot->pic, shot->x, loop);
            
            GFX_PutSprite(elaspow[shot->curframe - 1], shot->x, shot->y);
            
            h = lashit[shot->curframe - 1];
            
            y = shot->move.y2 - 8;
            
            if (y > 0 && y < 200)
            {
                GFX_PutSprite(h, shot->x - (h->width >> 2), y);
            }
        }
        else
            GFX_PutSprite(shot->pic, shot->x, shot->y);
    }
}
