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
#include "input.h"
#include "fileids.h"
#include "winids.h"

#ifdef _WIN32
#include <io.h>
#endif // _WIN32
#ifdef __GNUC__
#include <unistd.h>
#endif // __GNUC__
#ifdef _MSC_VER
#include <windows.h>
#define PATH_MAX MAX_PATH
#endif // _MSC_VER

#define MAX_SAVE  10

char cdpath[PATH_MAX];
char g_setup_ini[PATH_MAX];

int cdflag = 0;

int filepos = -1;
int map_item = -1;
int curplr_diff = 2;

static const char *fmt = "CHAR%04u.FIL";
static const char* cdfmt = "%s\\CHAR%04u.FIL";

map_t *mapmem;
csprite_t *csprite;
char *ml;

/***************************************************************************
RAP_SetPlayerDiff () - Set Player Difficulty
 ***************************************************************************/
void 
RAP_SetPlayerDiff(
    void
)
{
    cur_diff = 0;
    
    curplr_diff = plr.diff[cur_game];
    
    switch (curplr_diff)
    {
    case DIFF_0:
        cur_diff |= EB_EASY_LEVEL;
        break;
    
    case DIFF_1:
        cur_diff |= EB_EASY_LEVEL;
        break;
    default:
    case DIFF_2:
        cur_diff |= EB_EASY_LEVEL;
        cur_diff |= EB_MED_LEVEL;
        break;
    
    case DIFF_3:
        cur_diff |= EB_EASY_LEVEL;
        cur_diff |= EB_MED_LEVEL;
        cur_diff |= EB_HARD_LEVEL;
        break;
    }
}

/***************************************************************************
RAP_ClearPlayer () - Clear Player stuff
 ***************************************************************************/
void 
RAP_ClearPlayer(
    void
)
{
    OBJS_Clear();
    filepos = -1;
    memset(&plr, 0, sizeof(plr));
    plr.sweapon = -1;
    plr.diff[0] = DIFF_2;
    plr.diff[1] = DIFF_2;
    plr.diff[2] = DIFF_2;
    plr.diff[3] = DIFF_2;
    plr.fintrain = 0;
    cur_game = 0;
    memset(game_wave, 0, sizeof(game_wave));
}

/***************************************************************************
RAP_IsPlayer () - Returns TRUE if a player is defined
 ***************************************************************************/
int 
RAP_IsPlayer(
    void
)
{
    if (filepos != -1)
        return 1;
    
    return 0;
}

/***************************************************************************
RAP_AreSavedFiles() - Returns TRUE if thier are previously saved game files
 ***************************************************************************/
int 
RAP_AreSavedFiles(
    void
)
{
    char temp[PATH_MAX];
    int loop;
    
    for (loop = 0; loop < MAX_SAVE; loop++)
    {
        if (cdflag)
            sprintf(temp, cdfmt, cdpath, loop);
        else
            sprintf(temp, fmt, loop);
        
        if (!access(temp, 0))
            return 1;
    }
    
    return 0;
}

/***************************************************************************
RAP_ReadFile() - Reads file into buffer for sizerec and DECRYTES
 ***************************************************************************/
int                                 // RETURN: size of record
RAP_ReadFile(
    const char *name,               // INPUT : filename
    void *buffer,                   // OUTPUT: pointer to buffer
    int sizerec                     // INPUT : number of bytes to read
)
{
    FILE *handle;
    handle = fopen(name, "rb");
    
    if (!handle)
    {
        WIN_Msg("File open Error");
        return 0;
    }
    
    fread(buffer, 1, sizerec, handle);
    
    GLB_DeCrypt(gdmodestr, buffer, sizerec);
    
    fclose(handle);
    
    return sizerec;
}

/***************************************************************************
RAP_FFSaveFile() - Finds a filename to use
 ***************************************************************************/
