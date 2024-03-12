#pragma once

#include "rap.h"

typedef struct 
{
    int link;
    int slib;
    int x;
    int y;
    int game;
    int level;
}CSPRITE;

typedef struct 
{
    short flats;
    short fgame;
}MAZEDATA;

typedef struct 
{
    int sizerec;
    int spriteoff;
    int numsprites;
    MAZEDATA map[MAP_SIZE];
}MAZELEVEL;

extern int curplr_diff;

extern MAZELEVEL *mapmem;
extern CSPRITE *csprite;

const char* RAP_InitLoadSave(void);
const char* RAP_SetupFilename(void);

void RAP_ClearPlayer(void);
int RAP_IsSaveFile(PLAYEROBJ *in_plr);
int RAP_FFSaveFile(void);
void RAP_SetPlayerDiff(void);
int RAP_SavePlayer(void);
