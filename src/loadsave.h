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
    short f_0;
    short f_2;
};

typedef struct _map_t {
    int f_0;
    int f_4;
    int numsprites;
    maptile_t f_c[1350];
} map_t;

extern int curplr_diff;

extern map_t *mapmem;
extern csprite_t *csprite;

const char* RAP_DataPath(void);
const char* RAP_GetSetupPath(void);

void RAP_ClearPlayer(void);
int RAP_IsSaveFile(player_t *a1);
int RAP_FFSaveFile(void);
void RAP_SetPlayerDiff(void);
int RAP_SavePlayer(void);
