#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "common.h"
#include "loadsave.h"
#include "rap.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "objects.h"
#include "windows.h"
#include "fx.h"
#include "kbdapi.h"
#include "swdapi.h"
#include "store.h"
#include "enemy.h"
#include "bonus.h"
#include "tile.h"
#include "anims.h"
#include "joyapi.h"

#ifdef _WIN32
#include <io.h>
#endif // _WIN32
#ifdef __linux__
#include <sys/io.h>
#endif // __linux__
#ifdef __GNUC__
#include <unistd.h>
#endif
#ifdef _MSC_VER
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif

char g_data_path[PATH_MAX];
char g_setup_path[PATH_MAX];

int hasdatapath = 0;

int filepos = -1;
int map_item = -1;
int curplr_diff = 2;

static const char *fmt = "CHAR%04u.FIL";
static const char *fmt2 = "CHAR%04u.FIL";

map_t *mapmem;
csprite_t *csprite;
char *ml;

void RAP_SetPlayerDiff(void)
{
    cur_diff = 0;
    curplr_diff = plr.f_40[cur_game];
    switch (curplr_diff)
    {
    case 0:
        cur_diff |= 8;
        break;
    case 1:
        cur_diff |= 8;
        break;
    default:
    case 2:
        cur_diff |= 8;
        cur_diff |= 16;
        break;
    case 3:
        cur_diff |= 8;
        cur_diff |= 16;
        cur_diff |= 32;
        break;
    }
}

void RAP_ClearPlayer(void)
{
    OBJS_Clear();
    filepos = -1;
    memset(&plr, 0, sizeof(plr));
    plr.f_28 = -1;
    plr.f_40[0] = 2;
    plr.f_40[1] = 2;
    plr.f_40[2] = 2;
    plr.f_40[3] = 2;
    plr.f_54 = 0;
    cur_game = 0;
    memset(game_wave, 0, sizeof(game_wave));
}

int RAP_IsPlayer(void)
{
    if (filepos != -1)
        return 1;
    return 0;
}

int RAP_AreSavedFiles(void)
{
    char v4c[PATH_MAX];
    int v1c;
    for (v1c = 0; v1c < 10; v1c++)
    {
        if (hasdatapath)
            sprintf(v4c, fmt2, g_data_path, v1c);
        else
            sprintf(v4c, fmt, v1c);
        if (!access(v4c, 0))
            return 1;
    }
    return 0;
}

int RAP_ReadFile(const char *a1, void *a2, int a3)
{
    FILE *v14;
    v14 = fopen(a1, "rb");
    if (!v14)
    {
        WIN_Msg("File open Error");
        return 0;
    }
    fread(a2, 1, a3, v14);
    GLB_DeCrypt(gdmodestr, a2, a3);
    fclose(v14);
    return a3;
}

int RAP_FFSaveFile(void)
{
    char v50[PATH_MAX];
    int v1c, v20;
    v1c = 0;
    filepos = -1;
    for (v20 = 0; v20 < 10; v20++)
    {
        if (hasdatapath)
            sprintf(v50, fmt2, g_data_path, v20);
        else
            sprintf(v50, fmt, v20);
        if (access(v50, 0) != 0)
        {
            RAP_ClearPlayer();
            filepos = v20;
            v1c = 1;
            break;
        }
    }
    return v1c;
}

int RAP_IsSaveFile(player_t *a1)
{
    player_t vb0;
    char v58[PATH_MAX];
    int v1c, v24;
    FILE *v20;
    v1c = 0;
    for (v24 = 0; v24 < 10; v24++)
    {
        if (hasdatapath)
            sprintf(v58, fmt2, g_data_path, v24);
        else
            sprintf(v58, fmt, v24);
        v20 = fopen(v58, "rb");
        if (v20)
        {
            fread(&vb0, 1, sizeof(vb0), v20);
            fclose(v20);
            if (!strcmp(vb0.f_0, a1->f_0) && !strcmp(vb0.f_14, a1->f_14))
            {
                v1c = 1;
                break;
            }
        }
    }
    return v1c;
}

