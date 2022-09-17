#pragma once
#include "rap.h"

struct slib_t {
    char f_0[16];
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
    int f_74;
    short f_78[24];
    short f_a8[24];
    short f_d8[24];
    short f_108[24];
    short f_138[24];
    short f_168[24];
    short f_198[30];
    short f_1d4[30];
}; // size 0x210

struct enemy_t {
    enemy_t *f_0;
    enemy_t *f_4;
    int item;
    slib_t *f_c;
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
    int f_3c;
    int f_40;
    int f_44;

    int f_50;
    int f_54;
    int f_58;
    mobj_t mobj;
    int f_88;
    int f_8c;
    int f_90;
    int f_94;
    int f_98;
    int f_9c;

    int f_a0;
    int f_a4;
    int f_a8;
    int f_ac;
    int f_b0;
    int f_b4;
    int f_b8;
};

extern enemy_t first_enemy, last_enemy;
void ENEMY_Clear(void);
void ENEMY_LoadLib(void);
void ENEMY_LoadSprites(void);
void ENEMY_FreeSprites(void);
enemy_t *ENEMY_GetRandom(void);
enemy_t* ENEMY_GetRandomAir(void);
enemy_t *ENEMY_DamageEnergy(int a1, int a2, int a3);
int ENEMY_DamageAll(int a1, int a2, int a3);
int ENEMY_DamageGround(int a1, int a2, int a3);
int ENEMY_DamageAir(int a1, int a2, int a3);
int ENEMY_GetBaseDamage(void);
void ENEMY_Think(void);
void ENEMY_DisplayGround(void);
void ENEMY_DisplaySky(void);
