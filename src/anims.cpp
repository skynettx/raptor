#include <string.h>
#include "common.h"
#include "anims.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "enemy.h"
#include "rap.h"
#include "tile.h"
#include "fileids.h"
#include "entypes.h"

#define MAX_ANIMLIB 25
#define MAX_ANIMS   100

ANIMS first_anims, last_anims, anims[MAX_ANIMS];
ANIMS *free_anims;

ANIMLIB animlib[MAX_ANIMLIB];
int curlib;

int adir[3] = {
    0, -1, 1
};

/***************************************************************************
ANIMS_Clear () - Clears out All ANIM Objects
 ***************************************************************************/
void 
ANIMS_Clear(
    void
)
{
    int loop;
    
    first_anims.prev = NULL;
    first_anims.next = &last_anims;
    
    last_anims.prev = &first_anims;
    last_anims.next = NULL;
    
    free_anims = anims;
    
    memset(anims, 0, sizeof(anims));
    
    for (loop = 0; loop < MAX_ANIMS - 1; loop++)
        anims[loop].next = &anims[loop + 1];
}

/*-------------------------------------------------------------------------*
ANIMS_Get () - Gets A Free ANIM from Link List
 *-------------------------------------------------------------------------*/
ANIMS
*ANIMS_Get(
    void
)
{
    ANIMS *newa;
    
    if (!free_anims)
        return NULL;
    
    newa = free_anims;
    free_anims = free_anims->next;
    
    memset(newa, 0, sizeof(ANIMS));
    
    newa->next = &last_anims;
    newa->prev = last_anims.prev;
    
    last_anims.prev = newa;
    newa->prev->next = newa;
    
    return newa;
}

/*-------------------------------------------------------------------------*
ANIMS_Remove () Removes ANIM from Link List
 *-------------------------------------------------------------------------*/
ANIMS
*ANIMS_Remove(
    ANIMS *anim
)
{
    ANIMS *next;
    
    next = anim->prev;
    
    anim->next->prev = anim->prev;
    anim->prev->next = anim->next;
    
    memset(anim, 0, sizeof(ANIMS));
    
    anim->next = free_anims;
    
    free_anims = anim;
    
    return next;
}

/***************************************************************************
ANIMS_Register () - Register a ANIM for USE with this stuff
 ***************************************************************************/
int 
ANIMS_Register(
    int item,              // INPUT : lumpnum of first frame
    int numframes,         // INPUT : number of frames
    int groundflag,        // INPUT : on the ground = TRUE
    int playerflag,        // INPUT : follow player movements
    int transparent,       // INPUT : Transparent ( LIGHT )
    int adir               // INPUT : Anim Direction
)
{
    ANIMLIB*cur;
    GFX_PIC *h;
    int handle;
    handle = curlib;
    
    if (curlib >= MAX_ANIMLIB)
        EXIT_Error("ANIMS_Register() - Max LIBs");
    
    cur = &animlib[curlib];
    curlib++;
    
    cur->item = item;
    cur->numframes = numframes;
    cur->groundflag = groundflag;
    cur->playerflag = playerflag;
    cur->transparent = transparent;
    cur->adir = adir;
    
    h = (GFX_PIC*)GLB_LockItem(item);
    cur->xoff = LE_LONG(h->width) >> 1;
    cur->yoff = LE_LONG(h->height) >> 1;
    GLB_FreeItem(item);
    
    return handle;
}

/***************************************************************************
ANIMS_Init () Initializes ANIM Stuff
 ***************************************************************************/
