#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "SDL_filesystem.h"
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
#include "prefapi.h"
#include "i_video.h"

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
int srwpos = 0;

static const char *fmt = "CHAR%04u.FIL";
static const char *cdfmt = "%sCHAR%04u.FIL";

MAZELEVEL *mapmem;
CSPRITE *csprite;
char *ml;
char *savebuffer;

/***************************************************************************
SaveRead8 () - Reads 8 bit unsigned char from save file buffer
 ***************************************************************************/
static unsigned char
SaveRead8(
    void
)
{
    unsigned char data = -1;

    memcpy(&data, &savebuffer[srwpos++], 1);

    return data;
}

/***************************************************************************
SaveWrite8 () - Writes 8 bit unsigned char to save file buffer
 ***************************************************************************/
static void
SaveWrite8(
    unsigned char data
)
{
    memcpy(&savebuffer[srwpos++], &data, 1);
}

/***************************************************************************
SaveRead32 () - Reads 32 bit int from save file buffer
 ***************************************************************************/
static int
SaveRead32(
    void
)
{
    int convert;

    convert = SaveRead8();
    convert |= SaveRead8() << 8;
    convert |= SaveRead8() << 16;
    convert |= SaveRead8() << 24;

    return convert;
}

/***************************************************************************
SaveWrite32 () - Writes 32 bit int to save file buffer
 ***************************************************************************/
static void
SaveWrite32(
    int convert
)
{
    SaveWrite8(convert & 0xff);
    SaveWrite8((convert >> 8) & 0xff);
    SaveWrite8((convert >> 16) & 0xff);
    SaveWrite8((convert >> 24) & 0xff);
}

/***************************************************************************
SaveReadPointer () - Reads a 32 bit pointer regardless of the architecture
 ***************************************************************************/
static void*
SaveReadPointer(
    void
)
{
    return (void*)(intptr_t)SaveRead32();
}

/***************************************************************************
SaveWritePointer () - Writes a 32 bit pointer regardless of the architecture
 ***************************************************************************/
static void
SaveWritePointer(
    const void *pointer
)
{
    SaveWrite32((intptr_t)pointer);
}

/***************************************************************************
ReadPlayerExt () - Reads external Player Structure regardless of architecture
 ***************************************************************************/
static void
ReadPlayerExt(
    void
)
{
    int i;

    for (i = 0; i < 20; ++i)
        plr.name[i] = SaveRead8();

    for (i = 0; i < 12; ++i)
        plr.callsign[i] = SaveRead8();

    plr.id_pic = SaveRead32();
    plr.score = SaveRead32();
    plr.sweapon = SaveRead32();
    plr.cur_game = SaveRead32();

    for (i = 0; i < 3; ++i)
        plr.game_wave[i] = SaveRead32();

    plr.numobjs = SaveRead32();

    for (i = 0; i < 4; ++i)
        plr.diff[i] = SaveRead32();

    plr.trainflag = SaveRead32();

    plr.fintrain = SaveRead32();
}

/***************************************************************************
ReadPlayer () - Reads Player Structure regardless of architecture
 ***************************************************************************/
static void
ReadPlayer(
    PLAYEROBJ *tplr
)
{
    int i;

    for (i = 0; i < 20; ++i)
        tplr->name[i] = SaveRead8();

    for (i = 0; i < 12; ++i)
        tplr->callsign[i] = SaveRead8();

    tplr->id_pic = SaveRead32();
    tplr->score = SaveRead32();
    tplr->sweapon = SaveRead32();
    tplr->cur_game = SaveRead32();

    for (i = 0; i < 3; ++i)
        tplr->game_wave[i] = SaveRead32();

    tplr->numobjs = SaveRead32();

    for (i = 0; i < 4; ++i)
        tplr->diff[i] = SaveRead32();

    tplr->trainflag = SaveRead32();

    tplr->fintrain = SaveRead32();
}

/***************************************************************************
WritePlayerExt () - Writes external Player Structure regardless of architecture
 ***************************************************************************/
