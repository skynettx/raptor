#include <string.h>
#include "common.h"
#include "objects.h"
#include "glbapi.h"
#include "enemy.h"
#include "shots.h"
#include "loadsave.h"
#include "fx.h"
#include "anims.h"
#include "fileids.h"

int obj_cnt;
object_t first_objs, last_objs, objs[20];
object_t *free_objs;
object_t *p_objs[24];

objlib_t obj_lib[24];

int objuse_flag;
int think_cnt;

void OBJS_Clear(void)
{
    int v1c;
    obj_cnt = 0;
    first_objs.f_0 = NULL;
    first_objs.f_4 = &last_objs;
    last_objs.f_0 = &first_objs;
    last_objs.f_4 = NULL;
    free_objs = objs;
    memset(objs, 0, sizeof(objs));
    memset(p_objs, 0, sizeof(p_objs));
    for (v1c = 0; v1c < 19; v1c++)
        objs[v1c].f_4 = &objs[v1c + 1];
}

object_t *OBJS_Get(void)
{
    object_t *v1c;
    if (!free_objs)
        return 0;
    v1c = free_objs;
    free_objs = free_objs->f_4;
    memset(v1c, 0, sizeof(object_t));
    v1c->f_4 = &last_objs;
    v1c->f_0 = last_objs.f_0;
    last_objs.f_0 = v1c;
    v1c->f_0->f_4 = v1c;
    obj_cnt++;
    return v1c;
}

object_t *OBJS_Remove(object_t *a1)
{
    object_t *v1c;
    v1c = a1->f_0;
    a1->f_4->f_0 = a1->f_0;
    a1->f_0->f_4 = a1->f_4;
    memset(a1, 0, sizeof(object_t));
    a1->f_4 = free_objs;
    free_objs = a1;
    obj_cnt--;
    return v1c;
}

void OBJS_CachePics(void)
{
    int v20 , v24;
    objlib_t *v1c;
    for (v20 = 0; v20 < 24; v20++)
    {
        v1c = &obj_lib[v20];
        if (v1c && v1c->f_0 != -1)
        {
            for (v24 = 0; v24 < v1c->f_4; v24++)
            {
                GLB_CacheItem(v1c->f_0 + v24);
            }
        }
    }
    GLB_CacheItem(FILE1e0_SMSHIELD_PIC);
    GLB_CacheItem(FILE1df_SMBOMB_PIC);
}

void OBJS_FreePics(void)
{
    int v20, v24;
    objlib_t* v1c;
    for (v20 = 0; v20 < 24; v20++)
    {
        v1c = &obj_lib[v20];
        if (v1c && v1c->f_0 != -1)
        {
            for (v24 = 0; v24 < v1c->f_4; v24++)
            {
                GLB_FreeItem(v1c->f_0 + v24);
            }
        }
    }
    GLB_FreeItem(FILE1e0_SMSHIELD_PIC);
    GLB_FreeItem(FILE1df_SMBOMB_PIC);
}

