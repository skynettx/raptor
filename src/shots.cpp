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
        detpow[i] = (texture_t*)GLB_LockItem(0x10239 + i);
    }
    for (i = 0; i < 4; i++)
    {
        laspow[i] = (texture_t*)GLB_LockItem(0x1023d + i);
    }
    for (i = 0; i < 4; i++)
    {
        lashit[i] = (texture_t*)GLB_LockItem(0x101f1 + i);
    }
    memset(shot_lib, 0, sizeof(shot_lib));
    v1c = &shot_lib[0];
    v1c->f_0 = 0x101c3;
    v1c->f_2c = 0;
    v1c->f_30 = 0;
    v1c->f_34 = 1;
    v1c->f_38 = 8;
    v1c->f_3c = 0x10;
    v1c->f_40 = 0;
    v1c->f_44 = 4;
    v1c->f_50 = 0;
    v1c->f_48 = 2;
    v1c->f_4c = 0;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 0;

    v1c = &shot_lib[1];
    v1c->f_0 = 65999;
    v1c->f_2c = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 2;
    v1c->f_38 = 4;
    v1c->f_3c = 8;
    v1c->f_40 = 0;
    v1c->f_44 = 2;
    v1c->f_50 = 0;
    v1c->f_48 = 10;
    v1c->f_4c = 0;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 1;

    v1c = &shot_lib[2];
    v1c->f_0 = 0x101c1;
    v1c->f_2c = 0;
    v1c->f_30 = 2;
    v1c->f_34 = 2;
    v1c->f_38 = 2;
    v1c->f_3c = 8;
    v1c->f_40 = 0;
    v1c->f_44 = 2;
    v1c->f_50 = 0;
    v1c->f_48 = 4;
    v1c->f_4c = 0;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 3;

    v1c = &shot_lib[3];
    v1c->f_0 = 0x101ba;
    v1c->f_2c = 1;
    v1c->f_30 = 3;
    v1c->f_34 = 4;
    v1c->f_38 = 2;
    v1c->f_3c = 0xc;
    v1c->f_40 = 1;
    v1c->f_44 = 3;
    v1c->f_50 = 1;
    v1c->f_48 = 10;
    v1c->f_4c = 0;
    v1c->f_58 = 1;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 0;

    v1c = &shot_lib[4];
    v1c->f_0 = 0x101c3;
    v1c->f_2c = 1;
    v1c->f_30 = 4;
    v1c->f_34 = 1;
    v1c->f_38 = 8;
    v1c->f_3c = 10;
    v1c->f_40 = 1;
    v1c->f_44 = 4;
    v1c->f_50 = 0;
    v1c->f_48 = 1;
    v1c->f_4c = 0;
    v1c->f_58 = 1;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 3;

    v1c = &shot_lib[5];
    v1c->f_0 = -1;
    v1c->f_2c = 0;
    v1c->f_30 = 5;
    v1c->f_34 = 5;
    v1c->f_38 = 0;
    v1c->f_3c = 0;
    v1c->f_40 = 0;
    v1c->f_44 = 0;
    v1c->f_50 = 0;
    v1c->f_48 = 6;
    v1c->f_4c = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 0;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 1;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    // v1c->f_78 = v1c->f_74->f_c >> 1;
    // v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 0;

    v1c = &shot_lib[6];
    v1c->f_0 = 0x101bd;
    v1c->f_2c = 0;
    v1c->f_30 = 6;
    v1c->f_34 = 4;
    v1c->f_38 = 1;
    v1c->f_3c = 0x10;
    v1c->f_40 = 0;
    v1c->f_44 = 2;
    v1c->f_50 = 0;
    v1c->f_48 = 5;
    v1c->f_4c = 0;
    v1c->f_54 = 1;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 1;

    v1c = &shot_lib[7];
    v1c->f_0 = 0x101bd;
    v1c->f_2c = 0;
    v1c->f_30 = 6;
    v1c->f_34 = 4;
    v1c->f_38 = 1;
    v1c->f_3c = 0xc;
    v1c->f_40 = 0;
    v1c->f_44 = 2;
    v1c->f_50 = 0;
    v1c->f_48 = 10;
    v1c->f_4c = 0;
    v1c->f_54 = 1;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 1;

    v1c = &shot_lib[8];
    v1c->f_0 = 0x101bf;
    v1c->f_2c = 0;
    v1c->f_30 = 8;
    v1c->f_34 = 0x14;
    v1c->f_38 = 1;
    v1c->f_3c = 6;
    v1c->f_40 = 0;
    v1c->f_44 = 2;
    v1c->f_50 = 0;
    v1c->f_48 = 0x14;
    v1c->f_4c = 0;
    v1c->f_54 = 1;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 2;

    v1c = &shot_lib[9];
    v1c->f_0 = 0x101d3;
    v1c->f_2c = 0;
    v1c->f_30 = 9;
    v1c->f_34 = 0x32;
    v1c->f_38 = 1;
    v1c->f_3c = 4;
    v1c->f_40 = 0;
    v1c->f_44 = 1;
    v1c->f_50 = 0;
    v1c->f_48 = 0x1e;
    v1c->f_4c = 0;
    v1c->f_54 = 0;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 4;

    v1c = &shot_lib[10];
    v1c->f_0 = 0x1021d;
    v1c->f_2c = 0;
    v1c->f_30 = 10;
    v1c->f_34 = 3;
    v1c->f_38 = 4;
    v1c->f_3c = 8;
    v1c->f_40 = 0;
    v1c->f_44 = 6;
    v1c->f_50 = 0;
    v1c->f_48 = 2;
    v1c->f_4c = 0;
    v1c->f_54 = 0;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 5;

    v1c = &shot_lib[11];
    v1c->f_0 = 0x101cb;
    v1c->f_2c = 1;
    v1c->f_30 = 0xb;
    v1c->f_34 = 0x32;
    v1c->f_38 = 2;
    v1c->f_3c = 2;
    v1c->f_40 = 0;
    v1c->f_44 = 4;
    v1c->f_50 = 0;
    v1c->f_48 = 0x3c;
    v1c->f_4c = 0;
    v1c->f_54 = 0;
    v1c->f_58 = 1;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 1;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 0;

    v1c = &shot_lib[12];
    v1c->f_0 = 0x10223;
    v1c->f_2c = 0;
    v1c->f_30 = 0xc;
    v1c->f_34 = 5;
    v1c->f_38 = 8;
    v1c->f_3c = 8;
    v1c->f_40 = 0;
    v1c->f_44 = 2;
    v1c->f_50 = 0;
    v1c->f_48 = 3;
    v1c->f_4c = 0;
    v1c->f_54 = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 1;
    v1c->f_64 = 0;
    v1c->f_68 = 0;
    v1c->f_6c = 0;
    v1c->f_70 = 0;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 0;

    v1c = &shot_lib[13];
    v1c->f_0 = 0x10235;
    v1c->f_2c = 0;
    v1c->f_30 = 0xd;
    v1c->f_34 = 10;
    v1c->f_38 = 0;
    v1c->f_3c = 0;
    v1c->f_40 = 0;
    v1c->f_44 = 4;
    v1c->f_50 = 0;
    v1c->f_48 = 7;
    v1c->f_4c = 0;
    v1c->f_54 = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 0;
    v1c->f_64 = 1;
    v1c->f_68 = 1;
    v1c->f_6c = 1;
    v1c->f_70 = 2;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 1;

    v1c = &shot_lib[14];
    v1c->f_0 = 0x10231;
    v1c->f_2c = 0;
    v1c->f_30 = 0xe;
    v1c->f_34 = 6;
    v1c->f_38 = 0;
    v1c->f_3c = 0;
    v1c->f_40 = 0;
    v1c->f_44 = 4;
    v1c->f_50 = 0;
    v1c->f_48 = 7;
    v1c->f_4c = 0;
    v1c->f_54 = 0;
    v1c->f_58 = 0;
    v1c->f_5c = 0;
    v1c->f_64 = 1;
    v1c->f_68 = 1;
    v1c->f_6c = 1;
    v1c->f_70 = 2;
    for (i = 0; i < v1c->f_44; i++)
    {
        v24 = v1c->f_0 + i;
        v1c->f_4[i] = (texture_t*)GLB_LockItem(v24);
    }
    v1c->f_74 = v1c->f_4[0];
    v1c->f_78 = v1c->f_74->f_c >> 1;
    v1c->f_7c = v1c->f_74->f_10 >> 1;
    v1c->f_60 = 3;
}

