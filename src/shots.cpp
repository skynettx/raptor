#include <string.h>
#include "common.h"
#include "shots.h"
#include "glbapi.h"
#include "fx.h"
#include "rap.h"
#include "enemy.h"
#include "anims.h"
#include "tile.h"
#include "eshot.h"
#include "fileids.h"

shot_t shots[70];

shot_t first_shots, last_shots;

shot_t *free_shots;

int shotnum;
int shothigh;

texture_t *detpow[4];
texture_t *laspow[4];
texture_t *lashit[4];

shot_lib_t shot_lib[15];

void SHOTS_Clear(void)
{
    int i;
    shotnum = 0;
    first_shots.f_0 = NULL;
    first_shots.f_4 = &last_shots;
    last_shots.f_0 = &first_shots;
    last_shots.f_4 = NULL;
    free_shots = shots;
    memset(shots, 0, sizeof(shots));

    for (i = 0; i < 70; i++)
    {
        if (i == 69) // FIXME
        {
            shots[i].f_4 = &first_shots;
            continue;
        }
        shots[i].f_4 = &shots[i + 1];
    }
}

shot_t *SHOTS_Get(void)
{
    shot_t *v1c;
    if (!free_shots)
        return NULL;
    shotnum++;
    if (shotnum > shothigh)
        shothigh = shotnum;
    v1c = free_shots;
    free_shots = free_shots->f_4;
    memset(v1c, 0, sizeof(shot_t));
    v1c->f_4 = &last_shots;
    v1c->f_0 = last_shots.f_0;
    last_shots.f_0 = v1c;
    v1c->f_0->f_4 = v1c;
    return v1c;
}

shot_t *SHOTS_Remove(shot_t *a1)
{
    shot_t *v1c;
    shotnum--;
    v1c = a1->f_0;
    a1->f_4->f_0 = a1->f_0;
    a1->f_0->f_4 = a1->f_4;
    memset(a1, 0, sizeof(shot_t));
    a1->f_4 = free_shots;
    free_shots = a1;
    return v1c;
}

