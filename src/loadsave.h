#pragma once

#include "rap.h"

struct csprite_t {
    int link;
    int slib;
    int x;
    int y;
    int game;
    int level;
};

struct maptile_t {
    short flats;
    short fgame;
};

typedef struct _map_t {
    int sizerec;
    int spriteoff;
    int numsprites;
    maptile_t map[MAP_SIZE];
} map_t;

extern int curplr_diff;

extern map_t *mapmem;
extern csprite_t *csprite;

const char* RAP_InitLoadSave(void);
const char* RAP_SetupFilename(void);

void RAP_ClearPlayer(void);
int RAP_IsSaveFile(player_t *in_plr);
int RAP_FFSaveFile(void);
void RAP_SetPlayerDiff(void);
int RAP_SavePlayer(void);
