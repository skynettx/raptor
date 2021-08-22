#include <stdio.h>
#include <string.h>
#include "common.h"
#include "enemy.h"
#include "gfxapi.h"
#include "rap.h"
#include "glbapi.h"
#include "tile.h"
#include "loadsave.h"
#include "fx.h"
#include "eshot.h"
#include "shadow.h"
#include "anims.h"
#include "objects.h"
#include "bonus.h"
#include "flame.h"

int spriteflag[4];
int spriteitm[4] = {
    0x100fc, 0x20000, 0x30000, 0x40000
};

slib_t *slib[4];

int cur_visable;
int boss_sound;
int g_numslibs;
int numslibs[4];

int numboss, numships;
int end_waveflag;

enemy_t ships[30];
enemy_t last_enemy, first_enemy;
enemy_t *free_enemy;

csprite_t *end_enemy, *cur_enemy;

int tiley;

enemy_t *onscreen[30], *rscreen[30];


int MoveEobj(mobj_t *a1, int a2)
{
    if (!a2)
        return 0;
    if (a1->f_10 >= a1->f_14)
    {
        while (a2)
        {
            a2--;
            a1->f_20--;
            if (a1->f_20 == 0)
            {
                a1->f_28 = 1;
                return a2;
            }
            a1->f_0 += a1->f_18;
            a1->f_24 += a1->f_14;
            if (a1->f_24 > 0)
            {
                a1->f_4 += a1->f_1c;
                a1->f_24 -= a1->f_10;
            }
        }
    }
    else
    {
        while (a2)
        {
            a2--;
            a1->f_20--;
            if (a1->f_20 == 0)
            {
                a1->f_28 = 1;
                return a2;
            }
            a1->f_4 += a1->f_1c;
            a1->f_24 += a1->f_10;
            if (a1->f_24 > 0)
            {
                a1->f_0 += a1->f_18;
                a1->f_24 -= a1->f_14;
            }
        }
    }
    if (a1->f_20 < 1)
        a1->f_28 = 1;
    return a2;
}

void ENEMY_FreeSprites(void)
{
    int v1c, v20;
    csprite_t *v28;
    slib_t *v24;

    for (v1c = 0; v1c < 4; v1c++)
    {
        if (spriteflag[v1c])
            GLB_FreeItem(spriteitm[v1c]);
    }
    for (v1c = 0; v1c < mapmem->f_8; v1c++)
    {
        v28 = &csprite[v1c];
        v24 = &slib[csprite[v1c].f_10][csprite[v1c].f_4];
        if (cur_diff & v28->f_14)
        {
            for (v20 = 0; v20 < v24->f_2c; v20++)
            {
                GLB_FreeItem(v24->f_10 + v20);
            }
        }
    }
}

void ENEMY_LoadSprites(void)
{
    int v1c, v20, v2c;
    csprite_t *v28;
    slib_t *v24;
    ENEMY_Clear();
    cur_visable = 0;
    boss_sound = 0;
    for (v1c = 0; v1c < mapmem->f_8; v1c++)
    {
        v28 = &csprite[v1c];
        v24 = &slib[csprite[v1c].f_10][csprite[v1c].f_4];
        v24->f_10 = GLB_GetItemID(v24->f_0);
        switch (v28->f_14)
        {
        default:
            v28->f_14 = 64;
            break;
        case 0:
            v28->f_14 = 1;
            break;
        case 1:
            v28->f_14 = 2;
            break;
        case 2:
            v28->f_14 = 4;
            break;
        case 3:
            v28->f_14 = 8;
            break;
        case 4:
            v28->f_14 = 16;
            break;
        case 5:
            v28->f_14 = 32;
            break;
        }
        if (cur_diff & v28->f_14)
        {
            if (v24->f_10 != -1)
            {
                for (v20 = 0; v20 < v24->f_2c; v20++)
                {
                    v2c = v24->f_10 + v20;
                    GLB_CacheItem(v2c);
                }
            }
            else
            {
                v28->f_14 = 64;
            }
        }
        else
            v28->f_14 = 64;
    }
}