void SHOTS_Init(void)
{
    int i, v24;
    shot_lib_t *v1c;
    SHOTS_Clear();
    for (i = 0; i < 4; i++)
    {
        detpow[i] = (texture_t*)GLB_LockItem(FILE139_DETHPOW_BLK + i);
    }
    for (i = 0; i < 4; i++)
    {
        laspow[i] = (texture_t*)GLB_LockItem(FILE13d_LASERPOW_BLK + i);
    }
    for (i = 0; i < 4; i++)
    {
        lashit[i] = (texture_t*)GLB_LockItem(FILE1f1_DRAYHIT_BLK + i);
    }
    memset(shot_lib, 0, sizeof(shot_lib));
    v1c = &shot_lib[0];
    v1c->f_0 = FILE1c3_NMSHOT_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 0;
    v1c->damageAmount = 1;
    v1c->startSpeed = 8;
    v1c->maxSpeedY = 0x10;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 4;
    v1c->offsetX = 0;
    v1c->offsetY = 2;
    v1c->y = 0;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 0;

    v1c = &shot_lib[1];
    v1c->f_0 = 65999;
    v1c->f_2c = 0;
    v1c->removeType = 1;
    v1c->damageAmount = 2;
    v1c->startSpeed = 4;
    v1c->maxSpeedY = 8;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 2;
    v1c->offsetX = 0;
    v1c->offsetY = 10;
    v1c->y = 0;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 1;

    v1c = &shot_lib[2];
    v1c->f_0 = FILE1c1_MICROM_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 2;
    v1c->damageAmount = 2;
    v1c->startSpeed = 2;
    v1c->maxSpeedY = 8;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 2;
    v1c->offsetX = 0;
    v1c->offsetY = 4;
    v1c->y = 0;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 3;

    v1c = &shot_lib[3];
    v1c->f_0 = FILE1ba_MISDUM_BLK;
    v1c->f_2c = 1;
    v1c->removeType = 3;
    v1c->damageAmount = 4;
    v1c->startSpeed = 2;
    v1c->maxSpeedY = 0xc;
    v1c->frameResetPoint = 1;
    v1c->numberOfFrames = 3;
    v1c->offsetX = 1;
    v1c->offsetY = 10;
    v1c->y = 0;
    v1c->f_58 = 1;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 0;

    v1c = &shot_lib[4];
    v1c->f_0 = FILE1c3_NMSHOT_BLK;
    v1c->f_2c = 1;
    v1c->removeType = 4;
    v1c->damageAmount = 1;
    v1c->startSpeed = 8;
    v1c->maxSpeedY = 10;
    v1c->frameResetPoint = 1;
    v1c->numberOfFrames = 4;
    v1c->offsetX = 0;
    v1c->offsetY = 1;
    v1c->y = 0;
    v1c->f_58 = 1;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 3;

    v1c = &shot_lib[5];
    v1c->f_0 = -1;
    v1c->f_2c = 0;
    v1c->removeType = 5;
    v1c->damageAmount = 5;
    v1c->startSpeed = 0;
    v1c->maxSpeedY = 0;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 0;
    v1c->offsetX = 0;
    v1c->offsetY = 6;
    v1c->y = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 0;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 1;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    // v1c->f_78 = v1c->f_74->f_c >> 1;
    // v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->damageType = 0;

    v1c = &shot_lib[6];
    v1c->f_0 = FILE1bd_MISRAT_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 6;
    v1c->damageAmount = 4;
    v1c->startSpeed = 1;
    v1c->maxSpeedY = 0x10;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 2;
    v1c->offsetX = 0;
    v1c->offsetY = 5;
    v1c->y = 0;
    v1c->hasAnimation = 1;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 1;

    v1c = &shot_lib[7];
    v1c->f_0 = FILE1bd_MISRAT_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 6;
    v1c->damageAmount = 4;
    v1c->startSpeed = 1;
    v1c->maxSpeedY = 0xc;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 2;
    v1c->offsetX = 0;
    v1c->offsetY = 10;
    v1c->y = 0;
    v1c->hasAnimation = 1;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 1;

    v1c = &shot_lib[8];
    v1c->f_0 = FILE1bf_MISGRD_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 8;
    v1c->damageAmount = 0x14;
    v1c->startSpeed = 1;
    v1c->maxSpeedY = 6;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 2;
    v1c->offsetX = 0;
    v1c->offsetY = 0x14;
    v1c->y = 0;
    v1c->hasAnimation = 1;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 2;

    v1c = &shot_lib[9];
    v1c->f_0 = FILE1d3_BLDGBOMB_PIC;
    v1c->f_2c = 0;
    v1c->removeType = 9;
    v1c->damageAmount = 0x32;
    v1c->startSpeed = 1;
    v1c->maxSpeedY = 4;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 1;
    v1c->offsetX = 0;
    v1c->offsetY = 0x1e;
    v1c->y = 0;
    v1c->hasAnimation = 0;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 4;

    v1c = &shot_lib[10];
    v1c->f_0 = FILE11d_POWDIS_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 10;
    v1c->damageAmount = 3;
    v1c->startSpeed = 4;
    v1c->maxSpeedY = 8;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 6;
    v1c->offsetX = 0;
    v1c->offsetY = 2;
    v1c->y = 0;
    v1c->hasAnimation = 0;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 5;

    v1c = &shot_lib[11];
    v1c->f_0 = FILE1cb_MEGABM_BLK;
    v1c->f_2c = 1;
    v1c->removeType = 0xb;
    v1c->damageAmount = 0x32;
    v1c->startSpeed = 2;
    v1c->maxSpeedY = 2;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 4;
    v1c->offsetX = 0;
    v1c->offsetY = 0x3c;
    v1c->y = 0;
    v1c->hasAnimation = 0;
    v1c->f_58 = 1;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 1;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 0;

    v1c = &shot_lib[12];
    v1c->f_0 = FILE123_SHOKWV_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 0xc;
    v1c->damageAmount = 5;
    v1c->startSpeed = 8;
    v1c->maxSpeedY = 8;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 2;
    v1c->offsetX = 0;
    v1c->offsetY = 3;
    v1c->y = 0;
    v1c->hasAnimation = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 1;
    v1c->offsetPlayerX = 0;
    v1c->offsetPlayerY = 0;
    v1c->skipDamage = 0;
    v1c->shotType = 0;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 0;

    v1c = &shot_lib[13];
    v1c->f_0 = FILE135_FRNTLAS_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 0xd;
    v1c->damageAmount = 10;
    v1c->startSpeed = 0;
    v1c->maxSpeedY = 0;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 4;
    v1c->offsetX = 0;
    v1c->offsetY = 7;
    v1c->y = 0;
    v1c->hasAnimation = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 0;
    v1c->offsetPlayerX = 1;
    v1c->offsetPlayerY = 1;
    v1c->skipDamage = 1;
    v1c->shotType = 2;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 1;

    v1c = &shot_lib[14];
    v1c->f_0 = FILE131_DETHRY_BLK;
    v1c->f_2c = 0;
    v1c->removeType = 0xe;
    v1c->damageAmount = 6;
    v1c->startSpeed = 0;
    v1c->maxSpeedY = 0;
    v1c->frameResetPoint = 0;
    v1c->numberOfFrames = 4;
    v1c->offsetX = 0;
    v1c->offsetY = 7;
    v1c->y = 0;
    v1c->hasAnimation = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 0;
    v1c->offsetPlayerX = 1;
    v1c->offsetPlayerY = 1;
    v1c->skipDamage = 1;
    v1c->shotType = 2;
    for (i = 0; i < v1c->numberOfFrames; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->TexturePtr = v1c->f_4[0];
    v1c->texWidthX2 = v1c->TexturePtr->width >> 1;
    v1c->texHeightX2 = v1c->TexturePtr->height >> 1;
    v1c->damageType = 3;
}

int SHOTS_PlayerShoot(int a1)
{
    shot_lib_t *v20;
    shot_t *v1c;
    enemy_t *v24;

    v20 = &shot_lib[a1];
    if (a1 == -1)
        EXIT_Error("SHOTS_PlayerShoot() type = EMPTY  ");
    if (v20->y)
        return 0;
    v20->y = v20->offsetY;
    v1c = SHOTS_Get();
    if (!v1c)
        return 0;
    switch (a1)
    {
    default:
        EXIT_Error("SHOTS_PlayerShoot() - Invalid Shot type");
        break;
    case 0:
        if (!fx_gus)
            SND_Patch(16, 127);
        g_flash = 7;
        v1c->currentFrame = (wrand() % v20->numberOfFrames);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;

        v1c->x = player_cx + o_gun1[playerpic];
        v1c->y = player_cy;
        
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, o_gun1[playerpic], 0);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->currentFrame = (wrand() % v20->numberOfFrames);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;

        v1c->x = player_cx - o_gun1[playerpic] - 1;
        v1c->y = player_cy;

        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, -o_gun1[playerpic] - 1, 0);
        break;
    case 1:
        if (!fx_gus)
            SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->currentFrame = wrand() % v20->numberOfFrames;
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 2:
        if (!fx_gus)
            SND_Patch(16, 127);
        g_flash = 7;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx + o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx - o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 3:
        if (!fx_gus)
            SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x + (wrand() % 16) + 10;
        v1c->mobj.y2 = v1c->y + 5;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->mobj);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x - (wrand() % 16) - 10;
        v1c->mobj.y2 = v1c->y + 5;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->mobj);
        break;
    case 4:
        v24 = ENEMY_GetRandom();
        if (!v24)
            SHOTS_Remove(v1c);
        else
        {
            if (!fx_gus)
                SND_Patch(16, 127);
            v1c->currentFrame = wrand() % v20->numberOfFrames;
            v1c->shotLib = &shot_lib[a1];
            v1c->f_4c = v20->offsetX;
            v1c->speedY = v20->startSpeed;
            v1c->x = player_cx;
            v1c->y = player_cy;
            v1c->mobj.x = v1c->x;
            v1c->mobj.y = v1c->y;
            v1c->mobj.x2 = (wrand() % v24->width) + v24->x - 1;
            v1c->mobj.y2 = (wrand() % v24->height) + v24->hly + v24->y - 1;
            v1c->f_50 = player_cx;
            v1c->f_54 = player_cy;
            InitMobj(&v1c->mobj);
        }
        break;
    case 5:
        v24 = ENEMY_GetRandomAir();
        if (!v24)
        {
            SHOTS_Remove(v1c);
            SND_Patch(34, 127);
        }
        else
        {
            SND_Patch(21, 127);
            v1c->shotLib = &shot_lib[a1];
            v24->hits -= v20->damageAmount;
            v1c->currentFrame = 0;
            v1c->f_4c = v20->offsetX;
            v1c->speedY = v20->startSpeed;
            v1c->x = player_cx;
            v1c->y = player_cy;
            v1c->mobj.x = (wrand() % v24->width) + v24->mobj.x - 1;
            v1c->mobj.y = (wrand() % v24->height) + v24->mobj.y - 1;
            v1c->mobj.x2 = player_cx;
            v1c->mobj.y2 = player_cy;
            v1c->f_50 = player_cx;
            v1c->f_54 = player_cy;
            InitMobj(&v1c->mobj);
            ANIMS_StartAnim(9, v1c->mobj.x, v1c->mobj.y);
        }
        break;
    case 6:
        SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx + o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, o_gun2[playerpic], 1);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx - o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, -o_gun2[playerpic] - 1, 1);
        break;
    case 7:
        SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx + o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx - o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 8:
        SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx + o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx - o_gun2[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 9:
        SND_Patch(19, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 10:
        SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx - 4;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 11:
        SND_Patch(16, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = 160;
        v1c->mobj.y2 = 75;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->mobj);
        break;
    case 12:
        SND_Patch(35, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx;
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 13:
        SND_Patch(18, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx + o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = -24;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx - o_gun3[playerpic];
        v1c->y = player_cy;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = -24;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 14:
        SND_Patch(18, 127);
        v1c->shotLib = &shot_lib[a1];
        v1c->f_4c = v20->offsetX;
        v1c->speedY = v20->startSpeed;
        v1c->x = player_cx;
        v1c->y = player_cy - 24;
        v1c->mobj.x = v1c->x;
        v1c->mobj.y = v1c->y;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c->mobj.x2 = v1c->x;
        v1c->mobj.y2 = -24;
        break;
    }
    return 1;
}


void SHOTS_Think(void)
{
    shot_lib_t *v1c;
    shot_t *v20;
    enemy_t *v24;
    int i;

    v1c = shot_lib;
    for (i = 0; i <= 14; i++, v1c++)
    {
        if (v1c->y > 0)
            v1c->y--;
    }

    for (v20 = first_shots.f_4; &last_shots != v20; v20 = v20->f_4)
    {
        v1c = v20->shotLib;
        switch (v1c->shotType)
        {
        default:
            EXIT_Error("SHOTS_Think()");
            break;
        case 0:
            v20->TexturePtr = v1c->f_4[v20->currentFrame];
            v20->x = v20->mobj.x - v1c->texWidthX2;
            if (v1c->f_5c)
                v20->y = v20->mobj.y - v1c->texHeightX2;
            else
                v20->y = v20->mobj.y;
            if (v1c->hasAnimation)
                ANIMS_StartAnim(11, v20->x + v1c->texWidthX2, v20->y + v1c->texHeightX2 * 2);
            break;
        case 1:
            v20->x = v20->mobj.x;
            v20->y = v20->mobj.y;
            break;
        case 2:
            v20->TexturePtr = v1c->f_4[v20->currentFrame];
            v20->x = v20->mobj.x - v1c->texWidthX2;
            v20->y = v20->mobj.y;
            for (v24 = first_enemy.next; &last_enemy != v24; v24 = v24->next)
            {
                if (v20->x > v24->x && v20->x < v24->x2 && v24->y < player_cy && v24->y > -30)
                {
                    v24->hits -= v1c->damageAmount;
                    if (v24->hits != -1)
                    {
                        v20->mobj.y2 = v24->y + v24->hly;
                        break;
                    }
                }
            }
            break;
        }
        if (v1c->offsetPlayerX)
            v20->x += player_cx - v20->f_50;
        if (v1c->offsetPlayerY)
            v20->y += player_cy - v20->f_54;
        //if (v20->f_10 + 16 < 0 || v20->f_c < 0 && v20->f_c > 320 && v20->f_10 > 200)
        if ((v20->y + 16 < 0) || (v20->x < 0) || (v20->x > 320) || (v20->y > 200))
        {
            if (v1c->f_5c)
            {
                v20->mobj.done = 1;
                goto LAB_00015d11;
            }
        }
        
        if (v20->f_4c == 0)
        {
            if (v20->speedY < v1c->maxSpeedY)
                v20->speedY++;
            v20->currentFrame++;
            if (v20->currentFrame >= v1c->numberOfFrames)
            {
                if (v1c->f_5c)
                    v20->currentFrame = v1c->frameResetPoint;
                else
                {
                    v20->mobj.done = 1;
                    goto LAB_00015d11;
                }
            }
        }
        if (v20->f_48)
        {
            v20->mobj.done = 1;
            goto LAB_00015d11;
        }
        if (v1c->skipDamage)
            goto LAB_00015d11;
        switch (v1c->damageType)
        {
        default:
            v24 = ENEMY_DamageEnergy(v20->x, v20->y, v1c->damageAmount);
            if (v24)
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(14, v20->x, v20->y);
                ANIMS_StartEAnim(v24, 19, v24->hlx, v24->hly);
            }
            break;
        case 3:
            if (ENEMY_DamageAll(v20->x, v20->y, v1c->damageAmount))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->x, v20->y);
                else
                    ANIMS_StartAnim(15, v20->x, v20->y);
            }
            break;
        case 0:
            if (ENEMY_DamageAll(v20->x, v20->y, v1c->damageAmount))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->x, v20->y);
                else
                    ANIMS_StartAnim(15, v20->x, v20->y);
            }
            else
            {
                if (TILE_IsHit(v1c->damageAmount, v20->x, v20->y))
                {
                    v20->mobj.done = 1;
                }
            }
            break;
        case 1:
            if (ENEMY_DamageAir(v20->x, v20->y, v1c->damageAmount))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->x, v20->y);
                else
                    ANIMS_StartAnim(15, v20->x, v20->y);
            }
            break;
        case 2:
            if (ENEMY_DamageGround(v20->x, v20->y, v1c->damageAmount))
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(15, v20->x, v20->y);
            }
            else
            {
                if (TILE_IsHit(v1c->damageAmount, v20->x, v20->y))
                {
                    v20->mobj.done = 1;
                }
            }
            break;
        case 4:
            if (TILE_Bomb(v1c->damageAmount, v20->x, v20->y))
            {
                v20->mobj.done = 1;
            }
            if (ENEMY_DamageGround(v20->x, v20->y, 5))
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(1, v20->x, v20->y);
            }
            break;
        }
    LAB_00015d11:
        if (v20->mobj.done)
        {
            if (v20->f_4c)
            {
                v20->f_4c = 0;
                v20->mobj.x2 = v20->mobj.x + ((wrand() % 32) - 16);
                v20->mobj.y2 = 0;
                ANIMS_StartAnim(11, v20->mobj.x, v20->mobj.y);
                InitMobj(&v20->mobj);
            }
            else
            {
                switch (v1c->removeType)
                {
                case 11:
                    ESHOT_Clear();
                    TILE_DamageAll();
                    for (v24 = first_enemy.next; &last_enemy != v24; v24 = v24->next)
                    {
                        v24->hits -= v1c->damageAmount;
                    }
                    startfadeflag = 1;
                    ANIMS_StartAnim(20, 0, 0);
                    v20 = SHOTS_Remove(v20);
                    continue;
                case 5:
                    break;
                default:
                    v20 = SHOTS_Remove(v20);
                    continue;
                }
            }
        }
        if (v1c->f_5c)
        {
            if (v1c->f_58)
            {
                MoveSobj(&v20->mobj, v20->speedY);
            }
            else
            {
                v20->mobj.y -= v20->speedY;
                if (v20->mobj.y < 0)
                {
                    v20->mobj.done = 1;
                    v20->f_48 = 1;
                }
            }
        }
    }
}

