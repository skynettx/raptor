#pragma once

#include "objects.h"

struct bonus_t {
    bonus_t *f_0;
    bonus_t *f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    int f_28;
    int f_2c;
    int f_30;
    int f_34;
    int f_38;
    objlib_t *f_3c;
};

void BONUS_Clear(void);
void BONUS_Think(void);
void BONUS_Display(void);
void BONUS_Init(void);
void BONUS_Add(int a1, int a2, int a3);