void 
ANIMS_Init(
    void
)
{
    ANIMS_Clear();
    
    memset(animlib, 0, sizeof(animlib));
    
    curlib = 0;
    
    // GROUND EXPLOSIONS
    ANIMS_Register(FILE164_GEXPLO_BLK, 0x2a, GROUND, 0, 0, A_NORM);
    ANIMS_Register(FILE17d_BOOM_PIC, 0x23, GROUND, 0, 0, A_NORM);
    ANIMS_Register(FILE1f5_SPLAT_BLK, 7, GROUND, 0, 0, A_NORM);
    ANIMS_Register(FILE113_BIGSPLAT_BLK, 10, GROUND, 0, 0, A_NORM);
    
    // AIR EXPLOSIONS
    ANIMS_Register(FILE1e5_LGFLAK_BLK, 0xc, HIGH_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE1d8_EXPLO2_BLK, 0xd, GROUND, 0, 0, A_NORM);
    ANIMS_Register(FILE1fc_SMFLAK_BLK, 0xe, HIGH_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE1a0_AIRBOOM_PIC, 0x10, HIGH_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE1ae_NRGBANG_BLK, 0xc, HIGH_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE129_LRBLST_BLK, 4, HIGH_AIR, 0, 0, A_NORM);
    
    // MISC ANIMS
    ANIMS_Register(FILE10a_SSMOKE_BLK, 9, MID_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE10e_SSMOKE_BLK, 5, MID_AIR, 0, 1, A_MOVEDOWN);
    ANIMS_Register(FILE12d_SMOKTRAL_BLK, 4, MID_AIR, 0, 1, A_MOVEUP);
    
    ANIMS_Register(FILE18e_LGHTIN_BLK, 0xe, MID_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE19c_BSPARK_BLK, 9, MID_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE1a5_OSPARK_BLK, 9, MID_AIR, 0, 0, A_NORM);
    ANIMS_Register(FILE1c7_GUNSTR_BLK, 4, MID_AIR, 1, 0, A_NORM);
    
    // GROUND EXPLOSION OVERLAYS
    ANIMS_Register(FILE152_FLARE_PIC, 0x1a, GROUND, 0, 0, A_NORM);
    ANIMS_Register(FILE16c_SPARKLE_PIC, 0x11, GROUND, 0, 0, A_NORM);
    
    // ENERGY GRAB
    ANIMS_Register(FILE18e_LGHTIN_BLK, 0xe, HIGH_AIR, 0, 0, A_NORM);
    
    // SUPER SHIELD
    ANIMS_Register(FILE149_SHIPGLOW_BLK, 4, HIGH_AIR, 1, 1, A_NORM);
}

/***************************************************************************
ANIMS_CachePics() - Cache registered anim pics
 ***************************************************************************/
void 
ANIMS_CachePics(
    void
)
{
    int loop;
    unsigned int frames;
    ANIMLIB *cur;
    
    cur = animlib;
    
    for (loop = 0; loop < curlib; loop++, cur++)
    {
        for (frames = 0; frames < (unsigned int)cur->numframes; frames++)
        {
            GLB_CacheItem(cur->item + frames);
        }
    }
}

/***************************************************************************
ANIMS_FreePics() - Free Up Anims Used
 ***************************************************************************/
void 
ANIMS_FreePics(
    void
)
{
    int loop;
    unsigned int frames;
    ANIMLIB *cur;
    
    cur = animlib;
    
    for (loop = 0; loop < curlib; loop++, cur++)
    {
        for (frames = 0; frames < (unsigned int)cur->numframes; frames++)
        {
            GLB_FreeItem(cur->item + frames);
        }
    }
}

/***************************************************************************
ANIMS_StartAnim () - Start An ANIM Playing
 ***************************************************************************/
void 
ANIMS_StartAnim(
    int handle,            // INPUT : ANIM handle
    int x,                 // INPUT : x position
    int y                  // INPUT : y position
)
{
    ANIMLIB *lib;
    ANIMS *cur;
    lib = &animlib[handle];
    
    cur = ANIMS_Get();
    if (!cur)
        return;
    
    cur->lib = lib;
    cur->x = x - lib->xoff;
    cur->y = y - lib->yoff;
    cur->groundflag = lib->groundflag;
}

/***************************************************************************
ANIMS_StartGAnim () - Start An ANIM Playing with groundflag == GROUND
 ***************************************************************************/
void 
ANIMS_StartGAnim(
    int handle,             // INPUT : ANIM handle        
    int x,                  // INPUT : x position
    int y                   // INPUT : y position
)
{
    ANIMS *cur;
    
    cur = ANIMS_Get();
    if (!cur)
        return;
    
    cur->lib = &animlib[handle];
    cur->x = x;
    cur->y = y;
    cur->groundflag = 0;
}

/***************************************************************************
ANIMS_StartEAnim () - Start An ANIM Playing locked onto ENEMY
 ***************************************************************************/