void SHOTS_Display(void)
{
    int v20, v24, v28;
    shot_t *v1c;
    texture_t * v2c;
    for (v1c = first_shots.f_4; v1c != &last_shots; v1c = v1c->f_4)
    {
        switch (v1c->shotLib->shotType)
        {
        default:
            EXIT_Error("SHOTS_Display()");
            break;
        case 0:
            GFX_PutSprite(v1c->TexturePtr, v1c->x, v1c->y);
            break;
        case 1:
            GFX_Line(player_cx + 1, player_cy, v1c->mobj.x, v1c->mobj.y, 69);
            GFX_Line(player_cx - 1, player_cy, v1c->mobj.x, v1c->mobj.y, 69);
            GFX_Line(player_cx, player_cy, v1c->mobj.x, v1c->mobj.y, 64);
            v1c = SHOTS_Remove(v1c);
            break;
        case 2:
            for (v20 = v1c->mobj.y2; v20 < v1c->y; v20 += 3)
            {
                GFX_PutSprite(v1c->TexturePtr, v1c->x, v20);
            }
            if (v1c->shotLib->removeType == 14)
                GFX_PutSprite(detpow[v1c->f_5c], v1c->x - 4, v1c->y);
            else
                GFX_PutSprite(laspow[v1c->f_5c], v1c->x, v1c->y);
            v2c = lashit[v1c->f_5c];
            v24 = v1c->x - (v2c->width >> 2);
            v28 = v1c->mobj.y2 - 8;
            if (v28 > 0)
                GFX_PutSprite(v2c, v24, v28);
            v1c->f_5c++;
            v1c->f_5c = v1c->f_5c % 4;
            break;
        }
    }
}