int 
RAP_FFSaveFile(
    void
)
{
    char temp[PATH_MAX];
    int rval, loop;
    rval = 0;
    
    filepos = -1;
    
    for (loop = 0; loop < MAX_SAVE; loop++)
    {
        if (cdflag)
            sprintf(temp, cdfmt, cdpath, loop);
        else
            sprintf(temp, fmt, loop);
        
        if (access(temp, 0) != 0)
        {
            RAP_ClearPlayer();
            filepos = loop;
            rval = 1;
            break;
        }
    }
    
    return rval;
}

/***************************************************************************
RAP_IsSaveFile() - Returns True if thier is a sopt to save a character
 ***************************************************************************/
int 
RAP_IsSaveFile(
    player_t *in_plr
)
{
    player_t tp;
    char temp[PATH_MAX];
    int rval, loop;
    FILE *handle;
    rval = 0;
    
    for (loop = 0; loop < MAX_SAVE; loop++)
    {
        if (cdflag)
            sprintf(temp, cdfmt, cdpath, loop);
        else
            sprintf(temp, fmt, loop);
        
        handle = fopen(temp, "rb");
        
        if (handle)
        {
            fread(&tp, 1, sizeof(tp), handle);
            fclose(handle);
            if (!strcmp(tp.name, in_plr->name) && !strcmp(tp.callsign, in_plr->callsign))
            {
                rval = 1;
                break;
            }
        }
    }
    
    return rval;
}

/***************************************************************************
RAP_LoadPlayer () - Loads player from disk
 ***************************************************************************/
int 
RAP_LoadPlayer(
    void
)
{
    char filename[PATH_MAX];
    int rval, loop;
    FILE *handle;
    object_t inobj;

    rval = 0;
    
    if (filepos == -1)
        return 0;
    
    // == Clear Player =======================
    OBJS_Clear();
    memset(&plr, 0, sizeof(plr));
    
    if (cdflag)
        sprintf(filename, cdfmt, cdpath, filepos);
    else
        sprintf(filename, fmt, filepos);
    
    handle = fopen(filename, "rb");
    
    if (!handle)
    {
        WIN_Msg("Load Player Error");
        return 0;
    }
    
    fread(&plr, 1, sizeof(plr), handle);
    GLB_DeCrypt(gdmodestr, &plr, sizeof(plr));
    
    for (loop = 0; loop < plr.numobjs; loop++)
    {
        fread(&inobj, 1, sizeof(inobj), handle);
        GLB_DeCrypt(gdmodestr, &inobj, sizeof(inobj));
        
        if (!OBJS_Load(&inobj))
            break;
    }
    
    fclose(handle);
    
    cur_game = plr.cur_game;
    game_wave[0] = plr.game_wave[0];
    game_wave[1] = plr.game_wave[1];
    game_wave[2] = plr.game_wave[2];
    
    if (!OBJS_IsEquip(plr.sweapon))
        OBJS_GetNext();
    
    if (OBJS_GetAmt(S_ENERGY) <= 0)
        EXIT_Error("RAP_LoadPLayer() - Loaded DEAD player");
    
    rval = 1;
    RAP_SetPlayerDiff();
    
    return rval;
}

/***************************************************************************
RAP_SavePlayer() - Saves player data to filename
 ***************************************************************************/