void OBJS_Init(void)
{
    objlib_t *v1c;
    OBJS_Clear();
    memset(obj_lib, 0, sizeof(obj_lib));
    memset(p_objs, 0, sizeof(p_objs));
    v1c = &obj_lib[0];
    v1c->f_0 = FILE1b0_BONUS00_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x2ee0;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 0;
    v1c->f_28 = 0;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

//Plasma Cannon
    v1c = &obj_lib[1];
    v1c->f_0 = FILE1b1_BONUS01_PIC;
    v1c->f_4 = 2;
    v1c->payAmount = 0x133d0;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;


    v1c = &obj_lib[2];
    v1c->f_0 = FILE1b3_BONUS02_PIC;
    v1c->f_4 = 2;
    v1c->payAmount = 0x2adf0;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    //Dumbfire Missile
    v1c = &obj_lib[3];
    v1c->f_0 = FILE1b5_BONUS03_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x23730;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x13;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[4];
    v1c->f_0 = FILE1b6_BONUS04_PIC;
    v1c->f_4 = 4;
    v1c->payAmount = 0x3d31a;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[5];
    v1c->f_0 = FILE1ba_BONUS05_PIC;
    v1c->f_4 = 4;
    v1c->payAmount = 0x7d352;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x12;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 0;
    v1c->f_34 = 1;

    v1c = &obj_lib[6];
    v1c->f_0 = FILE1be_BONUS06_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x32096;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    //Air/Air Missile
    v1c = &obj_lib[7];
    v1c->f_0 = FILE1bf_BONUS07_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0xf80c;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x13;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    //AIR/Ground Missile
    v1c = &obj_lib[8];
    v1c->f_0 = FILE1c0_BONUS08_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 110000;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x13;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    //Bomb
    v1c = &obj_lib[9];
    v1c->f_0 = FILE1da_BONUS21_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x17f98;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x13;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 0;
    v1c->f_34 = 1;

    v1c = &obj_lib[10];
    v1c->f_0 = FILE1c1_BONUS09_PIC;
    v1c->f_4 = 4;
    v1c->payAmount = 0x496ce;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 0;
    v1c->f_34 = 1;

    //Megabomb
    v1c = &obj_lib[11];
    v1c->f_0 = FILE1c5_BONUS10_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x07dfa;
    v1c->resaleAmount = 1;
    v1c->f_10 = 5;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 0;
    v1c->f_20 = 1;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[12];
    v1c->f_0 = FILE1c6_BONUS11_PIC;
    v1c->f_4 = 2;
    v1c->payAmount = 0xb1008;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x10;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[13];
    v1c->f_0 = FILE1c8_BONUS12_PIC;
    v1c->f_4 = 4;
    v1c->payAmount = 0x1ab3f0;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x12;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 0;
    v1c->f_34 = 1;

    v1c = &obj_lib[14];
    v1c->f_0 = FILE1cc_BONUS13_PIC;
    v1c->f_4 = 4;
    v1c->payAmount = 950000;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_14 = SHOTS_PlayerShoot;
    v1c->f_18 = 0x12;
    v1c->f_1c = 1;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 1;
    v1c->pays = 0;
    v1c->f_30 = 0;
    v1c->f_34 = 1;

    v1c = &obj_lib[15];
    v1c->f_0 = FILE1d0_BONUS14_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x132a4;
    v1c->resaleAmount = 100;
    v1c->f_10 = 100;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 0;
    v1c->f_24 = 0;
    v1c->f_28 = 0;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[16];
    v1c->f_0 = FILE1d1_BONUS15_PIC;
    v1c->f_4 = 4;
    v1c->payAmount = 400;
    v1c->resaleAmount = 0x19;
    v1c->f_10 = 100;
    v1c->f_18 = -1;
    v1c->f_1c = 1;
    v1c->f_20 = 1;
    v1c->f_24 = 0;
    v1c->f_28 = 0;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    //Ion Scanner
    v1c = &obj_lib[17];
    v1c->f_0 = FILE1d5_BONUS16_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x02710;
    v1c->resaleAmount = 1;
    v1c->f_10 = 1;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 1;
    v1c->f_24 = 0;
    v1c->f_28 = 0;
    v1c->pays = 0;
    v1c->f_30 = 1;
    v1c->f_34 = 0;

    v1c = &obj_lib[18];
    v1c->f_0 = FILE1d5_BONUS16_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x16e68;
    v1c->resaleAmount = 0x16e68;
    v1c->f_10 = 0x16e68;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 1;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[19];
    v1c->f_0 = FILE1d6_BONUS17_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 76000;
    v1c->resaleAmount = 76000;
    v1c->f_10 = 76000;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 1;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[20];
    v1c->f_0 = FILE1d7_BONUS18_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0xd994;
    v1c->resaleAmount = 0xd994;
    v1c->f_10 = 0xd994;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 1;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[21];
    v1c->f_0 = FILE1d8_BONUS19_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x8980;
    v1c->resaleAmount = 0x8980;
    v1c->f_10 = 0x8980;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 1;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[22];
    v1c->f_0 = FILE1d9_BONUS20_PIC;
    v1c->f_4 = 1;
    v1c->payAmount = 0x1de84;
    v1c->resaleAmount = 0x1de84;
    v1c->f_10 = 0x1de84;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 1;
    v1c->f_30 = 1;
    v1c->f_34 = 1;

    v1c = &obj_lib[23];
    v1c->f_0 = FILE1db_BONUS22_PIC;
    v1c->f_4 = 4;
    v1c->payAmount = 0x32;
    v1c->resaleAmount = 0x32;
    v1c->f_10 = 0x32;
    v1c->f_18 = -1;
    v1c->f_1c = 0;
    v1c->f_20 = 0;
    v1c->f_24 = 1;
    v1c->f_28 = 0;
    v1c->pays = 1;
    v1c->f_30 = 1;
    v1c->f_34 = 0;
}