static void
WritePlayerExt(
    void
)
{
    int i;

    for (i = 0; i < 20; ++i)
        SaveWrite8(plr.name[i]);

    for (i = 0; i < 12; ++i)
        SaveWrite8(plr.callsign[i]);

    SaveWrite32(plr.id_pic);
    SaveWrite32(plr.score);
    SaveWrite32(plr.sweapon);
    SaveWrite32(plr.cur_game);

    for (i = 0; i < 3; ++i)
        SaveWrite32(plr.game_wave[i]);

    SaveWrite32(plr.numobjs);

    for (i = 0; i < 4; ++i)
        SaveWrite32(plr.diff[i]);

    SaveWrite32(plr.trainflag);

    SaveWrite32(plr.fintrain);
}

/***************************************************************************
ReadObject () - Reads Object Structure regardless of architecture
 ***************************************************************************/
static void
ReadObject(
    OBJ *inobj
)
{
    inobj->prev = (OBJ*)SaveReadPointer();
    inobj->next = (OBJ*)SaveReadPointer();

    inobj->num = SaveRead32();
    inobj->type = SaveRead32();

    inobj->lib = (OBJ_LIB*)SaveReadPointer();

    inobj->inuse = SaveRead32();
}

/***************************************************************************
WriteObject () - Writes Object Structure regardless of architecture
 ***************************************************************************/
static void
WriteObject(
    OBJ *inobj
)
{
    SaveWritePointer(inobj->prev);
    SaveWritePointer(inobj->next);

    SaveWrite32(inobj->num);
    SaveWrite32(inobj->type);

    SaveWritePointer(inobj->lib);

    SaveWrite32(inobj->inuse);
}

/***************************************************************************
SaveResetReadWritePosition () - Resets read/write position in save file buffer
 ***************************************************************************/
