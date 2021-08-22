#include <stdlib.h>
#include <string.h>
#include "common.h"
#include "tile.h"
#include "gfxapi.h"
#include "rap.h"
#include "glbapi.h"
#include "loadsave.h"
#include "fx.h"
#include "anims.h"

int eitems[1350];
int titems[1350];
short hits[1350];
short tdead[1350];
short money[1350];

tdel_t tdel[81];

tdel_t first_delay;
tdel_t last_delay;

tdel_t *free_delay;

int tileloopy;
char *tilepic;

int scroll_flag;
int last_tile;

char *tilestart;

int startflat[4];

int tilepos;
int tileyoff;

tspot_t tspots[72];
tspot_t *lastspot;
int spark_delay;
int flare_delay;

void TILE_Explode(tspot_t* a1, int a2);

void TClear(void)
{
    int i;
    first_delay.f_0 = NULL;
    first_delay.f_4 = &last_delay;
    last_delay.f_0 = &first_delay;
    last_delay.f_4 = NULL;
    free_delay = &tdel[0];
    memset(tdel, 0, sizeof(tdel));
    for (i = 0; i < 80; i++)
    {
        tdel[i].f_4 = &tdel[i + 1];
    }
}

tdel_t *TGet(void)
{
    tdel_t *v1c;
    if (!free_delay)
        EXIT_Error("TILEDELAY_Get() - Max ");
    v1c = free_delay;
    free_delay = free_delay->f_4;
    memset(v1c, 0, sizeof(tdel_t));
    v1c->f_4 = &last_delay;
    v1c->f_0 = last_delay.f_0;
    last_delay.f_0 = v1c;
    v1c->f_0->f_4 = v1c;
    return v1c;
}

tdel_t *TRemove(tdel_t *a1)
{
    tdel_t *v1c;
    v1c = a1->f_0;
    a1->f_4->f_0 = a1->f_0;
    a1->f_0->f_4 = a1->f_4;
    memset(a1, 0, sizeof(tdel_t));
    a1->f_4 = free_delay;
    free_delay = a1;
    return v1c;
}

void FUN_000127a1(int a1, int a2)
{
    static int mlookup[3] = {
        -1, -9, 1
    };
    static int xlookup[3] = {
        -1, 0, 1
    };
    int i, v18, v20, v24;
    v18 = a1 % 9;
    for (i = 0; i < 3; i++)
    {
        v20 = a1 + mlookup[i];
        if (v20 < 0 || v20 >= 1350)
            continue;
        if (eitems[v20] != titems[v20])
        {
            v24 = v18 + xlookup[i];
            if (v24 < 0 || v24 >= 9)
                continue;
            hits[v20] -= a2;
        }
    }
}

void TILE_Put(char *a1, int a2, int a3)
{
    int v14;
    v14 = 0;
    if (a3 + 32 <= 0 || a3 >= 200)
        return;
    tileloopy = 32;
    tilepic = a1;
    if (a3 < 0)
    {
        tilepic += a3 * -32;
        tileloopy += a3;
        a3 = 0;
        v14 = 1;
    }
    if (a3 + tileloopy > 200)
    {
        tileloopy = 200 - a3;
        v14 = 1;
    }
    tilestart = displaybuffer + ylookup[a3] + a2;
    if (v14)
        TILE_ClipDraw();
    else
        TILE_Draw();
}

void TILE_Init(void)
{
    TClear();
    g_mapleft = 16;
    scroll_flag = 1;
    last_tile = 0;
    startflat[0] = GLB_GetItemID("STARTG1TILES");
    startflat[0]++;
    startflat[1] = GLB_GetItemID("STARTG2TILES");
    startflat[1]++;
    startflat[2] = GLB_GetItemID("STARTG3TILES");
    startflat[2]++;
    startflat[3] = GLB_GetItemID("STARTG4TILES");
    startflat[3]++;
}

void TILE_CacheLevel(void)
{
    int i, v1c, v24;
    flat_t *v20;
    TClear();
    g_mapleft = 16;
    tilepos = 1278;
    tileyoff = -56;
    lastspot = &tspots[71];
    scroll_flag = 1;
    last_tile = 0;
    memset(titems, 0, sizeof(titems));
    memset(eitems, 0, sizeof(eitems));
    memset(hits, 0, sizeof(hits));
    memset(tdead, 0, sizeof(tdead));
    for (i = 0; i < 1350; i++)
    {
        v1c = mapmem->f_c[i].f_2;
        v20 = flatlib[v1c];
        money[i] = v20[mapmem->f_c[i].f_0].f_6;
        v24 = startflat[v1c];
        v24 += mapmem->f_c[i].f_0;
        titems[i] = v24;
        GLB_CacheItem(v24);
        v24 = startflat[v1c];
        v24 += v20[mapmem->f_c[i].f_0].f_0;
        eitems[i] = v24;
        if (eitems[i] != titems[i])
            GLB_CacheItem(v24);
        if (eitems[i] != titems[i])
            hits[i] = v20[mapmem->f_c[i].f_0].f_4;
        else
            hits[i] = 1;
    }
}

