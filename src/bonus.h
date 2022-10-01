#pragma once

#include "objects.h"

#define MAX_BONUS 12
#define BONUS_WIDTH 16
#define BONUS_HEIGHT 16

#define MAX_MONEY ( MAX_BONUS - 3 )

struct bonus_t {
    bonus_t *prev;                    // LINK LIST PREV
    bonus_t *next;                    // LINK LIST NEXT
    int item;                         // GLB item
    int curframe;                     // CURRENT ANIM FRAME
    int curglow;                      // CURRENT GLOW FRAME
    int x;                            // X POSITION
    int y;                            // Y POSITION
    int bx;                           // BONUS X POSITION
    int by;                           // BONUS Y POSITION
    int gx;                           // GLOW X POSITION
    int gy;                           // GLOW Y POSITION
    int pos;                          // POS IN CIRCLE
    int dflag;                        // DONEFLAG
    int countdown;                    // COUNTDOWN TO REMOVE
    int type;                         // OBJECT TYPE 
    objlib_t *lib;                    // POINTER TO OBJECT LIBRARY
};

void BONUS_Clear(void);
void BONUS_Think(void);
void BONUS_Display(void);
void BONUS_Init(void);
void BONUS_Add(int type, int x, int y);