void OBJS_DisplayStats(void)
{
    int v1c;
    int v20, v24, v28;
    static int dpos;
    if (p_objs[17])
    {
        v20 = ENEMY_GetBaseDamage();
        if (v20 > 0)
        {
            GFX_ColorBox(0x6d, 0xbf, 0x66, 8, 0x4a);
            GFX_ColorBox(0x6e, 0xc0, v20, 6, 0x44);
        }
        else
        {
            GFX_VLine(dpos + 0x6e, 0xbe, 3, 0x44);
            GFX_VLine(0xd1 - dpos, 0xbe, 3, 0x44);
            dpos++;
            dpos %= 50;
        }
    }
    if (player.currentWeapon != -1)
    {
        v1c = obj_lib[player.currentWeapon].f_0;
        GFX_PutSprite((texture_t*)GLB_GetItem(v1c), 0x11e, 2);
    }
    if (p_objs[15])
    {
        v24 = 18;
        v28 = OBJS_GetTotal(15);
        for (v20 = 0; v20 < v28; v20++)
        {
            GFX_PutSprite((texture_t*)GLB_GetItem(FILE1e0_SMSHIELD_PIC), v24, 1);
            v24 += 13;
        }
    }
    if (p_objs[11])
    {
        v24 = 18;
        for (v20 = 0; v20 < p_objs[11]->f_8; v20++)
        {
            GFX_PutSprite((texture_t*)GLB_GetItem(FILE1df_SMBOMB_PIC), v24, 0xba);
            v24 += 13;
        }
    }
}

int OBJS_Equip(int a1)
{
    object_t *v1c;
    for (v1c = first_objs.f_4; &last_objs != v1c; v1c = v1c->f_4)
    {
        if (v1c->f_c == a1 && !p_objs[a1])
        {
            v1c->f_14 = 1;
            p_objs[a1] = v1c;
            return 1;
        }
    }
    return 0;
}

int OBJS_Load(object_t *a1)
{
    object_t *v1c;

    v1c = OBJS_Get();
    if (!v1c)
        return 0;

    v1c->f_8 = a1->f_8;
    v1c->f_c = a1->f_c;
    v1c->f_10 = &obj_lib[a1->f_c];
    v1c->f_14 = a1->f_14;
    if (v1c->f_14)
        p_objs[a1->f_c] = v1c;
    return 1;
}

int OBJS_Add(int a1)
{
    objlib_t *v20;
    object_t *v1c;
    if (a1 >= 24)
        return 3;
    g_oldsuper = -1;
    g_oldshield = -1;
    v20 = &obj_lib[a1];
    if (v20->pays)
    {
        player.money += v20->payAmount;
        return 0;
    }
    if (!reg_flag && !v20->f_30)
        return 0;
    if (v20->f_20)
    {
        for (v1c = first_objs.f_4; &last_objs != v1c; v1c = v1c->f_4)
        {
            if (v1c->f_c == a1)
            {
                if (v1c->f_8 >= v20->f_10)
                    return 2;
                v1c->f_8 += v20->resaleAmount;
                if (v1c->f_8 > v20->f_10)
                    v1c->f_8 = v20->f_10;
                return 0;
            }
        }
    }
    v1c = OBJS_Get();
    if (!v1c)
        return 2;
    v1c->f_8 = v20->resaleAmount;
    v1c->f_c = a1;
    v1c->f_10 = v20;
    if (!p_objs[a1])
    {
        v1c->f_14 = 1;
        p_objs[a1] = v1c;
        if (player.currentWeapon == -1 && v20->f_28)
        {
            player.currentWeapon = a1;
        }
    }
    return 0;
}

void OBJS_Del(int a1)
{
    object_t *v1c;
    v1c = p_objs[a1];
    if (v1c)
    {
        OBJS_Remove(v1c);
        p_objs[a1] = NULL;
        OBJS_Equip(a1);
        if (a1 == player.currentWeapon)
            OBJS_GetNext();
    }
}