void TILE_FreeLevel(void)
{
    int i;
    for (i = 0; i < 1350; i++)
    {
        GLB_FreeItem(titems[i]);
        if (eitems[i] != titems[i])
            GLB_FreeItem(eitems[i]);
    }
}

void TILE_DamageAll(void)
{
    tspot_t *v1c;
    v1c = tspots;
    do
    {
        if (eitems[v1c->f_c] != titems[v1c->f_c])
            hits[v1c->f_c] -= 20;
        if (v1c == lastspot)
            break;
        v1c++;
    } while (1);
}

void TILE_Think(void)
{
    int v3c, v38, v34, v30, v2c, i, j;
    tspot_t *v1c;
    tdel_t *v20;

    v34 = tileyoff;
    v2c = tilepos;
    v1c = tspots;
    for (i = 0; i < 8; i++, v34 += 32)
    {
        v30 = 16;
        for (j = 0; j < 9; j++, v30 += 32, v2c++, v1c++)
        {
            v1c->f_c = v2c;
            v1c->f_4 = v30;
            v1c->f_8 = v34;
            v1c->f_0 = titems[v2c];
            if (hits[v2c] < 0 && !tdead[v2c])
            {
                SND_3DPatch(15, v30 + 16, v34 + 16);
                FUN_000127a1(v1c->f_c, 5);
                plr.f_24 += money[v2c];
                TILE_Explode(v1c, 10);
                ANIMS_StartAnim(0, v30 + 16, v34 + 16);
                tdead[v2c] = 1;
            }
        }
    }
    for (v20 = first_delay.f_4; &last_delay != v20; v20 = v20->f_4)
    {
        if (v20->f_c - tilepos > 72)
        {
            v20 = TRemove(v20);
            continue;
        }
        if (v20->f_8 < 0)
        {
            v38 = v20->f_14->f_4 + 8 + (wrand() % 8);
            v3c = v20->f_14->f_8 + 26;
            FUN_000127a1(v20->f_c, 20);
            spark_delay++;
            flare_delay++;
            if (spark_delay > 2)
            {
                ANIMS_StartAnim(18, v38, v3c);
                spark_delay = 0;
            }
            if (flare_delay > 4)
            {
                ANIMS_StartAnim(17, v38, v3c);
                flare_delay = 0;
            }
            v1c = &tspots[v20->f_c - tilepos];
            titems[v1c->f_c] = v20->f_10;
            eitems[v1c->f_c] = v20->f_10;
            v20 = TRemove(v20);
        }
        else
            v20->f_8--;
    }
}

void TILE_Display(void)
{
    char *v20;
    tspot_t *v1c;

    v1c = tspots;
    do
    {
        v20 = GLB_GetItem(v1c->f_0);
        v20 += 20;
        TILE_Put(v20, v1c->f_4, v1c->f_8);
        if (v1c == lastspot)
            break;
        v1c++;
    } while (1);
    tileyoff++;
    if (tileyoff > 0)
    {
        if (last_tile && tileyoff >= 0)
        {
            tileyoff = 0;
            scroll_flag = 0;
        }
        else
        {
            tileyoff -= 32;
            tilepos -= 9;
        }
        if (tilepos <= 0)
        {
            tilepos = 0;
            last_tile = 1;
        }
    }
}

int TILE_IsHit(int a1, int a2, int a3)
{
    tspot_t *v14 = tspots;
    while (v14 != lastspot)
    {
        if (a2 >= v14->f_4 && v14->f_4 + 32 > a2 && a3 >= v14->f_8 && v14->f_8 + 32 > a3)
        {
            if (eitems[v14->f_c] != titems[v14->f_c])
            {
                hits[v14->f_c] -= a1;
                switch (wrand() % 2)
                {
                case 0:
                    ANIMS_StartGAnim(14, a2, a3);
                    break;
                case 1:
                    ANIMS_StartGAnim(15, a2, a3);
                    break;
                }
                return 1;
            }
        }
        v14++;
    }
    return 0;
}

int TILE_Bomb(int a1, int a2, int a3)
{
    tspot_t *v14 = tspots;
    while (v14 != lastspot)
    {
        if (a2 >= v14->f_4 && v14->f_4 + 32 > a2 && a3 >= v14->f_8 && v14->f_8 + 32 > a3)
        {
            if (eitems[v14->f_c] != titems[v14->f_c])
            {
                hits[v14->f_c] -= a1;
                FUN_000127a1(v14->f_c, a1);
                if (v14->f_c > 9)
                    FUN_000127a1(v14->f_c - 9, a1 >> 1);
                return 1;
            }
        }
        v14++;
    }
    return 0;
}

void TILE_Explode(tspot_t *a1, int a2)
{
    tdel_t *v18;
    if (a1->f_c != -1 && a1->f_c >= tilepos)
    {
        if (a2)
        {
            v18 = TGet();
            if (v18)
            {
                v18->f_14 = a1;
                v18->f_c = a1->f_c;
                v18->f_8 = a2;
                v18->f_10 = eitems[a1->f_c];
            }
            eitems[a1->f_c] = titems[a1->f_c];
        }
        else
        {
            a1->f_0 = eitems[a1->f_c];
            titems[a1->f_c] = eitems[a1->f_c];
        }
    }
}

