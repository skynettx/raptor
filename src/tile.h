#pragma once

extern char *tilepic;
extern char *tilestart;
extern int tileloopy;
extern int tileyoff;
extern int tilepos;
extern int scroll_flag;

typedef struct 
{
    int item;
    int x;
    int y;
    int mapspot;
}TILESPOT;

typedef struct TDELAY_S
{
    struct TDELAY_S *prev;
    struct TDELAY_S *next;
    int frames;
    int mapspot;
    int item;
    TILESPOT *ts;
}TILEDELAY;

void TILE_Init(void);
void TILE_Think(void);
void TILE_Display(void);
int TILE_IsHit(int damage, int x, int y);
int TILE_Bomb(int damage, int x, int y);
void TILE_Explode(TILESPOT *ts, int delay);
void TILE_DamageAll(void);
void TILE_CacheLevel(void);
void TILE_FreeLevel(void);

void TILE_Draw(void);
void TILE_ClipDraw(void);
void TILE_ShakeScreen(void);
void TILE_DisplayScreen(void);