void ENEMY_LoadLib(void)
{
    int v1c;
    memset(spriteflag, 0, sizeof(spriteflag));
    for (v1c = 0; v1c < mapmem->f_8; v1c++)
    {
        spriteflag[csprite[v1c].f_10] = 1;
    }
    g_numslibs = 0;
    for (v1c = 0; v1c < 4; v1c++)
    {
        slib[v1c] = NULL;
        numslibs[v1c] = 0;
        if (spriteflag[v1c])
            g_numslibs++;
    }
    if (g_numslibs > 1 && !gameflag[2] && !gameflag[3])
        EXIT_Error("ENEMY_LoadSprites() - F:%d  G1:%d G2:%d G3:%d G4:%d", g_numslibs, spriteflag[0], spriteflag[1], spriteflag[2], spriteflag[3]);
    for (v1c = 0; v1c < 4; v1c++)
    {
        if (spriteflag[v1c])
        {
            slib[v1c] = (slib_t*)GLB_LockItem(spriteitm[v1c]);
            if (!slib[v1c])
                EXIT_Error("ENEMY_LoadSprites() - memory");
            numslibs[v1c] = GLB_ItemSize(spriteitm[v1c]);
            numslibs[v1c] /= sizeof(slib_t);
        }
    }
}

void ENEMY_Clear(void)
{
    int v1c;
    numboss = 0;
    numships = 0;
    end_waveflag = 0;
    first_enemy.f_0 = NULL;
    first_enemy.f_4 = &last_enemy;
    last_enemy.f_0 = &first_enemy;
    last_enemy.f_4 = NULL;
    free_enemy = ships;
    memset(ships, 0, sizeof(ships));
    for (v1c = 0; v1c < 29; v1c++)
    {
        ships[v1c].f_4 = &ships[v1c + 1];
    }
    if (mapmem->f_8)
    {
        end_enemy = csprite + mapmem->f_8 - 1;
        cur_enemy = csprite;
    }
    else
    {
        end_enemy = NULL;
        cur_enemy = NULL;
    }
}

enemy_t *ENEMY_Get(void)
{
    enemy_t *v1c;
    if (!free_enemy)
        EXIT_Error("ENEMY_Get() - Max Sprites");
    numships++;
    v1c = free_enemy;
    free_enemy = free_enemy->f_4;
    memset(v1c, 0, sizeof(enemy_t));
    v1c->f_4 = &last_enemy;
    v1c->f_0 = last_enemy.f_0;
    last_enemy.f_0 = v1c;
    v1c->f_0->f_4 = v1c;
    return v1c;
}

enemy_t *ENEMY_Remove(enemy_t *a1)
{
    enemy_t *v1c;
    if (a1->f_c->f_40)
        numboss--;
    numships--;
    if (end_waveflag && numships < 1)
        startendwave = 60;
    v1c = a1->f_0;
    a1->f_4->f_0 = a1->f_0;
    a1->f_0->f_4 = a1->f_4;
    memset(a1, 0, sizeof(enemy_t));
    a1->f_8 = -1;
    a1->f_4 = free_enemy;
    free_enemy = a1;
    return v1c;
}