int SHOTS_PlayerShoot(int a1)
{
    shot_lib_t *v20;
    shot_t *v1c;
    enemy_t *v24;

    v20 = &shot_lib[a1];
    if (a1 == -1)
        EXIT_Error("SHOTS_PlayerShoot() type = EMPTY  ");
    if (v20->f_4c)
        return 0;
    v20->f_4c = v20->f_48;
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
        v1c->f_44 = (wrand() % v20->f_44);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;

        v1c->f_c = player_cx + o_gun1[playerpic];
        v1c->f_10 = player_cy;
        
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, o_gun1[playerpic], 0);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->f_44 = (wrand() % v20->f_44);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;

        v1c->f_c = player_cx - o_gun1[playerpic] - 1;
        v1c->f_10 = player_cy;

        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, -o_gun1[playerpic] - 1, 0);
        break;
    case 1:
        if (!fx_gus)
            SND_Patch(16, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_44 = wrand() % v20->f_44;
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx;
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 2:
        if (!fx_gus)
            SND_Patch(16, 127);
        g_flash = 7;
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx + o_gun3[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx - o_gun3[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 3:
        if (!fx_gus)
            SND_Patch(19, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx;
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c + (wrand() % 16) + 10;
        v1c->f_14.f_c = v1c->f_10 + 5;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->f_14);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx;
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c - (wrand() % 16) - 10;
        v1c->f_14.f_c = v1c->f_10 + 5;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->f_14);
        break;
    case 4:
        v24 = ENEMY_GetRandom();
        if (!v24)
            SHOTS_Remove(v1c);
        else
        {
            if (!fx_gus)
                SND_Patch(16, 127);
            v1c->f_44 = wrand() % v20->f_44;
            v1c->f_58 = &shot_lib[a1];
            v1c->f_4c = v20->f_50;
            v1c->f_40 = v20->f_38;
            v1c->f_c = player_cx;
            v1c->f_10 = player_cy;
            v1c->f_14.f_0 = v1c->f_c;
            v1c->f_14.f_4 = v1c->f_10;
            v1c->f_14.f_8 = (wrand() % v24->f_28) + v24->f_18 - 1;
            v1c->f_14.f_c = (wrand() % v24->f_2c) + v24->f_34 + v24->f_1c - 1;
            v1c->f_50 = player_cx;
            v1c->f_54 = player_cy;
            InitMobj(&v1c->f_14);
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
            v1c->f_58 = &shot_lib[a1];
            v24->f_50 -= v20->f_34;
            v1c->f_44 = 0;
            v1c->f_4c = v20->f_50;
            v1c->f_40 = v20->f_38;
            v1c->f_c = player_cx;
            v1c->f_10 = player_cy;
            v1c->f_14.f_0 = (wrand() % v24->f_28) + v24->f_5c.f_0 - 1;
            v1c->f_14.f_4 = (wrand() % v24->f_2c) + v24->f_5c.f_4 - 1;
            v1c->f_14.f_8 = player_cx;
            v1c->f_14.f_c = player_cy;
            v1c->f_50 = player_cx;
            v1c->f_54 = player_cy;
            InitMobj(&v1c->f_14);
            ANIMS_StartAnim(9, v1c->f_14.f_0, v1c->f_14.f_4);
        }
        break;
    case 6:
        SND_Patch(16, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx + o_gun2[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, o_gun2[playerpic], 1);
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx - o_gun2[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        ANIMS_StartAnim(16, -o_gun2[playerpic] - 1, 1);
        break;
    case 7:
        SND_Patch(19, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx + o_gun2[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx - o_gun2[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 8:
        SND_Patch(19, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx + o_gun2[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx - o_gun2[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 9:
        SND_Patch(19, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx;
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 10:
        SND_Patch(16, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx - 4;
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 11:
        SND_Patch(16, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx;
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = 160;
        v1c->f_14.f_c = 75;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        InitMobj(&v1c->f_14);
        break;
    case 12:
        SND_Patch(35, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx;
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = 0;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 13:
        SND_Patch(18, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx + o_gun3[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = -24;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c = SHOTS_Get();
        if (!v1c)
            return 0;
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx - o_gun3[playerpic];
        v1c->f_10 = player_cy;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = -24;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        break;
    case 14:
        SND_Patch(18, 127);
        v1c->f_58 = &shot_lib[a1];
        v1c->f_4c = v20->f_50;
        v1c->f_40 = v20->f_38;
        v1c->f_c = player_cx;
        v1c->f_10 = player_cy - 24;
        v1c->f_14.f_0 = v1c->f_c;
        v1c->f_14.f_4 = v1c->f_10;
        v1c->f_50 = player_cx;
        v1c->f_54 = player_cy;
        v1c->f_14.f_8 = v1c->f_c;
        v1c->f_14.f_c = -24;
        break;
    }
    return 1;
}


void SHOTS_Think(void)
{
    shot_lib_t *v1c;
    shot_t *v20;
    enemy_t *v24;
    int v2c;
    int i;

    v1c = shot_lib;
    for (i = 0; i <= 14; i++, v1c++)
    {
        if (v1c->f_4c > 0)
            v1c->f_4c--;
    }

    for (v20 = first_shots.f_4; &last_shots != v20; v20 = v20->f_4)
    {
        v1c = v20->f_58;
        switch (v1c->f_70)
        {
        default:
            EXIT_Error("SHOTS_Think()");
            break;
        case 0:
            v20->f_8 = v1c->f_4[v20->f_44];
            v20->f_c = v20->f_14.f_0 - v1c->f_78;
            if (v1c->f_5c)
                v20->f_10 = v20->f_14.f_4 - v1c->f_7c;
            else
                v20->f_10 = v20->f_14.f_4;
            if (v1c->f_54)
                ANIMS_StartAnim(11, v20->f_c + v1c->f_78, v20->f_10 + v1c->f_7c * 2);
            break;
        case 1:
            v20->f_c = v20->f_14.f_0;
            v20->f_10 = v20->f_14.f_4;
            break;
        case 2:
            v20->f_8 = v1c->f_4[v20->f_44];
            v20->f_c = v20->f_14.f_0 - v1c->f_78;
            v20->f_10 = v20->f_14.f_4;
            for (v24 = first_enemy.f_4; &last_enemy != v24; v24 = v24->f_4)
            {
                if (v20->f_c > v24->f_18 && v20->f_c < v24->f_20 && v24->f_1c < player_cy && v24->f_1c > -30)
                {
                    v24->f_50 -= v1c->f_34;
                    if (v24->f_50 != -1)
                    {
                        v20->f_14.f_c = v24->f_1c + v24->f_34;
                        break;
                    }
                }
            }
            break;
        }
        if (v1c->f_64)
            v20->f_c += player_cx - v20->f_50;
        if (v1c->f_68)
            v20->f_10 += player_cy - v20->f_54;
        //if (v20->f_10 + 16 < 0 || v20->f_c < 0 && v20->f_c > 320 && v20->f_10 > 200)
        if ((v20->f_10 + 16 < 0) || (v20->f_c < 0) || (v20->f_c > 320) || (v20->f_10 > 200))
        {
            if (v1c->f_5c)
            {
                v20->f_14.f_28 = 1;
                goto LAB_00015d11;
            }
        }
        
        if (v20->f_4c == 0)
        {
            if (v20->f_40 < v1c->f_3c)
                v20->f_40++;
            v20->f_44++;
            if (v20->f_44 >= v1c->f_44)
            {
                if (v1c->f_5c)
                    v20->f_44 = v1c->f_40;
                else
                {
                    v20->f_14.f_28 = 1;
                    goto LAB_00015d11;
                }
            }
        }
        if (v20->f_48)
        {
            v20->f_14.f_28 = 1;
            goto LAB_00015d11;
        }
        if (v1c->f_6c)
            goto LAB_00015d11;
        switch (v1c->f_60)
        {
        default:
            v24 = ENEMY_DamageEnergy(v20->f_c, v20->f_10, v1c->f_34);
            if (v24)
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(14, v20->f_c, v20->f_10);
                ANIMS_StartEAnim(v24, 19, v24->f_30, v24->f_34);
            }
            break;
        case 3:
            if (ENEMY_DamageAll(v20->f_c, v20->f_10, v1c->f_34))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->f_c, v20->f_10);
                else
                    ANIMS_StartAnim(15, v20->f_c, v20->f_10);
            }
            break;
        case 0:
            if (ENEMY_DamageAll(v20->f_c, v20->f_10, v1c->f_34))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->f_c, v20->f_10);
                else
                    ANIMS_StartAnim(15, v20->f_c, v20->f_10);
            }
            else
            {
                if (TILE_IsHit(v1c->f_34, v20->f_c, v20->f_10))
                {
                    v20->f_14.f_28 = 1;
                }
            }
            break;
        case 1:
            if (ENEMY_DamageAir(v20->f_c, v20->f_10, v1c->f_34))
            {
                v20->f_48 = 1;
                if ((wrand() % 2) != 0)
                    ANIMS_StartAnim(14, v20->f_c, v20->f_10);
                else
                    ANIMS_StartAnim(15, v20->f_c, v20->f_10);
            }
            break;
        case 2:
            if (ENEMY_DamageGround(v20->f_c, v20->f_10, v1c->f_34))
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(15, v20->f_c, v20->f_10);
            }
            else
            {
                if (TILE_IsHit(v1c->f_34, v20->f_c, v20->f_10))
                {
                    v20->f_14.f_28 = 1;
                }
            }
            break;
        case 4:
            if (TILE_Bomb(v1c->f_34, v20->f_c, v20->f_10))
            {
                v20->f_14.f_28 = 1;
            }
            if (ENEMY_DamageGround(v20->f_c, v20->f_10, 5))
            {
                v20->f_48 = 1;
                ANIMS_StartAnim(1, v20->f_c, v20->f_10);
            }
            break;
        }
    LAB_00015d11:
        if (v20->f_14.f_28)
        {
            if (v20->f_4c)
            {
                v20->f_4c = 0;
                v20->f_14.f_8 = v20->f_14.f_0 + ((wrand() % 32) - 16);
                v20->f_14.f_c = 0;
                ANIMS_StartAnim(11, v20->f_14.f_0, v20->f_14.f_4);
                InitMobj(&v20->f_14);
            }
            else
            {
                switch (v1c->f_30)
                {
                case 11:
                    ESHOT_Clear();
                    TILE_DamageAll();
                    for (v24 = first_enemy.f_4; &last_enemy != v24; v24 = v24->f_4)
                    {
                        v24->f_50 -= v1c->f_34;
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
                MoveSobj(&v20->f_14, v20->f_40);
            }
            else
            {
                v20->f_14.f_4 -= v20->f_40;
                if (v20->f_14.f_4 < 0)
                {
                    v20->f_14.f_28 = 1;
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
        switch (v1c->f_58->f_70)
        {
        default:
            EXIT_Error("SHOTS_Display()");
            break;
        case 0:
            GFX_PutSprite(v1c->f_8, v1c->f_c, v1c->f_10);
            break;
        case 1:
            GFX_Line(player_cx + 1, player_cy, v1c->f_14.f_0, v1c->f_14.f_4, 69);
            GFX_Line(player_cx - 1, player_cy, v1c->f_14.f_0, v1c->f_14.f_4, 69);
            GFX_Line(player_cx, player_cy, v1c->f_14.f_0, v1c->f_14.f_4, 64);
            v1c = SHOTS_Remove(v1c);
            break;
        case 2:
            for (v20 = v1c->f_14.f_c; v20 < v1c->f_10; v20 += 3)
            {
                GFX_PutSprite(v1c->f_8, v1c->f_c, v20);
            }
            if (v1c->f_58->f_30 == 14)
                GFX_PutSprite(detpow[v1c->f_5c], v1c->f_c - 4, v1c->f_10);
            else
                GFX_PutSprite(laspow[v1c->f_5c], v1c->f_c, v1c->f_10);
            v2c = lashit[v1c->f_5c];
            v24 = v1c->f_c - (v2c->f_c >> 2);
            v28 = v1c->f_14.f_c - 8;
            if (v28 > 0)
                GFX_PutSprite(v2c, v24, v28);
            v1c->f_5c++;
            v1c->f_5c = v1c->f_5c % 4;
            break;
        }
    }
}
