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