void ENEMY_Add(csprite_t *a1)
{
    slib_t *v1c;
    enemy_t *v20;
    char *v28;
    texture_t *v24;
    v1c = &slib[a1->f_10][a1->f_4];
    v20 = ENEMY_Get();
    v28 = GLB_GetItem(v1c->f_10);
    v24 = (texture_t*)v28;
    v20->f_8 = v1c->f_10;
    v20->f_28 = v24->f_c;
    v20->f_2c = v24->f_10;
    v20->f_30 = v24->f_c >> 1;
    v20->f_34 = v24->f_10 >> 1;
    v20->f_a0 = 0;
    v20->f_50 = v1c->f_44;
    v20->f_c = &slib[a1->f_10][a1->f_4];
    v20->f_1c = tileyoff - (tiley - a1->f_c) * 32 - 97;
    v20->f_18 = a1->f_8 * 32 + 16;
    v20->f_9c = 0;
    v20->f_18 += 16;
    v20->f_1c += 16;
    v20->f_18 -= v20->f_30;
    v20->f_1c -= v20->f_34;
    v20->f_20 = v20->f_18 + v20->f_28;
    v20->f_24 = v20->f_1c + v20->f_2c;
    v20->f_5c.f_0 = v20->f_10 = v20->f_18;
    v20->f_5c.f_4 = v20->f_14 = v20->f_1c;
    v20->f_a4 = v1c->f_28;
    v20->f_b4 = v1c->f_58;
    v20->f_88 = v1c->f_30 - v20->f_5c.f_4;
    v20->f_ac = 0;
    v20->f_a8 = 0;
    v20->f_3c = -1;
    v20->f_40 = v1c->f_50;
    v20->f_44 = v1c->f_4c;
    if (v1c->f_40 && curplr_diff <= 1)
    {
        v20->f_50 -= v20->f_50 >> 1;
        v20->f_40 -= v20->f_40 >> 2;
    }
    switch (v1c->f_38)
    {
    default:
        EXIT_Error("ENEMY_Add() - Invalid ANIMTYPE");
        break;
    case 0:
        v20->f_98 = 1;
        v20->f_94 = v1c->f_2c;
        break;
    case 1:
        v20->f_98 = 0;
        v20->f_94 = v1c->f_2c;
        break;
    case 2:
        v20->f_98 = 1;
        v20->f_94 = v1c->f_34;
        break;
    }
    switch (v1c->f_64)
    {
    case 0:
    case 1:
    case 2:
        v20->f_54 = 0;
        v20->f_14 = 100 - v20->f_34;
        v20->f_5c.f_8 = v20->f_10 + v1c->f_198[0];
        v20->f_5c.f_c = v20->f_14 + v1c->f_1d4[0];
        v20->f_38 = 1;
        InitMobj(&v20->f_5c);
        MoveMobj(&v20->f_5c);
        break;
    case 3:
        v20->f_54 = 1;
        v20->f_5c.f_8 = v20->f_18;
        v20->f_5c.f_c = 211;
        break;
    case 5:
        v20->f_18 = v20->f_28;
        v20->f_10 = v20->f_18;
        v20->f_5c.f_0 = v20->f_10;
        v20->f_54 = 1;
        v20->f_5c.f_8 = 335;
        v20->f_5c.f_c = 211;
        break;
    case 4:
        v20->f_18 = v20->f_28;
        v20->f_10 = v20->f_18;
        v20->f_5c.f_0 = v20->f_10;
        v20->f_54 = 1;
        v20->f_5c.f_8 = -v20->f_30;
        v20->f_5c.f_c = 211;
        break;
    }
    v20->f_b8 = v1c->f_44 >> 4;
    if (v1c->f_74 != -1)
        boss_sound = 1;
}

enemy_t *ENEMY_GetRandom(void)
{
    int v1c;
    if (!cur_visable)
        return NULL;
    v1c = wrand() % cur_visable;
    return onscreen[v1c];
}

enemy_t *ENEMY_GetRandomAir(void)
{
    int v1c;
    int v20;
    if (!cur_visable)
        return NULL;
    v1c = 0;
    for (v20 = 0; v20 < cur_visable; v20++)
    {
        if (onscreen[v20]->f_54)
            continue;
        rscreen[v1c] = onscreen[v20];
        v1c++;
    }
    if (v1c > 0)
    {
        v1c = wrand() % v1c;
        return rscreen[v1c];
    }
    return NULL;
}

int ENEMY_DamageAll(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (a1 > v18->f_18&& a1 < v18->f_20 && a2 > v18->f_1c&& a2 < v18->f_24)
        {
            v18->f_50 -= a3;
            return 1;
        }
    }
    return 0;
}

int ENEMY_DamageGround(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (!v18->f_54)
            continue;
        if (a1 > v18->f_18 && a1 < v18->f_20 && a2 > v18->f_1c&& a2 < v18->f_24)
        {
            v18->f_50 -= a3;
            if (curplr_diff == 0)
                v18->f_50 -= a3;
            return 1;
        }
    }
    return 0;
}

int ENEMY_DamageAir(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (v18->f_54)
            continue;
        if (a1 > v18->f_18 && a1 < v18->f_20 && a2 > v18->f_1c&& a2 < v18->f_24)
        {
            v18->f_50 -= a3;
            if (curplr_diff == 0)
                v18->f_50 -= a3;
            return 1;
        }
    }
    return 0;
}

