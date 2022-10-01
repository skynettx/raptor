#pragma once
#include "rap.h"
#include "enemy.h"
#include "gfxapi.h"

enum ESHOT_TYPE
{
    ES_ATPLAYER,      // 0
    ES_ATDOWN,        // 1
    ES_ANGLELEFT,     // 2
    ES_ANGLERIGHT,    // 3
    ES_MISSLE,        // 4
    ES_LASER,         // 5
    ES_MINES,         // 6
    ES_PLASMA,        // 7
    ES_COCONUTS       // 8
};

struct plib_t {
    int item;                                // BASE ITEM NUMBER
    texture_t *pic[10];                      // POINTER TO PICS
    int num_frames;                          // NUMBER OF FRAMES                    
    int smokeflag;                           // TRUE = SMOKETRAIL                       
    int speed;                               // SPEED OF SHOT
    int xoff;                                // X CENTER OFFSET
    int yoff;                                // Y CENTER OFFSET
    int hits;                                // HIT POINT DAMAGE TO PLAYER
};

struct eshot_t {
    eshot_t *prev;                           // LINK LIST PREV
    eshot_t *next;                           // LINK LIST NEXT
    texture_t *pic;                          // POINTER TO CUR FRAME PIC
    int curframe;                            // CURRENT ANIM FRAME
    int x;                                   // CUR SHOT CENTER X
    int y;                                   // CUR SHOT CENTER Y
    mobj_t move;                             // MOVE STUFF
    int doneflag;                            // SHOT DONE = TRUE
    plib_t *lib;                             // POINTER TO LIB
    int cnt;
    int speed;
    int pos;
    int type;
    enemy_t *en;
    int gun_num;
};

void ESHOT_Shoot(enemy_t *enemy, int gun_num);
void ESHOT_Clear(void);
void ESHOT_Think(void);
void ESHOT_Display(void);
void ESHOT_Init(void);