int RAP_LoadPlayer(void)
{
    char v6c[PATH_MAX];
    int v1c, v24;
    FILE *v20;
    object_t v40;

    v1c = 0;
    if (filepos == -1)
        return 0;
    OBJS_Clear();
    memset(&plr, 0, sizeof(plr));
    if (hasdatapath)
        sprintf(v6c, fmt2, g_data_path, filepos);
    else
        sprintf(v6c, fmt, filepos);
    v20 = fopen(v6c, "rb");
    if (!v20)
    {
        WIN_Msg("Load Player Error");
        return 0;
    }
    fread(&plr, 1, sizeof(plr), v20);
    GLB_DeCrypt(gdmodestr, &plr, sizeof(plr));
    for (v24 = 0; v24 < plr.f_3c; v24++)
    {
        fread(&v40, 1, sizeof(v40), v20);
        GLB_DeCrypt(gdmodestr, &v40, sizeof(v40));
        if (!OBJS_Load(&v40))
            break;
    }
    fclose(v20);
    cur_game = plr.f_2c;
    game_wave[0] = plr.f_30[0];
    game_wave[1] = plr.f_30[1];
    game_wave[2] = plr.f_30[2];
    if (!OBJS_IsEquip(plr.f_28))
        OBJS_GetNext();
    if (OBJS_GetAmt(16) <= 0)
        EXIT_Error("RAP_LoadPLayer() - Loaded DEAD player");
    v1c = 1;
    RAP_SetPlayerDiff();
    return v1c;
}

int RAP_SavePlayer(void)
{
    int v1c;
    char v5c[PATH_MAX];
    FILE *v20;
    object_t *v28;

    v1c = 0;
    if (filepos == -1)
        EXIT_Error("RAP_Save() ERR: Try to Save Invalid Player");
    if (OBJS_GetAmt(16) <= 0)
        EXIT_Error("RAP_Save() ERR: Try to save Dead player");
    if (hasdatapath)
        sprintf(v5c, fmt2, g_data_path, filepos);
    else
        sprintf(v5c, fmt, filepos);

    v20 = fopen(v5c, "wb");
    if (!v20)
    {
        WIN_Msg("Save Player Error !!!");
        return 0;
    }
    plr.f_2c = cur_game;
    plr.f_30[0] = game_wave[0];
    plr.f_30[1] = game_wave[1];
    plr.f_30[2] = game_wave[2];
    plr.f_3c = 0;
    for (v28 = first_objs.f_4; &last_objs != v28; v28 = v28->f_4)
    {
        plr.f_3c++;
    }
    GLB_EnCrypt(gdmodestr, &plr, sizeof(plr));
    fwrite(&plr, 1, sizeof(plr), v20);
    GLB_DeCrypt(gdmodestr, &plr, sizeof(plr));
    for (v28 = first_objs.f_4; &last_objs != v28; v28 = v28->f_4)
    {
        GLB_EnCrypt(gdmodestr, v28, sizeof(object_t));
        fwrite(v28, 1, sizeof(object_t), v20);
        GLB_DeCrypt(gdmodestr, v28, sizeof(object_t));
    }
    v1c = 1;
    fclose(v20);
    return v1c;
}

void RAP_LoadMap(void)
{
    char v44[44];
    if (!gameflag[cur_game])
        EXIT_Error("Loading Invalid map game %d", cur_game);
    GLB_FreeAll();
    sprintf(v44, "MAP%uG%u_MAP", game_wave[cur_game] + 1, cur_game + 1);
    map_item = GLB_GetItemID(v44);
    if (map_item == -1)
        EXIT_Error("RAP_LoadMap() - Invalid MAP.(%s)", v44);
    ml = GLB_LockItem(map_item);
    mapmem = (map_t*)ml;
    csprite = (csprite_t*)(ml + 0x1524);
    ENEMY_LoadLib();
    SND_CacheGFX();
    BONUS_Init();
    WIN_SetLoadLevel(20);
    OBJS_CachePics();
    WIN_SetLoadLevel(0x3c);
    ENEMY_LoadSprites();
    WIN_SetLoadLevel(0x50);
    TILE_CacheLevel();
    WIN_SetLoadLevel(100);
    WIN_EndLoad();
}

void RAP_FreeMap(void)
{
    if (map_item != -1)
    {
        TILE_FreeLevel();
        ENEMY_FreeSprites();
        ANIMS_FreePics();
        OBJS_FreePics();
        SND_FreeFX();
        GLB_FreeItem(map_item);
        map_item = -1;
    }
    GLB_FreeAll();
    SND_CacheIFX();
}

