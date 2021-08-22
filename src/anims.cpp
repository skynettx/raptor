#include <string.h>
#include "common.h"
#include "anims.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "enemy.h"
#include "rap.h"
#include "tile.h"

anim_t first_anims, last_anims, anims[100];
anim_t *free_anims;

animlib_t animlib[25];
int curlib;

int adir[3] = {
    0, -1, 1
};

void ANIMS_Clear(void)
{
    int v1c;
    first_anims.f_0 = NULL;
    first_anims.f_4 = &last_anims;
    last_anims.f_0 = &first_anims;
    last_anims.f_4 = NULL;
    free_anims = anims;
    memset(anims, 0, sizeof(anims));
    for (v1c = 0; v1c < 99; v1c++)
        anims[v1c].f_4 = &anims[v1c + 1];
}

anim_t *ANIMS_Get(void)
{
    anim_t *v1c;
    if (!free_anims)
        return NULL;
    v1c = free_anims;
    free_anims = free_anims->f_4;
    memset(v1c, 0, sizeof(anim_t));
    v1c->f_4 = &last_anims;
    v1c->f_0 = last_anims.f_0;
    last_anims.f_0 = v1c;
    v1c->f_0->f_4 = v1c;
    return v1c;
}

anim_t *ANIMS_Remove(anim_t *a1)
{
    anim_t *v1c;
    v1c = a1->f_0;
    a1->f_4->f_0 = a1->f_0;
    a1->f_0->f_4 = a1->f_4;
    memset(a1, 0, sizeof(anim_t));
    a1->f_4 = free_anims;
    free_anims = a1;
    return v1c;
}


int ANIMS_Register(int a1, int a2, int a3, int a4, int a5, int a6)
{
    animlib_t *v14;
    texture_t *v18;
    int v10;
    v10 = curlib;
    if (curlib >= 25)
        EXIT_Error("ANIMS_Register() - Max LIBs");
    v14 = &animlib[curlib];
    curlib++;
    v14->f_0 = a1;
    v14->f_4 = a2;
    v14->f_8 = a3;
    v14->f_c = a4;
    v14->f_10 = a5;
    v14->f_14 = a6;
    v18 = (texture_t*)GLB_LockItem(a1);
    v14->f_18 = v18->f_c >> 1;
    v14->f_1c = v18->f_10 >> 1;
    GLB_FreeItem(a1);
    return v10;
}

void ANIMS_Init(void)
{
    ANIMS_Clear();
    memset(animlib, 0, sizeof(animlib));
    curlib = 0;
    ANIMS_Register(0x10164, 0x2a, 0, 0, 0, 0);
    ANIMS_Register(0x1027d, 0x23, 0, 0, 0, 0);
    ANIMS_Register(0x101f5, 7, 0, 0, 0, 0);
    ANIMS_Register(0x10213, 10, 0, 0, 0, 0);
    ANIMS_Register(0x101e5, 0xc, 2, 0, 0, 0);
    ANIMS_Register(0x101d8, 0xd, 0, 0, 0, 0);
    ANIMS_Register(0x101fc, 0xe, 2, 0, 0, 0);
    ANIMS_Register(0x102a0, 0x10, 2, 0, 0, 0);
    ANIMS_Register(0x101ae, 0xc, 2, 0, 0, 0);
    ANIMS_Register(0x10229, 4, 2, 0, 0, 0);
    ANIMS_Register(0x1020a, 9, 1, 0, 0, 0);
    ANIMS_Register(0x1020e, 5, 1, 0, 1, 2);
    ANIMS_Register(0x1022d, 4, 1, 0, 1, 1);
    ANIMS_Register(0x1018e, 0xe, 1, 0, 0, 0);
    ANIMS_Register(0x1019c, 9, 1, 0, 0, 0);
    ANIMS_Register(0x101a5, 9, 1, 0, 0, 0);
    ANIMS_Register(0x101c7, 4, 1, 1, 0, 0);
    ANIMS_Register(0x10252, 0x1a, 0, 0, 0, 0);
    ANIMS_Register(0x1026c, 0x11, 0, 0, 0, 0);
    ANIMS_Register(0x1018e, 0xe, 2, 0, 0, 0);
    ANIMS_Register(0x10249, 4, 2, 1, 1, 0);
}

void ANIMS_CachePics(void)
{
    int v1c;
    int v24;
    animlib_t *v20;
    v20 = animlib;
    for (v1c = 0; v1c < curlib; v1c++, v20++)
    {
        for (v24 = 0; v24 < (unsigned int)v20->f_4; v24++)
        {
            GLB_CacheItem(v20->f_4 + v24);
        }
    }
}

void ANIMS_FreePics(void)
{
    int v1c;
    int v24;
    animlib_t *v20;
    v20 = animlib;
    for (v1c = 0; v1c < curlib; v1c++, v20++)
    {
        for (v24 = 0; v24 < (unsigned int)v20->f_4; v24++)
        {
            GLB_FreeItem(v20->f_4 + v24);
        }
    }
}


