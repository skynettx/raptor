#pragma once

#include "rap.h"
#include "gfxapi.h"

struct shot_lib_t {
    int f_0;
    texture_t *f_4[10];
    int f_2c;
    int f_30;
    int f_34;
    int f_38;
    int f_3c;
    int f_40;
    int f_44;
    int f_48;
    int f_4c;
    int f_50;
    int f_54;
    int f_58;
    int f_5c;
    int f_60;
    int f_64;
    int f_68;
    int f_6c;
    int f_70;
    texture_t *f_74;
    int f_78;
    int f_7c;
};

struct shot_t {
    shot_t* f_0;
    shot_t* f_4;
    texture_t *f_8;
    int f_c;
    int f_10;
    mobj_t f_14;
    int f_40;
    int f_44;
    int f_48;
    int f_4c;
    int f_50;
    int f_54;
    shot_lib_t *f_58;
    int f_5c;
};

extern texture_t *lashit[4];

int SHOTS_PlayerShoot(int a1);
void SHOTS_Clear(void);
void SHOTS_Think(void);
void SHOTS_Display(void);
void SHOTS_Init(void);