int RAP_LoadWin(void)
{
    char v254[10][PATH_MAX];
    char v68[PATH_MAX];
    int v30, v20, v24, v3c, v38, v1c, v2c, v28;
    player_t v108;
    wdlg_t vb0;
    v30 = 1;
    v20 = -1;
    v24 = -2;
    v3c = 0;
    v38 = 0;
    memset(v254, 0, sizeof(v254));
    for (v1c = 0; v1c < 10; v1c++)
    {
        if (hasdatapath)
            sprintf(v68, fmt2, g_data_path, v1c);
        else
            sprintf(v68, fmt, v1c);
        if (!access(v68, 0))
        {
            if (v20 == -1)
                v20= v1c;
            strncpy(v254[v1c], v68, PATH_MAX);
        }
    }
    if (v20 == -1)
        return-1;
    RAP_ReadFile(v254[v20], &v108, sizeof(v108));
    KBD_Clear();
    v2c = SWD_InitWindow(0x10039);
    SWD_SetActiveField(v2c, 5);
    SND_Patch(20, 127);
    while (1)
    {
        SWD_Dialog(&vb0);
        switch (XButton)                                                                                //Input Erase Savestate
        {
        case 1:
            JOY_IsKey(XButton);
            vb0.f_10 = 0x53;
            break;
        }
        if (KBD_IsKey(1) || JOY_IsKey(Back) || JOY_IsKey(BButton))                                      //Abort Load Window
        {
            v38 = 0;
            goto LAB_00022ecd;
        }
        if (keyboard[45] && keyboard[56])
            WIN_AskExit();
        if (v30)
        {
            v30 = 0;
            if (v20 != v24)
            {
                if (v20 < v24)
                    v28 = -1;
                else
                    v28 = 1;
                if (v20 >= 0)
                    v20 %= 10;
                else
                    v20 += 10;
                if (v20 < 0)
                    EXIT_Error("Help");
                v3c = 0;
                for (v1c = 0; v1c < 10; v1c++)
                {
                    if (v254[v20][0] == 0)
                    {
                        v20 += v28;
                        if (v20 >= 0)
                            v20 %= 10;
                        else
                            v20 += 10;
                    }
                    else
                    {
                        v3c = 1;
                        break;
                    }
                }
                if (v3c == 0)
                {
                    v38 = -1;
                    goto LAB_00022ecd;
                }
                RAP_ReadFile(v254[v20], &v108, sizeof(v108));
                v24 = v20;
            }
            SWD_SetFieldItem(v2c, 1, id_pics[v108.f_20]);
            SWD_SetFieldText(v2c, 9, v108.f_0);
            SWD_SetFieldText(v2c, 10, v108.f_14);
            sprintf(v68, "%07u", v108.f_24);
            SWD_SetFieldText(v2c, 11, v68);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            SND_Patch(20, 127);
            }
        switch (vb0.f_10)
        {
        case 0x50:
        case 0x51:
        case 0x4b:
            vb0.f_8 = 1;
            vb0.f_c = 10;
            vb0.f_4 = 2;
            break;
        case 0x48:
        case 0x49:
        case 0x4d:
            vb0.f_8 = 1;
            vb0.f_c = 10;
            vb0.f_4 = 3;
            break;
        case 0x53:
            vb0.f_8 = 1;
            vb0.f_c = 10;
            vb0.f_4 = 4;
            break;
        }
        if (vb0.f_8 == 1 && vb0.f_c == 10)
        {
            switch (vb0.f_4)
            {
            case 2:
                v20++;
                v30 = 1;
                break;
            case 3:
                v20--;
                v30 = 1;
                break;
            case 4:
                v30 = 1;
                sprintf(v68, "Delete Pilot %s ?", v108.f_14);
                if (WIN_AskBool(v68))
                {
                    remove(v254[v20]);
                    WIN_Msg("Pilot Removed !");
                    v254[v20][0] = 0;
                    v20++;
                }
                break;
            case 6:
                goto LAB_00022ecd;
            case 5:
                filepos = v20;
                RAP_LoadPlayer();
                v38 = 1;
                goto LAB_00022ecd;
            }
        }
    }
LAB_00022ecd:
    SWD_DestroyWindow(v2c);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    return v38;
}

const char *RAP_DataPath(void)
{
    memset(g_data_path, 0, sizeof(g_data_path));
    hasdatapath = 0;
    strcpy(g_setup_path, "setup.ini");
    return g_data_path;
}

const char *RAP_GetSetupPath(void)
{
    return g_setup_path;
}
