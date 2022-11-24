#pragma once

#include "rap.h"
#include "gfxapi.h"

#define MAX_SHOTS 70

enum HIT_TYPE
{
    S_ALL,
    S_AIR,
    S_GROUND,
    S_GRALL,
    S_GTILE,
    S_SUCK
};

enum BEAM_TYPE
{
    S_SHOOT,
    S_LINE,
    S_BEAM
};

struct shot_lib_t {
    int lumpnum;                            // LUMPNUM OF PIC(S)
    texture_t *pic[10];                     // POINTER TO PIC
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
    texture_t *h;
    int hlx;
    int hly;
};

struct shot_t {
    shot_t* prev;                           // Link List Prev
    shot_t* next;                           // Link List Next
    texture_t *TexturePtr;
    int x;
    int y;
    mobj_t mobj;
    int speedY;
    int currentFrame;
    int f_48;
    int f_4c;
    int f_50;
    int f_54;
    shot_lib_t *shotLib;
    int f_5c;
};

extern texture_t *lashit[4];

int SHOTS_PlayerShoot(int a1);
void SHOTS_Clear(void);
void SHOTS_Think(void);
void SHOTS_Display(void);
void SHOTS_Init(void);