void OBJS_GetNext(void)
{
    int v1c, v20, v24;
    object_t *v28;

    v24 = -1;
    if (player.currentWeapon < 3)
        v20 = 3;
    else
        v20 = player.currentWeapon + 1;
    for (v1c = 3; v1c <= 14; v1c++)
    {
        if (v20 > 14)
            v20 = 3;
        v28 = p_objs[v20];
        if (v28 && v28->f_8 && v28->f_10->f_28)
        {
            v24 = v20;
            break;
        }
        v20++;
    }
    player.currentWeapon = v24;
}

void OBJS_Use(int a1)
{
    object_t *v1c;
    objlib_t *v20;

    v1c = p_objs[a1];
    v20 = &obj_lib[a1];
    if (v1c)
    {
        objuse_flag = 1;
        think_cnt = 0;
        if (v20->f_14(a1) && !v20->f_1c)
        {
            v1c->f_8--;
        }
        if (v1c->f_8 <= 0 && !v20->f_1c)
        {
            OBJS_Remove(v1c);
            p_objs[a1] = NULL;
            OBJS_Equip(a1);
            if (player.currentWeapon == a1)
                OBJS_GetNext();
        }
    }
}

int OBJS_Sell(int a1)
{
    object_t *v1c;
    objlib_t *v20;
    int v24;
    v1c = p_objs[a1];
    v20 = &obj_lib[a1];
    if (!v1c)
        return 0;
    player.money += OBJS_GetResale(a1);
    if (a1 == 17)
    {
        p_objs[a1] = NULL;
        return 0;
    }
    if (v20->f_20)
    {
        v1c->f_8 -= v20->resaleAmount;
        if (v1c->f_8 <= 0)
        {
            v24 = 0;
            v1c->f_8 = 0;
            if (!v20->f_1c)
            {
                OBJS_Remove(v1c);
                p_objs[a1] = NULL;
                OBJS_Equip(a1);
                if (player.currentWeapon == a1)
                    OBJS_GetNext();
            }
        }
        else
            v24 = v1c->f_8;
    }
    else
    {
        OBJS_Del(a1);
        v24 = OBJS_GetTotal(a1);
    }
    return v24;
}

int OBJS_Buy(unsigned int a1)
{
    int v1c, v20;

    v1c = 1;
    if (a1 == 15)
    {
        v20 = OBJS_GetTotal(15);
        if (v20 >= 5)
            return 2;
    }
    if ((unsigned int)OBJS_GetCost(a1) <= player.money)
    {
        v1c = OBJS_Add(a1);
        if (!v1c)
            player.money -= OBJS_GetCost(a1);
    }
    return v1c;
}

int FUN_0001b169(int a1, int a2)
{
    object_t* v18;

    v18 = p_objs[a1];
    if (!v18)
        return 0;
    v18->f_8 -= a2;
    if (v18->f_8 < 0)
        v18->f_8 = 0;
    return v18->f_8;
}

int OBJS_GetAmt(int a1)
{
    object_t* v1c;

    v1c = p_objs[a1];
    if (!v1c)
        return 0;
    return v1c->f_8;
}

int OBJS_GetTotal(int a1)
{
    int v20;
    object_t *v1c;

    v20 = 0;
    for (v1c = first_objs.f_4; &last_objs != v1c; v1c = v1c->f_4)
    {
        if (a1 == v1c->f_c)
            v20++;
    }
    return v20;
}

int OBJS_IsOnly(int a1)
{
    objlib_t *v1c;

    v1c = &obj_lib[a1];
    return v1c->f_20;
}

int OBJS_GetCost(int a1)
{
    objlib_t* v1c;
    int v20;

    v1c = &obj_lib[a1];
    if (!v1c)
        return 99999999;
    if (v1c->f_20)
        v20 = v1c->payAmount * v1c->resaleAmount;
    else
        v20 = v1c->payAmount;
    return v20;
}

int OBJS_GetResale(int a1)
{
    object_t *v1c;
    objlib_t* v20;
    int v24;

    v1c = p_objs[a1];
    v20 = &obj_lib[a1];
    if (!v1c)
        return 0;
    if (v20->f_20)
        v24 = v20->payAmount * v20->resaleAmount;
    else
        v24 = v20->payAmount;
    return v24 >> 1;
}

