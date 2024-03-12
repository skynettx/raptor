#pragma once

#include "rap.h"
#include "gfxapi.h"

#define MAX_SHOTS 70

typedef enum
{
    S_ALL,
    S_AIR,
    S_GROUND,
    S_GRALL,
    S_GTILE,
    S_SUCK
}HIT_TYPE;

typedef enum
{
    S_SHOOT,
    S_LINE,
    S_BEAM
}BEAM_TYPE;

typedef struct 
{
    int lumpnum;                            // LUMPNUM OF PIC(S)
    char *pic[10];                          // POINTER TO PIC
    int shadow;                             // SHADOW ON/OFF
    int type;                               // SHOT TYPE
    int hits;                               // DAMAGE SHOT WILL DO
    int speed;                              // START SPEED
    int maxspeed;                           // MAXIMUM SPEED
    int startframe;                         // START ANIM ON FRAME
    int numframes;                          // NUMBER OF FRAMES OF ANIM
    int shoot_rate;                         // SHOOT EVERY (N) FRAMES
    int cur_shoot;                          // HOLDS FRAME COUNT DOWN
    int delayflag;                          // DEALAYED ANIM START
    int smoke;                              // SMOKE TRAIL
    int use_plot;                           // TRUE = PLOT X,Y ( SLOWER )
    int move_flag;                          // DOES SHOT NEED MOVEMENT
    int ht;                                 // SHOT HIT TYPE ( AIR/GROUND )
    int fplrx;                              // FOLLOW PLAYER X POS
    int fplry;                              // FOLLOW PLAYER Y POS
    int meffect;                            // TRACK MOVE TO FIND HIT ?
    int beam;                               // BEAM WEAPON TYPE
    GFX_PIC *h;
    int hlx;
    int hly;
}SHOT_LIB;

typedef struct SHOTS_S
{
    struct SHOTS_S *prev;                   // Link List Prev
    struct SHOTS_S *next;                   // Link List Next
    char *pic;                              // pointer to cur frame pic
    int x;                                  // cur shot center x
    int y;                                  // cur shot center y
    MOVEOBJ move;                           // MOVE stuff
    int speed;                              // cur speed
    int curframe;                           // current anim frame
    int doneflag;                           // shot done = TRUE
    int delayflag;                          // DEALAYED ANIM START
    int startx;                             // PLAYER X OFFSET;
    int starty;                             // PLAYER Y OFFSET;
    SHOT_LIB *lib;
    int cnt;
}SHOTS;

extern char *lashit[4];

int SHOTS_PlayerShoot(int type);
void SHOTS_Clear(void);
void SHOTS_Think(void);
void SHOTS_Display(void);
void SHOTS_Init(void);