void 
ANIMS_StartEAnim(
    SPRITE_SHIP *en,        // INPUT : pointer to ENEMY
    int handle,             // INPUT : ANIM handle
    int x,                  // INPUT : x position
    int y                   // INPUT : y position
)
{
    ANIMLIB *lib;
    ANIMS *cur;
    lib = &animlib[handle];
    
    cur = ANIMS_Get();
    if (!cur)
        return;
    
    cur->en = en;
    cur->lib = &animlib[handle];
    cur->x = x - lib->xoff;
    cur->y = y - lib->yoff;
    cur->groundflag = HIGH_AIR;
}

/***************************************************************************
ANIMS_StartAAnim () - Start An ANIM Playing with groundflag == HIGH_AIR
 ***************************************************************************/
void 
ANIMS_StartAAnim(
    int handle,             // INPUT : ANIM handle
    int x,                  // INPUT : x position
    int y                   // INPUT : y position
)
{
    ANIMLIB *lib;
    ANIMS *cur;
    lib = &animlib[handle];
    
    cur = ANIMS_Get();
    if (!cur)
        return;
    
    cur->lib = &animlib[handle];
    cur->x = x - lib->xoff;
    cur->y = y - lib->yoff;
    cur->groundflag = HIGH_AIR;
}

/***************************************************************************
ANIMS_Think () - Does all thinking for ANIMS
 ***************************************************************************/
void 
ANIMS_Think(
    void
)
{
    ANIMS *cur;
    ANIMLIB *lib;

    for (cur = first_anims.next; &last_anims != cur; cur = cur->next)
    {
        lib = cur->lib;
        
        if (cur->curframe >= lib->numframes)
        {
            cur = ANIMS_Remove(cur);
            continue;
        }
        
        cur->item = lib->item + cur->curframe;
        
        if (lib->playerflag)
        {
            cur->dx = player_cx + cur->x;
            cur->dy = player_cy + cur->y;
        }
        else if (cur->en)
        {
            if (cur->en->item == -1)
                cur->edone = 1;
            
            if (!cur->edone)
            {
                cur->dx = cur->en->move.x + cur->x;
                cur->dy = cur->en->move.y + cur->y;
            }
        }
        else
        {
            cur->dx = cur->x;
            cur->dy = cur->y;
        }
        
        switch (lib->adir)
        {
        case 0:
            break;
        case 2:
            cur->y++;
            break;
        case 1:
            cur->y--;
            break;
        }
        
        cur->y += adir[lib->adir];
        
        if ((lib->groundflag == GROUND) && (scroll_flag))
            cur->y++;
        
        cur->curframe++;
    }
}

/***************************************************************************
ANIMS_DisplayGround () - Displays All Active ANIMS on the Ground
 ***************************************************************************/
void 
ANIMS_DisplayGround(
    void
)
{
    ANIMS *cur;
    char *pic;

    for (cur = first_anims.next; &last_anims != cur; cur = cur->next)
    {
        if (cur->groundflag)
            continue;
        
        pic = (char*)GLB_GetItem(cur->item);
        
        if (cur->lib->transparent)
            GFX_ShadeShape(LIGHT, pic, cur->dx, cur->dy);
        else
            GFX_PutSprite(pic, cur->dx, cur->dy);
    }
}

/***************************************************************************
ANIMS_DisplaySky () - Displays All Active ANIMS in SKY
 ***************************************************************************/
void 
ANIMS_DisplaySky(
    void
)
{
    ANIMS *cur;
    char *pic;

    for (cur = first_anims.next; &last_anims != cur; cur = cur->next)
    {
        if (cur->groundflag != MID_AIR)
            continue;
        
        pic = (char*)GLB_GetItem(cur->item);
        
        if (cur->lib->transparent)
            GFX_ShadeShape(LIGHT, pic, cur->dx, cur->dy);
        
        else
            GFX_PutSprite(pic, cur->dx, cur->dy);
    }
}

/***************************************************************************
ANIMS_DisplayHigh () - Displays All Active ANIMS in ABOVE PLAYER
 ***************************************************************************/
void 
ANIMS_DisplayHigh(
    void
)
{
    ANIMS *cur;
    char *pic;

    for (cur = first_anims.next; &last_anims != cur; cur = cur->next)
    {
        if (cur->groundflag != HIGH_AIR)
            continue;
        
        pic = (char*)GLB_GetItem(cur->item);
        
        if (cur->lib->transparent)
            GFX_ShadeShape(LIGHT, pic, cur->dx, cur->dy);
        else
            GFX_PutSprite(pic, cur->dx, cur->dy);
    }
}
