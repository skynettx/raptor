#pragma once
#include "rap.h"
#include "enemy.h"
#include "gfxapi.h"

struct plib_t {
    int f_0;
    texture_t *f_4[10];
    int f_2c;
    int f_30;
    int f_34;
    int f_38;
    int f_3c;
    int f_40;
};

struct eshot_t {
    eshot_t *f_0;
    eshot_t *f_4;
    texture_t *f_8;
    int f_c;
    int f_10;
    int f_14;
    mobj_t f_18;
    int f_44;
    plib_t *f_48;
    int f_4c;
    int f_50;
    int f_54;
    int f_58;
    enemy_t *f_5c;
    int f_60;
};

void ESHOT_Shoot(enemy_t *a1, int a2);
void ESHOT_Clear(void);
void ESHOT_Think(void);
void ESHOT_Display(void);
void ESHOT_Init(void);