int OBJS_CanBuy(int a1)
{
    int v20;
    objlib_t *v1c;
    v1c = &obj_lib[a1];
    if (a1 >= 24)
        return 0;
    if (a1 == 0 && OBJS_IsEquip(a1))
        return 0;

    if (!reg_flag && !v1c->f_30)
        return 0;

    v20 = OBJS_GetCost(a1);
    if (!v20)
        return 0;
    return 1;
}

int OBJS_CanSell(int a1)
{
    object_t *v1c;
    objlib_t* v20;
    v1c = p_objs[a1];
    v20 = &obj_lib[a1];
    if (a1 >= 24)
        return 0;
    if (!v1c)
        return 0;
    if (v20->f_20 && a1 == 16 && v1c->f_8 <= v20->resaleAmount)
        return 0;
    if (v1c->f_8 < v20->resaleAmount)
        return 0;
    return 1;
}

int OBJS_GetNum(void)
{
    return obj_cnt;
}

objlib_t* OBJS_GetLib(int a1)
{
    objlib_t *v1c;
    v1c = &obj_lib[a1];
    return v1c;
}

int OBJS_IsEquip(int a1)
{
    //if (p_objs[a1])
    if (a1 >= 0) if (p_objs[a1])
     return 1;
    return 0;
}

int OBJS_SubEnergy(int a1)
{
    object_t* v1c;

    if (godmode)
        return 0;
    if (startendwave != -1)
        return 0;

    v1c = p_objs[15];
    if (curplr_diff == 0 && a1 > 1)
        a1 >>= 1;
    if (v1c)
    {
        ANIMS_StartAnim(20, 0, 0);
        SND_Patch(32, 127);
        v1c->f_8 -= a1;
        if (v1c->f_8 < 0)
            OBJS_Del(15);
    }
    else
    {
        v1c = p_objs[16];
        if (!v1c)
            return 0;
        SND_Patch(33, 127);
        v1c->f_8 -= a1;
        if (v1c->f_8 < 0)
            v1c->f_8 = 0;
    }
    return v1c->f_8;
}

int OBJS_AddEnergy(int a1)
{
    object_t *v1c;
    v1c = p_objs[16];
    if (!v1c)
        return 0;
    if (v1c->f_8 < v1c->f_10->f_10)
    {
        v1c = p_objs[16];
        if (!v1c)
            return 0;
        v1c->f_8 += a1;
        if (v1c->f_8 > v1c->f_10->f_10)
            v1c->f_8 = v1c->f_10->f_10;
    }
    else
    {
        v1c = p_objs[15];
        if (!v1c)
            return 0;
        if (!v1c->f_8)
            return 0;
        v1c->f_8 += (a1 >> 2);
        if (v1c->f_8 > v1c->f_10->f_10)
            v1c->f_8 = v1c->f_10->f_10;
    }
    return v1c->f_8;
}

int OBJS_LoseObj(void)
{
    int v24, v20;
    objlib_t *v1c;
    v24 = 1;
    if (player.currentWeapon == -1)
    {
        for (v20 = 23; v20 >= 0; v20--)
        {
            v1c = &obj_lib[v20];
            if (p_objs[v20] && v1c->f_24)
            {
                OBJS_Del(v20);
                v24 = 1;
                break;
            }
        }
    }
    else
    {
        OBJS_Del(player.currentWeapon);
        v24 = 1;
    }
    return v24;
}

void OBJS_Think(void)
{
    if (curplr_diff < 3)
    {
        if (objuse_flag)
            objuse_flag = 0;
        else
        {
            think_cnt++;
            if (think_cnt > 96)
            {
                if (startendwave == -1)
                    OBJS_AddEnergy(1);
                think_cnt = 0;
            }
        }
    }
}

int OBJS_MakeSpecial(int a1)
{
    object_t *v1c;
    objlib_t *v20;

    v1c = p_objs[a1];
    v20 = &obj_lib[a1];
    if (a1 >= 24)
        return 0;
    if (!v1c)
        return 0;
    if (!v20->f_28)
        return 0;
    player.currentWeapon = a1;
    return 1;
}

