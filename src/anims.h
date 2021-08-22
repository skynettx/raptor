#pragma once

#include "enemy.h"

struct animlib_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
};

struct anim_t {
    anim_t *f_0;
    anim_t *f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    animlib_t *f_20;
    int f_24;
    enemy_t *f_28;
    int f_2c;
};

void ANIMS_Clear(void);
anim_t *ANIMS_Remove(anim_t *a1);
void ANIMS_Init(void);
void ANIMS_FreePics(void);
void ANIMS_StartAnim(int a1, int a2, int a3);
void ANIMS_StartGAnim(int a1, int a2, int a3);
void ANIMS_StartEAnim(enemy_t* a1, int a2, int a3, int a4);
void ANIMS_StartAAnim(int a1, int a2, int a3);
void ANIMS_Think(void);
void ANIMS_DisplayGround(void);
void ANIMS_DisplaySky(void);
void ANIMS_DisplayHigh(void);
