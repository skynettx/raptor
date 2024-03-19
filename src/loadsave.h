#pragma once
#include <boost/endian.hpp>
#include "rap.h"

using namespace boost::endian;

typedef struct
{
    little_int32_t link;
    little_int32_t slib;
    little_int32_t x;
    little_int32_t y;
    little_int32_t game;
    little_int32_t level;
}CSPRITE;

typedef struct
{
    little_int16_t flats;
    little_int16_t fgame;
}MAZEDATA;

typedef struct
{
    little_int32_t sizerec;
    little_int32_t spriteoff;
    little_int32_t numsprites;
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