void ANIMS_StartAnim(int a1, int a2, int a3)
{
    animlib_t *v18;
    anim_t *v14;
    v18 = &animlib[a1];
    v14 = ANIMS_Get();
    if (!v14)
        return;
    v14->f_20 = v18;
    v14->f_14 = a2 - v18->f_18;
    v14->f_18 = a3 - v18->f_1c;
    v14->f_24 = v18->f_8;
}

void ANIMS_StartGAnim(int a1, int a2, int a3)
{
    anim_t *v14;
    v14 = ANIMS_Get();
    if (!v14)
        return;
    v14->f_20 = &animlib[a1];
    v14->f_14 = a2;
    v14->f_18 = a3;
    v14->f_24 = 0;
}

void ANIMS_StartEAnim(enemy_t *a1, int a2, int a3, int a4)
{
    animlib_t *v14;
    anim_t *v10;
    v14 = &animlib[a2];
    v10 = ANIMS_Get();
    if (!v10)
        return;
    v10->f_28 = a1;
    v10->f_20 = &animlib[a2];
    v10->f_14 = a3 - v14->f_18;
    v10->f_18 = a4 - v14->f_1c;
    v10->f_24 = 2;
}

void ANIMS_StartAAnim(int a1, int a2, int a3)
{
    animlib_t *v18;
    anim_t *v14;
    v18 = &animlib[a1];
    v14 = ANIMS_Get();
    if (!v14)
        return;
    v14->f_20 = &animlib[a1];
    v14->f_14 = a2 - v18->f_18;
    v14->f_18 = a3 - v18->f_1c;
    v14->f_24 = 2;
}

void ANIMS_Think(void)
{
    anim_t *v1c;
    animlib_t *v20;

    for (v1c = first_anims.f_4; &last_anims != v1c; v1c = v1c->f_4)
    {
        v20 = v1c->f_20;
        if (v1c->f_1c >= v20->f_4)
        {
            v1c = ANIMS_Remove(v1c);
            continue;
        }
        v1c->f_8 = v20->f_0 + v1c->f_1c;
        if (v20->f_c)
        {
            v1c->f_c = player_cx + v1c->f_14;
            v1c->f_10 = player_cy + v1c->f_18;
        }
        else if (v1c->f_28)
        {
            if (v1c->f_28->f_8 == -1)
                v1c->f_2c = 1;
            if (!v1c->f_2c)
            {
                v1c->f_c = v1c->f_28->f_5c.f_0 + v1c->f_14;
                v1c->f_10 = v1c->f_28->f_5c.f_4 + v1c->f_18;
            }
        }
        else
        {
            v1c->f_c = v1c->f_14;
            v1c->f_10 = v1c->f_18;
        }
        switch (v20->f_14)
        {
        case 0:
            break;
        case 2:
            v1c->f_18++;
            break;
        case 1:
            v1c->f_18--;
            break;
        }
        v1c->f_18 += adir[v20->f_14];
        if (!v20->f_8 && scroll_flag)
            v1c->f_18++;
        v1c->f_1c++;
    }
}

void ANIMS_DisplayGround(void)
{
    anim_t *v1c;
    texture_t *v20;

    for (v1c = first_anims.f_4; &last_anims != v1c; v1c = v1c->f_4)
    {
        if (v1c->f_24)
            continue;
        v20 = (texture_t*)GLB_GetItem(v1c->f_8);
        if (v1c->f_20->f_10)
            GFX_ShadeShape(1, v20, v1c->f_c, v1c->f_10);
        else
            GFX_PutSprite(v20, v1c->f_c, v1c->f_10);
    }
}

void ANIMS_DisplaySky(void)
{
    anim_t *v1c;
    texture_t *v20;

    for (v1c = first_anims.f_4; &last_anims != v1c; v1c = v1c->f_4)
    {
        if (v1c->f_24 != 1)
            continue;
        v20 = (texture_t*)GLB_GetItem(v1c->f_8);
        if (v1c->f_20->f_10)
            GFX_ShadeShape(1, v20, v1c->f_c, v1c->f_10);
        else
            GFX_PutSprite(v20, v1c->f_c, v1c->f_10);
    }
}

void ANIMS_DisplayHigh(void)
{
    anim_t *v1c;
    texture_t *v20;

    for (v1c = first_anims.f_4; &last_anims != v1c; v1c = v1c->f_4)
    {
        if (v1c->f_24 != 2)
            continue;
        v20 = (texture_t*)GLB_GetItem(v1c->f_8);
        if (v1c->f_20->f_10)
            GFX_ShadeShape(1, v20, v1c->f_c, v1c->f_10);
        else
            GFX_PutSprite(v20, v1c->f_c, v1c->f_10);
    }
}