static void
SaveResetReadWritePosition(
    void
)
{
    srwpos = 0;
}

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
    
    savebuffer = (char*)malloc(sizerec);
    fread(savebuffer, 1, sizerec, handle);
    
    GLB_DeCrypt(gdmodestr, savebuffer, sizerec);
    ReadPlayer((PLAYEROBJ*)buffer);
    
    fclose(handle);
    free(savebuffer);
    SaveResetReadWritePosition();
    
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
    PLAYEROBJ *in_plr
)
{
    PLAYEROBJ tp;
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
            savebuffer = (char*)malloc(sizeof(tp));
            fread(savebuffer, 1, sizeof(tp), handle);
            
            //GLB_DeCrypt(gdmodestr, savebuffer, sizeof(tp)); //missing in v1.2
            
            ReadPlayer(&tp);
            
            fclose(handle);
            free(savebuffer);
            SaveResetReadWritePosition();
            
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
    char *dchrplr;
    char *dchrobj;
    int size;
    int rval, loop;
    FILE *handle;
    OBJ inobj;

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
    
    fseek(handle, 0, SEEK_END);
    size = ftell(handle);
    fseek(handle, 0, SEEK_SET);
    
    dchrplr = (char*)malloc(sizeof(plr));
    savebuffer = (char*)malloc(size);
    dchrobj = (char*)malloc(size - sizeof(plr));
    
    fread(dchrplr, 1, sizeof(plr), handle);
    fseek(handle, sizeof(plr), SEEK_SET);
    
    GLB_DeCrypt(gdmodestr, dchrplr, sizeof(plr));
    
    memcpy(savebuffer, dchrplr, sizeof(plr));
    fread(dchrobj, 1, size - sizeof(plr), handle);
    
    GLB_DeCrypt(gdmodestr, dchrobj, size - sizeof(plr));
    
    memcpy(savebuffer + sizeof(plr), dchrobj, size - sizeof(plr));
    
    free(dchrplr);
    free(dchrobj);

    ReadPlayerExt();
    
    for (loop = 0; loop < plr.numobjs; loop++)
    {
        ReadObject(&inobj);
       
        if (!OBJS_Load(&inobj))
            break;
    }
    
    fclose(handle);
    free(savebuffer);
    SaveResetReadWritePosition();
    
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
    int size;
    char filename[PATH_MAX];
    char *echrplr;
    char *echrobj;
    FILE *handle;
    OBJ *cur;

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
    
    size = plr.numobjs * 24 + sizeof(plr);
    
    echrplr = (char*)malloc(sizeof(plr));
    savebuffer = (char*)malloc(size);
    echrobj = (char*)malloc(size - sizeof(plr));

    WritePlayerExt();
    
    memcpy(echrplr, savebuffer, sizeof(plr));
    
    GLB_EnCrypt(gdmodestr, echrplr, sizeof(plr));
   
    for (cur = first_objs.next; &last_objs != cur; cur = cur->next)
    {
        WriteObject(cur);
    }
    
    memcpy(echrobj, savebuffer + sizeof(plr), size - sizeof(plr));
    
    GLB_EnCrypt(gdmodestr, echrobj, size - sizeof(plr));
    
    memcpy(savebuffer, echrplr, sizeof(plr));
    memcpy(savebuffer + sizeof(plr), echrobj, size - sizeof(plr));
    
    free(echrplr);
    free(echrobj);

    fwrite(savebuffer, 1, size, handle);

    rval = 1;
    
    fclose(handle);
    free(savebuffer);
    SaveResetReadWritePosition();
    
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
    
    mapmem = (MAZELEVEL*)ml;
    csprite = (CSPRITE*)(ml + sizeof(MAZELEVEL));

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
    PLAYEROBJ tplr;
    SWD_DLG dlg;
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
        I_GetNeedResize(false);
        
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
#if _WIN32 || __linux__ || __APPLE__
    char* gethome;

    #if __ANDROID__
    gethome = (char*)SDL_AndroidGetExternalStoragePath();
    strcat(gethome, "/");
    #else
    gethome = SDL_GetPrefPath("", "Raptor");
    #endif //__ANDROID__

    if (gethome != NULL)
    {
        strcpy(cdpath, gethome);
        strcpy(g_setup_ini, gethome);
        
        if(RAP_CheckFileInPath("setup.ini"))
            sprintf(g_setup_ini, "%s%s", g_setup_ini, "setup.ini");
        else
            sprintf(g_setup_ini, "%s%s", g_setup_ini, "SETUP.INI");
        
        cdflag = 1;
        SDL_free(gethome);
    }
    else
    {
        EXIT_Error("Couldn't find home directory");
    }

    #if __ANDROID__
    for(int i = 0; i < 2; i++)
    {
        char src[PATH_MAX];
        char srclc[PATH_MAX];
        char dst[PATH_MAX];
        sprintf(src,"FILE%04u.GLB", i);
        sprintf(srclc, "file%04u.glb", i);
        sprintf(dst, "%sFILE%04u.GLB", cdpath, i);
        if (!RAP_CheckFileInPath(src) && !RAP_CheckFileInPath(srclc))
        {
            SDL_RWops *readsrc = SDL_RWFromFile(src, "rb");
            if(readsrc)
            {
                Sint64 size = SDL_RWsize(readsrc);
                char *buffer = (char*)malloc(size);
                SDL_RWread(readsrc, buffer, sizeof(char), size);
                SDL_RWops *writedst = SDL_RWFromFile(dst, "wb");
                if(writedst)
                {
                    SDL_RWwrite(writedst, buffer, sizeof(char), size);
                    SDL_RWclose(writedst);
                }
                SDL_RWclose(readsrc);
                free(buffer);
            }
        }
    }
    #endif //__ANDROID__

    return cdpath;
#else
    memset(cdpath, 0, sizeof(cdpath));

    cdflag = 0;

    if(!access("setup.ini", 0))
        strcpy(g_setup_ini, "setup.ini");
    else
        strcpy(g_setup_ini, "SETUP.INI");

    return cdpath;
#endif // _WIN32 || __linux__ || __APPLE__
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

/***************************************************************************
RAP_WriteDefaultSetup() - Writes default setup.ini
 ***************************************************************************/
void 
RAP_WriteDefaultSetup(
    void
)
{
    INI_PutPreferenceLong("Setup", "Detail", 1);
#if __ANDROID__
    INI_PutPreferenceLong("Setup", "Control", 1);
#else
    INI_PutPreferenceLong("Setup", "Control", 0);
#endif //__ANDROID__
    INI_PutPreferenceLong("Setup", "Haptic", 1);                           
    INI_PutPreferenceLong("Setup", "joy_ipt_MenuNew", 0);         

#if _WIN32 || __APPLE__
    INI_PutPreferenceLong("Setup", "sys_midi", 1);
#else
    INI_PutPreferenceLong("Setup", "sys_midi", 0);
#endif // _WIN32 __APPLE__
 
    INI_PutPreferenceLong("Setup", "winmm_mpu_device", 0);       
    INI_PutPreferenceLong("Setup", "core_dls_synth", 1);           
    INI_PutPreferenceLong("Setup", "core_midi_port", 0);           
    INI_PutPreferenceLong("Setup", "alsa_output_client", 128);           
    INI_PutPreferenceLong("Setup", "alsa_output_port", 0);               
    INI_PutPreference("Setup", "SoundFont", "SoundFont.sf2");
    INI_PutPreferenceLong("Music", "Volume", 85);

#if _WIN32 || __APPLE__
    INI_PutPreferenceLong("Music", "CardType", 8);
    INI_PutPreferenceLong("Music", "MidiPort", 330);
#else
    INI_PutPreferenceLong("Music", "CardType", 5);
    INI_PutPreferenceLong("Music", "BasePort", 220);
    INI_PutPreferenceLong("Music", "Irq", 7);
    INI_PutPreferenceLong("Music", "Dma", 1);
#endif // _WIN32 __APPLE__

    INI_PutPreferenceLong("SoundFX", "Volume", 85);
    INI_PutPreferenceLong("SoundFX", "CardType", 5);
    INI_PutPreferenceLong("SoundFX", "BasePort", 220);
    INI_PutPreferenceLong("SoundFX", "Irq", 7);
    INI_PutPreferenceLong("SoundFX", "Dma", 1);
    INI_PutPreferenceLong("SoundFX", "Channels", 4);
    INI_PutPreferenceLong("Keyboard", "MoveUp", 72);
    INI_PutPreferenceLong("Keyboard", "MoveDn", 80);
    INI_PutPreferenceLong("Keyboard", "MoveLeft", 75);
    INI_PutPreferenceLong("Keyboard", "MoveRight", 77);
    INI_PutPreferenceLong("Keyboard", "Fire", 29);
    INI_PutPreferenceLong("Keyboard", "FireSp", 56);
    INI_PutPreferenceLong("Keyboard", "ChangeSp", 57);
    INI_PutPreferenceLong("Keyboard", "MegaFire", 54);
    INI_PutPreferenceLong("Mouse", "Fire", 0);
    INI_PutPreferenceLong("Mouse", "FireSp", 1);
    INI_PutPreferenceLong("Mouse", "ChangeSp", 2);
    INI_PutPreferenceLong("JoyStick", "Fire", 0);
    INI_PutPreferenceLong("JoyStick", "FireSp", 1);
    INI_PutPreferenceLong("JoyStick", "ChangeSp", 2);
    INI_PutPreferenceLong("JoyStick", "MegaFire", 3);
#if __ANDROID__
    INI_PutPreferenceLong("Video", "fullscreen", 1);
    INI_PutPreferenceLong("Video", "aspect_ratio_correct", 0);
    INI_PutPreferenceLong("Video", "txt_fullscreen", 1);
#else
    INI_PutPreferenceLong("Video", "fullscreen", 0);
    INI_PutPreferenceLong("Video", "aspect_ratio_correct", 1);
    INI_PutPreferenceLong("Video", "txt_fullscreen", 0);
#endif //__ANDROID__
}

/***************************************************************************
RAP_GetPath() - Gets external path
 ***************************************************************************/
const char*
RAP_GetPath(
    void 
)
{
    return cdpath;
}

/***************************************************************************
RAP_CheckFileInPath() - Checks whether external path contains file
 ***************************************************************************/
int
RAP_CheckFileInPath(
    const char* filename
)
{
    char buffer[PATH_MAX];

    sprintf(buffer, "%s%s", cdpath, filename);

    if (!access(buffer, 0))
        return 1;
    else
        return 0;
}