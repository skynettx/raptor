#pragma once

extern char *tilepic;
extern char *tilestart;
extern int tileloopy;
extern int tileyoff;
extern int tilepos;
extern int scroll_flag;

struct tspot_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
};

struct tdel_t {
    tdel_t *f_0;
    tdel_t *f_4;
    int f_8;
    int f_c;
    int f_10;
    tspot_t *f_14;
};

void TILE_Init(void);
void TILE_Think(void);
void TILE_Display(void);
int TILE_IsHit(int a1, int a2, int a3);
int TILE_Bomb(int a1, int a2, int a3);
void TILE_DamageAll(void);
void TILE_CacheLevel(void);
void TILE_FreeLevel(void);

void TILE_Draw(void);
void TILE_ClipDraw(void);
void TILE_ShakeScreen(void);
void TILE_DisplayScreen(void);

