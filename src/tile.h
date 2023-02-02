#pragma once

extern char *tilepic;
extern char *tilestart;
extern int tileloopy;
extern int tileyoff;
extern int tilepos;
extern int scroll_flag;

struct tspot_t {
    int item;
    int x;
    int y;
    int mapspot;
};

struct tdel_t {
    tdel_t *prev;
    tdel_t *next;
    int frames;
    int mapspot;
    int item;
    tspot_t *ts;
};

void TILE_Init(void);
void TILE_Think(void);
void TILE_Display(void);
int TILE_IsHit(int damage, int x, int y);
int TILE_Bomb(int damage, int x, int y);
void TILE_Explode(tspot_t* ts, int delay);
void TILE_DamageAll(void);
void TILE_CacheLevel(void);
void TILE_FreeLevel(void);

void TILE_Draw(void);
void TILE_ClipDraw(void);
void TILE_ShakeScreen(void);
void TILE_DisplayScreen(void);

