#pragma once

#include "rap.h"
#include "gfxapi.h"

struct shot_lib_t {
    int f_0;
    texture_t *f_4[10];
    int f_2c;
    int removeType;
    int damageAmount;
    int startSpeed;
    int maxSpeedY;
    int frameResetPoint;
    int numberOfFrames;
    int offsetY;
    int y;
    int offsetX;
    int hasAnimation;
    int f_58;
    int f_5c;
    int damageType;
    int offsetPlayerX;
    int offsetPlayerY;
    int skipDamage;
    int shotType;
    texture_t *TexturePtr;
    int texWidthX2;
    int texHeightX2;
};

struct shot_t {
    shot_t* f_0;
    shot_t* f_4;
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
