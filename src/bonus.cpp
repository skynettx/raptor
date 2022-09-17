#include <string.h>
#include <stdlib.h>
#include "bonus.h"
#include "rap.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "objects.h"
#include "fx.h"
#include "fileids.h"

bonus_t bons[12];
bonus_t first_bonus, last_bonus;
bonus_t *free_bonus;
static int energy_count;
static int glow[4];
static int glow_lx, glow_ly;
static int xpos[16] = {
    -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2, -3, -3, -3, -2
};

static int ypos[16] = {
    -3, -3, -3, -2, -1, 0, 1, 2, 3, 3, 3, 2, 1, 0, -1, -2
};

void BONUS_Clear(void)
{
    int v1c;
    energy_count = 0;
    first_bonus.f_0 = NULL;
    first_bonus.f_4 = &last_bonus;
    last_bonus.f_0 = &first_bonus;
    last_bonus.f_4 = NULL;
    free_bonus = bons;
    memset(bons, 0, sizeof(bons));
    for (v1c = 0; v1c < 11; v1c++)
        bons[v1c].f_4 = &bons[v1c + 1];
}


bonus_t *BONUS_Get(void)
{
    bonus_t *v1c;
    if (!free_bonus)
        return NULL;
    v1c = free_bonus;
    free_bonus = free_bonus->f_4;
    memset(v1c, 0, sizeof(bonus_t));
    v1c->f_4 = &last_bonus;
    v1c->f_0 = last_bonus.f_0;
    last_bonus.f_0 = v1c;
    v1c->f_0->f_4 = v1c;
    return v1c;
}

bonus_t *BONUS_Remove(bonus_t *a1)
{
    bonus_t *v1c;
    if (a1->f_38 == 23)
        energy_count--;
    v1c = a1->f_0;
    a1->f_4->f_0 = a1->f_0;
    a1->f_0->f_4 = a1->f_4;
    memset(a1, 0, sizeof(bonus_t));
    a1->f_4 = free_bonus;
    free_bonus = a1;
    return v1c;
}

void BONUS_Init(void)
{
    int v1c;
    texture_t *v20;

    for (v1c = 0; v1c < 4; v1c++)
    {
        glow[v1c] = FILE125_ICNGLW_BLK + v1c;
    }
    v20 = (texture_t*)GLB_CacheItem(FILE125_ICNGLW_BLK);
    glow_lx = v20->width;
    glow_ly = v20->height;
    GLB_CacheItem(FILE126_ICNGLW_BLK);
    GLB_CacheItem(FILE127_ICNGLW_BLK);
    GLB_CacheItem(FILE128_ICNGLW_BLK);
    BONUS_Clear();
}

void BONUS_Add(int a1, int a2, int a3)
{
    bonus_t *v14;
    if (a1 >= 24)
        return;
    if (a1 == 23 && energy_count > 9)
        return;
    v14 = BONUS_Get();
    if (!v14)
        return;
    if (a1 == 23)
        energy_count++;

    v14->f_38 = a1;
    v14->f_3c = OBJS_GetLib(a1);
    v14->f_c = 0;
    v14->f_14 = 16 + a2;
    v14->f_18 = a3;
    v14->f_2c = wrand() % 16;
}

void BONUS_Think(void)
{
    int v20, v24, v28, v2c;
    bonus_t *v1c;
    static int gcnt;

    v20 = playerx;
    v24 = playery;
    v28 = playerx + 32;
    v2c = playery + 32;
    for (v1c = first_bonus.f_4; &last_bonus != v1c; v1c = v1c->f_4)
    {
        v1c->f_8 = v1c->f_3c->f_0 + v1c->f_c;
        v1c->f_1c = v1c->f_14 - 8 + xpos[v1c->f_2c];
        v1c->f_20 = v1c->f_18 - 8 + ypos[v1c->f_2c];
        v1c->f_24 = v1c->f_14 - (glow_lx>>1) + xpos[v1c->f_2c];
        v1c->f_28 = v1c->f_18 - (glow_ly>>1) + ypos[v1c->f_2c];
        v1c->f_18++;
        if (gcnt & 1)
        {
            v1c->f_2c++;
            if (v1c->f_2c >= 16)
                v1c->f_2c = 0;
            v1c->f_c++;
            if (v1c->f_c >= v1c->f_3c->f_4)
                v1c->f_c = 0;
        }
        v1c->f_10++;
        if (v1c->f_10 >= 4)
            v1c->f_10 = 0;
        if (v1c->f_14 > v20 && v1c->f_14 < v28 && v1c->f_18 > v24 && v1c->f_18 < v2c)
        {
            if (!v1c->f_30 && OBJS_GetAmt(16) > 0)
            {
                SND_Patch(10, 127);
                if (v1c->f_38 == 16)
                    OBJS_AddEnergy(25);
                else
                    OBJS_Add(v1c->f_38);
                if (v1c->f_3c->pays)
                {
                    v1c->f_30 = 1;
                    v1c->f_34 = 50;
                }
                else
                {
                    v1c = BONUS_Remove(v1c);
                    continue;
                }
            }
        }
        if (v1c->f_30)
        {
            v1c->f_34--;
            if (v1c->f_34 <= 0)
            {
                v1c = BONUS_Remove(v1c);
                continue;
            }
        }
        if (v1c->f_28 > 200)
            v1c = BONUS_Remove(v1c);
    }
    gcnt++;
}

void BONUS_Display(void)
{
    bonus_t *v1c;
    for (v1c = first_bonus.f_4; &last_bonus != v1c; v1c = v1c->f_4)
    {
        if (!v1c->f_30)
        {
            GFX_PutSprite((texture_t*)GLB_GetItem(v1c->f_8), v1c->f_1c, v1c->f_20);
            GFX_ShadeShape(1, (texture_t*)GLB_GetItem(glow[v1c->f_10]), v1c->f_24, v1c->f_28);
        }
        else
            GFX_PutSprite((texture_t*)GLB_GetItem(FILE10f_N$_PIC), v1c->f_1c, v1c->f_20);
    }
}