enemy_t *ENEMY_DamageEnergy(int a1, int a2, int a3)
{
    int v14;
    enemy_t *v18;
    for (v14 = 0; v14 < cur_visable; v14++)
    {
        v18 = onscreen[v14];
        if (v18->f_54)
            continue;
        if (a1 > v18->f_18&& a1 < v18->f_20 && a2 > v18->f_1c&& a2 < v18->f_24)
        {
            v18->f_50--;
            if (v18->f_c->f_24)
            {
                if (v18->f_b8 > 0)
                {
                    v18->f_b8 -= a3;
                }
                else
                {
                    v18->f_a8 = 0;
                    v18->f_ac = 1;
                    v18->f_3c = -1;
                    SND_3DPatch(14, v18->f_18 + v18->f_30, v18->f_1c + v18->f_30);
                }
            }
            return v18;
        }
    }
    return NULL;
}

void ENEMY_Think(void)
{
    csprite_t *v24;
    enemy_t *v1c;
    slib_t *v20;
    int v28, v2c, v30, v34, v3c, v38;
    if (boss_sound)
    {
        if (!SND_IsPatchPlaying(23))
            SND_Patch(23, 127);
    }
    tiley = tilepos / 9 - 3;
    while (!end_waveflag && cur_enemy->f_c == tiley)
    {
        do
        {
            v24 = cur_enemy;
            if (cur_enemy->f_14 != 64)
                ENEMY_Add(cur_enemy);
            if (cur_enemy == end_enemy)
            {
                end_waveflag = 1;
                break;
            }
            cur_enemy++;
        } while (v24->f_0 != -1 && v24->f_0 != 1);
    }
    cur_visable = 0;
    for (v1c = first_enemy.f_4; &last_enemy != v1c; v1c = v1c->f_4)
    {
        v20 = v1c->f_c;
        if (v20->f_2c > 1)
        {
            v1c->f_8 = v20->f_10 + v1c->f_8c;
            if (v1c->f_a4 < 1)
            {
                v1c->f_a4 = v20->f_28;
                if (v1c->f_98)
                {
                    v1c->f_8c++;
                    if ((unsigned int)v1c->f_8c >= (unsigned int)v1c->f_94)
                    {
                        v1c->f_8c -= v20->f_34;
                        switch (v20->f_38)
                        {
                        default:
                            EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE1");
                        case 0:
                            break;
                        case 1:
                            v1c->f_98 = 0;
                            v1c->f_a8 = 1;
                            break;
                        case 2:
                            switch (v1c->f_b0)
                            {
                            case 1:
                                v1c->f_94 = v20->f_2c;
                                v1c->f_b0 = 2;
                                break;
                            case 2:
                                v1c->f_a8 = 1;
                                break;
                            }
                            break;
                        }
                    }
                }
            }
            else
                v1c->f_a4--;
            if (v1c->f_88 < 1)
            {
                switch (v20->f_38)
                {
                default:
                    EXIT_Error("ENEMY_Think() - Invalid ANIMTYPE2");
                case 0:
                    v1c->f_a8 = 1;
                    break;
                case 1:
                    v1c->f_98 = 1;
                    break;
                case 2:
                    if (!v1c->f_b0)
                        v1c->f_b0 = 1;
                    break;
                }
            }
            else
                v1c->f_88 -= v20->f_58;
        }
        else
        {
            if (v1c->f_88 < 1)
            {
                v1c->f_88 = -1;
                v1c->f_a8 = 1;
            }
            else
                v1c->f_88 -= v20->f_58;
        }
        switch (v20->f_64)
        {
        case 0:
            v1c->f_18 = v1c->f_5c.f_0;
            v1c->f_1c = v1c->f_5c.f_4;
            v1c->f_20 = v1c->f_18 + v1c->f_28 - 1;
            v1c->f_24 = v1c->f_1c + v1c->f_2c - 1;
            v28 = v20->f_58;
            v28 = MoveEobj(&v1c->f_5c, v28);
            if (v1c->f_5c.f_28)
            {
                v1c->f_5c.f_0 = v1c->f_5c.f_8;
                v1c->f_5c.f_4 = v1c->f_5c.f_c;
                v1c->f_5c.f_8 = v1c->f_10 + v20->f_198[v1c->f_38];
                v1c->f_5c.f_c = v1c->f_14 + v20->f_1d4[v1c->f_38];
                InitMobj(&v1c->f_5c);
                MoveMobj(&v1c->f_5c);
                v28 = MoveEobj(&v1c->f_5c, v28);
                if (!v1c->f_9c)
                {
                    v1c->f_38++;
                    if (v1c->f_38 >= v20->f_5c)
                    {
                        v1c->f_9c = 1;
                        v1c->f_38 = v20->f_5c - 1;
                    }
                }
                else
                {
                    v1c->f_38--;
                    if (v1c->f_38 <= v20->f_60)
                    {
                        v1c->f_9c = 0;
                        v1c->f_38 = v20->f_60;
                    }
                }
            }
            break;
        case 2:
            v1c->f_18 = v1c->f_5c.f_0;
            v1c->f_1c = v1c->f_5c.f_4;
            v1c->f_20 = v1c->f_18 + v1c->f_28 - 1;
            v1c->f_24 = v1c->f_1c + v1c->f_2c - 1;
            v28 = v20->f_58;
            v28 = MoveEobj(&v1c->f_5c, v28);
            if (v1c->f_a0 == 2)
            {
                if (v1c->f_5c.f_4 > 0xc9)
                    v1c->f_58 = 1;
                if (v1c->f_30 + 320 < v1c->f_5c.f_0)
                    v1c->f_58 = 1;
                if (v1c->f_5c.f_4 + v1c->f_28 < 0)
                    v1c->f_58 = 1;
                if (v1c->f_5c.f_0 + v1c->f_28 < 0)
                    v1c->f_58 = 1;
            }
            if (v1c->f_5c.f_28 && v1c->f_a0 != 2)
            {
                v1c->f_5c.f_0 = v1c->f_5c.f_8;
                v1c->f_5c.f_4 = v1c->f_5c.f_c;
                v1c->f_20 = v1c->f_18 + v1c->f_28 - 1;
                v1c->f_24 = v1c->f_1c + v1c->f_2c - 1;
                if (v1c->f_a0 == 1)
                {
                    v1c->f_5c.f_8 = player_cx;
                    v1c->f_5c.f_c = player_cy;
                    v1c->f_a0 = 2;
                }
                else
                {
                    v1c->f_5c.f_8 = v1c->f_10 + v20->f_198[v1c->f_38];
                    v1c->f_5c.f_c = v1c->f_14 + v20->f_1d4[v1c->f_38];
                }
                InitMobj(&v1c->f_5c);
                MoveMobj(&v1c->f_5c);
                v28 = MoveEobj(&v1c->f_5c, v28);
                if (v20->f_5c - 1 > v1c->f_38)
                    v1c->f_38++;
                else if (v1c->f_a0 == 0)
                    v1c->f_a0 = 1;
            }
            break;
        case 1:
            v1c->f_18 = v1c->f_5c.f_0;
            v1c->f_1c = v1c->f_5c.f_4;
            v1c->f_20 = v1c->f_18 + v1c->f_28 - 1;
            v1c->f_24 = v1c->f_1c + v1c->f_2c - 1;
            v28 = v20->f_58;
            v28 = MoveEobj(&v1c->f_5c, v28);
            if (v1c->f_5c.f_28)
            {
                v1c->f_5c.f_0 = v1c->f_5c.f_8;
                v1c->f_5c.f_4 = v1c->f_5c.f_c;
                v1c->f_5c.f_8 = v1c->f_10 + v20->f_198[v1c->f_38];
                v1c->f_5c.f_c = v1c->f_14 + v20->f_1d4[v1c->f_38];
                InitMobj(&v1c->f_5c);
                MoveMobj(&v1c->f_5c);
                v28 = MoveEobj(&v1c->f_5c, v28);
                v1c->f_38++;
                if (v1c->f_38 > v20->f_5c)
                    v1c->f_58 = 1;
            }
            break;
        case 3:
            if (scroll_flag)
                v1c->f_1c++;
            if (v1c->f_1c > v1c->f_5c.f_c)
                v1c->f_58 = 1;
            v1c->f_20 = v1c->f_18 + v1c->f_28 - 1;
            v1c->f_24 = v1c->f_1c + v1c->f_2c - 1;
            break;
        case 5:
            if (scroll_flag)
                v1c->f_1c++;
            if (v1c->f_1c >= 0)
            {
                v1c->f_18 += v20->f_58;
                if (v1c->f_18 > v1c->f_5c.f_8)
                    v1c->f_58 = 1;
                else if (v1c->f_1c > v1c->f_5c.f_c)
                    v1c->f_58 = 1;
            }
            v1c->f_20 = v1c->f_18 + v1c->f_28 - 1;
            v1c->f_24 = v1c->f_1c + v1c->f_2c - 1;
            break;
        case 4:
            if (scroll_flag)
                v1c->f_1c++;
            if (v1c->f_1c >= 0)
            {
                v1c->f_18 -= v20->f_58;
                if (v1c->f_18 < v1c->f_5c.f_8)
                    v1c->f_58 = 1;
                else if (v1c->f_1c > v1c->f_5c.f_c)
                    v1c->f_58 = 1;
            }
            v1c->f_20 = v1c->f_18 + v1c->f_28 - 1;
            v1c->f_24 = v1c->f_1c + v1c->f_2c - 1;
            break;
        }
        if (v1c->f_a8)
        {
            switch (v1c->f_3c)
            {
            case -1:
                v1c->f_44--;
                if (v1c->f_44 < 0)
                {
                    v1c->f_44 = v20->f_1c;
                    if (v1c->f_ac == 0)
                    {
                        for (v2c = 0; v2c < v20->f_68; v2c++)
                        {
                            ESHOT_Shoot(v1c, v2c);
                        }
                    }
                    v1c->f_40--;
                    if (v1c->f_40 < 1)
                        v1c->f_3c = v20->f_54;
                }
                break;
            case 0:
                v1c->f_3c = -1;
                v1c->f_40 = v20->f_50;
                v1c->f_44 = v20->f_1c;
                break;
            default:
                v1c->f_3c--;
                break;
            }
        }
        if (v1c->f_58)
        {
            v1c = ENEMY_Remove(v1c);
            continue;
        }
        if (v20->f_3c)
        {
            if (v1c->f_54)
            {
                SHADOW_GAdd(v1c->f_8, v1c->f_18, v1c->f_1c);
            }
            else
            {
                SHADOW_Add(v1c->f_8, v1c->f_18, v1c->f_1c);
            }
        }
        if (v20->f_40)
        {
            numboss++;
            if (v1c->f_50 < 50 && (gl_cnt & 2) != 0)
            {
                v30 = v1c->f_18 + (wrand() % v1c->f_28);
                v34 = v1c->f_1c + (wrand() % v1c->f_2c);
                ANIMS_StartAnim(6, v30, v34);
            }
        }
        if (!v1c->f_54)
        {
            if (player_cx > v1c->f_18 && player_cx < v1c->f_20)
            {
                if (player_cy > v1c->f_1c && player_cy < v1c->f_24)
                {
                    v1c->f_50 -= 16;
                    if (v1c->f_28 > v1c->f_2c)
                        v3c = v1c->f_28;
                    else
                        v3c = v1c->f_2c;
                    OBJS_SubEnergy(v3c >> 2);
                    v30 = player_cx + (wrand() % 8) - 4;
                    v34 = player_cy + (wrand() % 8) - 4;
                    ANIMS_StartAnim(6, v30, v34);
                    SND_Patch(11, 127);
                }
            }
        }
        if (v1c->f_50 <= 0)
        {
            plr.f_24 += v20->f_48;
            SND_3DPatch(8, v1c->f_18 + v1c->f_30, v1c->f_18 + v1c->f_30);
            switch (v20->f_18)
            {
            case 8:
                ANIMS_StartAnim(8, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                BONUS_Add(23, v1c->f_18, v1c->f_1c);
                break;
            case 0:
                ANIMS_StartAnim(5, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                break;
            case 10:
                ANIMS_StartAnim(7, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                break;
            case 1:
                ANIMS_StartAnim(4, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                break;
            case 2:
                ANIMS_StartAnim(4, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                v38 = (v1c->f_28>>4) * (v1c->f_2c>>4);
                for (v2c = 0; v2c < v38; v2c++)
                {
                    v30 = v1c->f_18 + (wrand() % v1c->f_28);
                    v34 = v1c->f_1c + (wrand() % v1c->f_2c);
                    if (v2c & 1)
                        ANIMS_StartAnim(5, v30, v34);
                    else
                        ANIMS_StartAAnim(7, v30, v34);
                }
                break;
            case 3:
                ANIMS_StartAnim(1, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                break;
            case 4:
                v30 = v1c->f_18 + (wrand() % v1c->f_28);
                v34 = v1c->f_1c + (wrand() % v1c->f_2c);
                if ((wrand() % 2) == 0)
                    ANIMS_StartAnim(18, v30, v34);
                else
                    ANIMS_StartAnim(17, v30, v34);
                ANIMS_StartAnim(0, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                break;
            case 5:
                ANIMS_StartAnim(0, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                v38 = (v1c->f_28>>4) * (v1c->f_2c>>4);
                for (v2c = 0; v2c < v38; v2c++)
                {
                    v30 = v1c->f_18 + (wrand() % v1c->f_28);
                    v34 = v1c->f_1c + (wrand() % v1c->f_2c);
                    if ((wrand()%2) == 0)
                        ANIMS_StartAnim(17, v30, v34);
                    else
                        ANIMS_StartAnim(18, v30, v34);
                    ANIMS_StartAnim(1, v30, v34);
                }
                break;
            case 6:
                ANIMS_StartAnim(4, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                v38 = (v1c->f_28>>4) * (v1c->f_2c>>4);
                for (v2c = 0; v2c < v38; v2c++)
                {
                    v30 = v1c->f_18 + (wrand() % v1c->f_28);
                    v34 = v1c->f_1c + (wrand() % v1c->f_2c);
                    ANIMS_StartAAnim(17, v30, v34);
                    if (v2c & 1)
                        ANIMS_StartAnim(4, v30, v34);
                    else
                        ANIMS_StartAnim(7, v30, v34);
                }
                break;
            case 7:
                ANIMS_StartAnim(2, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                break;
            case 9:
                ANIMS_StartAnim(3, v1c->f_18 + v1c->f_30, v1c->f_1c + v1c->f_34);
                break;
            }
            if (v20->f_14 != -1)
                BONUS_Add(v20->f_14, v1c->f_18, v1c->f_1c);
            v1c = ENEMY_Remove(v1c);
            continue;
        }
        v34 = v1c->f_1c + v1c->f_2c;
        if (v34 > 0 && v1c->f_1c < 200)
        {
            v30 = v1c->f_18 + v1c->f_28;
            if (v30 > 0 && v1c->f_18 < 320)
            {
                onscreen[cur_visable] = v1c;
                cur_visable++;
            }
        }
    }
}

void ENEMY_DisplayGround(void)
{
    enemy_t *v1c;
    for (v1c = first_enemy.f_4; &last_enemy != v1c; v1c = v1c->f_4)
    {
        if (!v1c->f_54)
            continue;
        GFX_PutSprite((texture_t*)GLB_GetItem(v1c->f_8), v1c->f_18, v1c->f_1c);
    }
}

void ENEMY_DisplaySky(void)
{
    int v20;
    enemy_t *v1c;
    for (v1c = first_enemy.f_4; &last_enemy != v1c; v1c = v1c->f_4)
    {
        if (v1c->f_54)
            continue;
        GFX_PutSprite((texture_t*)GLB_GetItem(v1c->f_8), v1c->f_18, v1c->f_1c);
        for (v20 = 0; v20 < v1c->f_c->f_6c; v20++)
        {
            FLAME_Up(v1c->f_18 + v1c->f_c->f_a8[v20], v1c->f_1c + v1c->f_c->f_d8[v20], v1c->f_c->f_108[v20], v1c->f_90);
        }
        v1c->f_90 ^= 1;
    }
}

int ENEMY_GetBaseDamage(void)
{
    static int nums;
    int v1c, v24;
    enemy_t *v20;

    v1c = 0;
    nums = 0;
    for (v20 = first_enemy.f_4; &last_enemy != v20; v20 = v20->f_4)
    {
        if (!v20->f_c->f_40)
            continue;
        if (v20->f_1c + v20->f_34 >= 0)
        {
            v24 = (v20->f_50 * 100) / v20->f_c->f_44;
            v1c += v24;
            nums++;
        }
    }
    if (nums)
        v1c /= nums;
    return v1c;
}