int 
RAP_SavePlayer(
    void
)
{
    int rval;
    char filename[PATH_MAX];
    FILE *handle;
    object_t *cur;

    rval = 0;
    
    if (filepos == -1)
        EXIT_Error("RAP_Save() ERR: Try to Save Invalid Player");
    
    if (OBJS_GetAmt(S_ENERGY) <= 0)
        EXIT_Error("RAP_Save() ERR: Try to save Dead player");
    
    if (cdflag)
        sprintf(filename, cdfmt, cdpath, filepos);
    else
        sprintf(filename, fmt, filepos);

    handle = fopen(filename, "wb");
    
    if (!handle)
    {
        WIN_Msg("Save Player Error !!!");
        return 0;
    }
    
    plr.cur_game = cur_game;
    plr.game_wave[0] = game_wave[0];
    plr.game_wave[1] = game_wave[1];
    plr.game_wave[2] = game_wave[2];
    plr.numobjs = 0;
    
    for (cur = first_objs.next; &last_objs != cur; cur = cur->next)
    {
        plr.numobjs++;
    }
    
    GLB_EnCrypt(gdmodestr, &plr, sizeof(plr));
    fwrite(&plr, 1, sizeof(plr), handle);
    GLB_DeCrypt(gdmodestr, &plr, sizeof(plr));
    
    for (cur = first_objs.next; &last_objs != cur; cur = cur->next)
    {
        GLB_EnCrypt(gdmodestr, cur, sizeof(object_t));
        fwrite(cur, 1, sizeof(object_t), handle);
        GLB_DeCrypt(gdmodestr, cur, sizeof(object_t));
    }
    
    rval = 1;
    
    fclose(handle);
    
    return rval;
}

/***************************************************************************
 RAP_LoadMap () - Loads A level Map
 ***************************************************************************/
void 
RAP_LoadMap(
    void
)
{
    char temp[44];
    
    if (!gameflag[cur_game])
        EXIT_Error("Loading Invalid map game %d", cur_game);
    
    GLB_FreeAll();
    
    sprintf(temp, "MAP%uG%u_MAP", game_wave[cur_game] + 1, cur_game + 1);
    map_item = GLB_GetItemID(temp);
    
    if (map_item == -1)
        EXIT_Error("RAP_LoadMap() - Invalid MAP.(%s)", temp);
    
    ml = GLB_LockItem(map_item);
    
    mapmem = (map_t*)ml;
    csprite = (csprite_t*)(ml + sizeof(map_t));

    ENEMY_LoadLib();
    SND_CacheGFX();
    BONUS_Init();
    WIN_SetLoadLevel(20);
    OBJS_CachePics();
    WIN_SetLoadLevel(40);
    ANIMS_CachePics();
    WIN_SetLoadLevel(60);
    ENEMY_LoadSprites();
    WIN_SetLoadLevel(80);
    TILE_CacheLevel();
    WIN_SetLoadLevel(100);
    WIN_EndLoad();
}

/***************************************************************************
RAP_FreeMap() - Frees up cached map stuff
 ***************************************************************************/
void 
RAP_FreeMap(
    void
)
{
    if (map_item != -1)
    {
        TILE_FreeLevel();
        ENEMY_FreeSprites();
        ANIMS_FreePics();
        OBJS_FreePics();
        SND_FreeFX();
        
        // FREE MAP ========================
        
        GLB_FreeItem(map_item);
        
        map_item = -1;
    }
    
    GLB_FreeAll();
    SND_CacheIFX();
}

/***************************************************************************
RAP_LoadWin() -
 ***************************************************************************/
