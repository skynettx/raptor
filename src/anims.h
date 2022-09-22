#pragma once

#include "enemy.h"

enum ANIM_DIR
{
    A_NORM,
    A_MOVEUP,
    A_MOVEDOWN
};

enum GFLAG {
    GROUND,
    MID_AIR,
    HIGH_AIR
};

struct animlib_t {
    int item;
    int numframes;
    int groundflag;
    int playerflag;
    int transparent;
    int adir;
    int xoff;
    int yoff;
};

struct anim_t {
    anim_t *prev;
    anim_t *next;
    int item;
    int dx;
    int dy;
    int x;
    int y;
    int curframe;
    animlib_t *lib;
    int groundflag;
    enemy_t *en;
    int edone;
};

#define A_LARGE_GROUND_EXPLO1  0
#define A_SMALL_GROUND_EXPLO   1
#define A_PERSON               2
#define A_PLATOON              3

#define A_LARGE_AIR_EXPLO      4
#define A_MED_AIR_EXPLO        5
#define A_SMALL_AIR_EXPLO      6
#define A_MED_AIR_EXPLO2       7
#define A_ENERGY_AIR_EXPLO     8
#define A_LASER_BLAST          9

#define A_SMALL_SMOKE          10
#define A_SMALL_SMOKE_DOWN     11
#define A_SMALL_SMOKE_UP       12
#define A_ENERGY_GRAB_HIT      13
#define A_BLUE_SPARK           14
#define A_ORANGE_SPARK         15
#define A_PLAYER_SHOOT         16
#define A_GROUND_FLARE         17
#define A_GROUND_SPARKLE       18
#define A_ENERGY_GRAB          19
#define A_SUPER_SHIELD         20

void ANIMS_Clear(void);
anim_t *ANIMS_Remove(anim_t *anim);
void ANIMS_Init(void);
void ANIMS_FreePics(void);
void ANIMS_StartAnim(int handle, int x, int y);
void ANIMS_StartGAnim(int handle, int x, int y);
void ANIMS_StartEAnim(enemy_t* en, int handle, int x, int y);
void ANIMS_StartAAnim(int handle, int x, int y);
void ANIMS_Think(void);
void ANIMS_DisplayGround(void);
void ANIMS_DisplaySky(void);
void ANIMS_DisplayHigh(void);
