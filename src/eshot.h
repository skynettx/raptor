#pragma once
#include "rap.h"
#include "enemy.h"
#include "gfxapi.h"

typedef enum
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
}ESHOT_TYPE;

typedef struct 
{
    int item;                                // BASE ITEM NUMBER
    char *pic[10];                           // POINTER TO PICS
    int num_frames;                          // NUMBER OF FRAMES                    
    int smokeflag;                           // TRUE = SMOKETRAIL                       
    int speed;                               // SPEED OF SHOT
    int xoff;                                // X CENTER OFFSET
    int yoff;                                // Y CENTER OFFSET
    int hits;                                // HIT POINT DAMAGE TO PLAYER
}ESHOT_LIB;

typedef struct ESHOT_S
{
    struct ESHOT_S *prev;                    // LINK LIST PREV
    struct ESHOT_S *next;                    // LINK LIST NEXT
    char *pic;                               // POINTER TO CUR FRAME PIC
    int curframe;                            // CURRENT ANIM FRAME
    int x;                                   // CUR SHOT CENTER X
    int y;                                   // CUR SHOT CENTER Y
    MOVEOBJ move;                            // MOVE STUFF
    int doneflag;                            // SHOT DONE = TRUE
    ESHOT_LIB *lib;                          // POINTER TO LIB
    int cnt;
    int speed;
    int pos;
    int type;
    SPRITE_SHIP *en;
    int gun_num;
}ESHOT;

void ESHOT_Shoot(SPRITE_SHIP *enemy, int gun_num);
void ESHOT_Clear(void);
void ESHOT_Think(void);
void ESHOT_Display(void);
void ESHOT_Init(void);