int                        // RETURN : -1 = no FIles, 0=cancel, 1=loaded
RAP_LoadWin(
    void
)
{
    char filenames[MAX_SAVE][PATH_MAX];
    char temp[PATH_MAX];
    int update, pos, oldpos, fndflag, rval, loop, window, addnum;
    player_t tplr;
    wdlg_t dlg;
    update = 1;
    pos = -1;
    oldpos = -2;
    fndflag = 0;
    rval = 0;
    
    memset(filenames, 0, sizeof(filenames));
    for (loop = 0; loop < MAX_SAVE; loop++)
    {
        if (cdflag)
            sprintf(temp, cdfmt, cdpath, loop);
        else
            sprintf(temp, fmt, loop);
        
        if (!access(temp, 0))
        {
            if (pos == -1)
                pos = loop;
            strncpy(filenames[loop], temp, PATH_MAX);
        }
    }
    
    if (pos == -1)
        return-1;
    
    RAP_ReadFile(filenames[pos], &tplr, sizeof(tplr));
    KBD_Clear();
    window = SWD_InitWindow(FILE139_LOAD_SWD);
    SWD_SetActiveField(window, LOAD_LOAD);
    SND_Patch(FX_SWEP, 127);
    
    while (1)
    {
        SWD_Dialog(&dlg);
        
        if (joy_ipt_MenuNew)
        {
            if (XButton)                                                                                                                        
            {
                JOY_IsKey(XButton);
                dlg.keypress = SC_DELETE;
            }
        }
        
        if ((KBD_IsKey(SC_ESC)) || (JOY_IsKeyMenu(Back) && joy_ipt_MenuNew) || (JOY_IsKeyMenu(BButton) && joy_ipt_MenuNew))                                      
        {
            rval = 0;
            goto load_exit;
        }
        
        if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
            WIN_AskExit();
        
        if (update)
        {
            update = 0;
            if (pos != oldpos)
            {
                if (pos < oldpos)
                    addnum = -1;
                else
                    addnum = 1;
                
                if (pos >= 0)
                    pos %= MAX_SAVE;
                else
                    pos += MAX_SAVE;
                
                if (pos < 0)
                    EXIT_Error("Help");
                
                fndflag = 0;
                for (loop = 0; loop < MAX_SAVE; loop++)
                {
                    if (filenames[pos][0] == 0)
                    {
                        pos += addnum;
                        
                        if (pos >= 0)
                            pos %= MAX_SAVE;
                        else
                            pos += MAX_SAVE;
                    }
                    else
                    {
                        fndflag = 1;
                        break;
                    }
                }
                
                if (fndflag == 0)
                {
                    rval = -1;
                    goto load_exit;
                }
                
                RAP_ReadFile(filenames[pos], &tplr, sizeof(tplr));
                oldpos = pos;
            }
            
            SWD_SetFieldItem(window, LOAD_IDPIC, id_pics[tplr.id_pic]);
            SWD_SetFieldText(window, LOAD_NAME, tplr.name);
            SWD_SetFieldText(window, LOAD_CALL, tplr.callsign);
            sprintf(temp, "%07u", tplr.score);
            SWD_SetFieldText(window, LOAD_CREDITS, temp);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            SND_Patch(FX_SWEP, 127);
            }
        
        switch (dlg.keypress)
        {
        case SC_LEFT:
        case SC_PAGEDN:
        case SC_DOWN:
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = LOAD_NEXT;
            break;
        
        case SC_RIGHT:
        case SC_PAGEUP:
        case SC_UP:
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = LOAD_PREV;
            break;
        
        case SC_DELETE:
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = LOAD_DEL;
            break;

        case SC_ENTER:
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = LOAD_LOAD;
            break;
        }
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            switch (dlg.field)
            {
            case LOAD_NEXT:
                pos++;
                update = 1;
                break;
            
            case LOAD_PREV:
                pos--;
                update = 1;
                break;
            
            case LOAD_DEL:
                update = 1;
                sprintf(temp, "Delete Pilot %s ?", tplr.callsign);
                if (WIN_AskBool(temp))
                {
                    remove(filenames[pos]);
                    WIN_Msg("Pilot Removed !");
                    filenames[pos][0] = 0;
                    pos++;
                }
                break;
            
            case LOAD_CANCEL:
                goto load_exit;
            
            case LOAD_LOAD:
                filepos = pos;
                RAP_LoadPlayer();
                rval = 1;
                goto load_exit;
            }
        }
    }

load_exit:
    
    SWD_DestroyWindow(window);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    return rval;
}

/***************************************************************************
RAP_InitLoadSave() - Inits the load and save path stuff
 ***************************************************************************/
const char*
RAP_InitLoadSave(
    void
)
{
    memset(cdpath, 0, sizeof(cdpath));
    
    cdflag = 0;
    
    strcpy(g_setup_ini, "SETUP.INI");
    
    return cdpath;
}

/***************************************************************************
RAP_SetupFilename() - Gets current setup.ini path and name
 ***************************************************************************/
const char*
RAP_SetupFilename(
    void
)
{
    return g_setup_ini;
}
