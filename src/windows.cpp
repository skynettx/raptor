#include <string.h>
#include <stdio.h>
#include "common.h"
#include "glbapi.h"
#include "gfxapi.h"
#include "swdapi.h"
#include "imsapi.h"
#include "ptrapi.h"
#include "rap.h"
#include "fx.h"
#include "i_video.h"
#include "kbdapi.h"
#include "input.h"
#include "musapi.h"
#include "prefapi.h"
#include "help.h"
#include "loadsave.h"
#include "objects.h"
#include "intro.h"
#include "store.h"
#include "demo.h"
#include "joyapi.h"
#include "enemy.h"
#include "windows.h"
#include "fileids.h"
#include "winids.h"

#define HANGAR_MISSION   0
#define HANGAR_SUPPLIES  1
#define HANGAR_EXITDOS   2
#define HANGAR_NONE      3

#define MUSIC_VOL 0
#define FX_VOL 1

int d_count;
int hangto;
int opt_detail;

int opt_vol[2] = { 127, 127 };
int opt_window;

char hangtext[4][18] = {
    "FLY MISSION",
    "SUPPLY ROOM",
    "EXIT HANGAR",
    "SAVE PILOT",
};

char regtext[3][30] = {
    "ENTER NAME AND CALLSIGN",
    "   CHANGE ID PICTURE",
    "        EXIT"
};

int sid_pics[4] = {
    FILE116_WMALEID_PIC,
    FILE119_BMALEID_PIC,
    FILE117_WFMALEID_PIC,
    FILE118_BFMALEID_PIC
};

int songsg1[] = {
    FILE060_RAP8_MUS,       // WAVE 1
    FILE05a_RAP2_MUS,       // WAVE 2
    FILE05c_RAP4_MUS,       // WAVE 3
    FILE05f_RAP7_MUS,       // WAVE 4
    FILE05e_RAP6_MUS,       // WAVE 5
    FILE05a_RAP2_MUS,       // WAVE 6
    FILE05b_RAP3_MUS,       // WAVE 7
    FILE05c_RAP4_MUS,       // WAVE 8
    FILE05e_RAP6_MUS        // WAVE 9
};

int songsg2[] = {
    FILE05b_RAP3_MUS,       // WAVE 1
    FILE05a_RAP2_MUS,       // WAVE 2
    FILE05c_RAP4_MUS,       // WAVE 3
    FILE060_RAP8_MUS,       // WAVE 4
    FILE05e_RAP6_MUS,       // WAVE 5
    FILE05a_RAP2_MUS,       // WAVE 6
    FILE060_RAP8_MUS,       // WAVE 7
    FILE059_RAP1_MUS,       // WAVE 8
    FILE05e_RAP6_MUS        // WAVE 9
};

int songsg3[] = {
    FILE060_RAP8_MUS,       // WAVE 1
    FILE05c_RAP4_MUS,       // WAVE 2
    FILE059_RAP1_MUS,       // WAVE 3
    FILE05f_RAP7_MUS,       // WAVE 4
    FILE05e_RAP6_MUS,       // WAVE 5
    FILE05a_RAP2_MUS,       // WAVE 6
    FILE05b_RAP3_MUS,       // WAVE 7
    FILE05c_RAP4_MUS,       // WAVE 8
    FILE05e_RAP6_MUS        // WAVE 9
};

#define HANGTOSTORE    0
#define HANGTOMISSION  1

#define DEMO_DELAY ( 800 * 5 )

int g_x, g_y, g_lx, g_ly;
int diff_wrap[E_NUM_DIFF] = { 4, 9, 9, 9 };

/***************************************************************************
WIN_WinGame () - Window text for winners of a game
 ***************************************************************************/
void 
WIN_WinGame(
    int game               // INPUT : game number 0,1,2
)
{
    int window;
    int dtext[4] = {
        FILE00a_END1_TXT, 
        FILE00b_END2_TXT,
        FILE00c_END3_TXT,
        FILE009_END0_TXT
    };
    
    if (game > 3)
        return;

    GFX_FadeOut(0, 0, 0, 2);
    
    window = SWD_InitWindow(FILE13e_WINGAME_SWD);
    SWD_SetFieldItem(window, WIN_TEXT, dtext[game]);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 16);
    
    IMS_WaitTimed(30);
    
    SWD_DestroyWindow(window);
    GFX_DisplayUpdate();
}

/***************************************************************************
WIN_Msg () - Display a Message for ten secs or until user Acks something
 ***************************************************************************/
void 
WIN_Msg(
    const char *msg        // INPUT : pointer to message to ask
)
{
    int window;

    window = SWD_InitWindow(FILE13a_MSG_SWD);
    
    SWD_SetFieldText(window, INFO_MSG, msg);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    SND_Patch(FX_SWEP, 127);
    
    IMS_WaitTimed(10);
    
    SWD_DestroyWindow(window);
    GFX_DisplayUpdate();
    KBD_Clear();
}

/***************************************************************************
WIN_OptDraw() -
 ***************************************************************************/
void 
WIN_OptDraw(
    wdlg_t *dlg
)
{
    int x, y, lx, ly;
    
    if (!dlg)
        return;
    
    SWD_GetFieldXYL(opt_window, OPTS_VMUSIC, &x, &y, &lx, &ly);
    GFX_PutSprite((texture_t*)GLB_GetItem(FILE127_SLIDE_PIC), x + opt_vol[MUSIC_VOL] - 2, y);
    
    SWD_GetFieldXYL(opt_window, OPTS_VFX, &x, &y, &lx, &ly);
    GFX_PutSprite((texture_t*)GLB_GetItem(FILE127_SLIDE_PIC), x + opt_vol[FX_VOL] - 2, y);
}

/***************************************************************************
WIN_Opts() - Sets Game Options
 ***************************************************************************/
void 
WIN_Opts(
    void
)
{
    wdlg_t dlg;
    int x, y, lx, ly;
    int kbactive, patchflag, curd, cur_field;
    int new_vol;
    int fpics[3] = {
        OPTS_PIC1, OPTS_PIC2, OPTS_PIC3
    };
    char detail[2][16] = {
        "LOW DETAIL",
        "HIGH DETAIL"
    };

    kbactive = 0;
    patchflag = 0;
    curd = opt_detail;
    cur_field = 0;

    opt_vol[MUSIC_VOL] = music_volume;
    opt_vol[FX_VOL] = fx_volume;
    
    opt_window = SWD_InitWindow(FILE13f_OPTS_SWD);
    
    SWD_SetWindowPtr(opt_window);
    SWD_SetFieldText(opt_window, OPTS_DETAIL, detail[curd]);
    SWD_SetWinDrawFunc(opt_window, WIN_OptDraw);
    
    SWD_SetFieldItem(opt_window, OPTS_PIC1, -1);
    SWD_SetFieldItem(opt_window, OPTS_PIC2, -1);
    SWD_SetFieldItem(opt_window, OPTS_PIC3, -1);
    
    SWD_ShowAllWindows();
    
    SND_Patch(FX_SWEP, 127);
    GFX_DisplayUpdate();

    while (1)
    {
        patchflag = 0;
        SWD_Dialog(&dlg);

        if (joy_ipt_MenuNew)
        {
            if (StickY > 0 || Down)                                                   
            {
                if (JOY_IsScroll(0) == 1)
                    dlg.keypress = SC_DOWN;
            }
            
            if (StickY < 0 || Up)
            {
                if (JOY_IsScroll(0) == 1)
                    dlg.keypress = SC_UP;
            }
            
            if (StickX > 0 || Right)
            {
                if (JOY_IsScroll(0) == 1)
                    dlg.keypress = SC_RIGHT;
            }
            
            if (StickX < 0 || Left)
            {
                if (JOY_IsScroll(0) == 1)
                    dlg.keypress = SC_LEFT;
            }
            
            if (Back)
            {
                dlg.keypress = SC_ESC;
                JOY_IsKey(Back);
            }
            
            if (BButton)
            {
                dlg.keypress = SC_ESC;
                JOY_IsKey(BButton);
            }
            
            if (AButton)
            {
                dlg.keypress = SC_ENTER;
                JOY_IsKey(AButton);
            }
        }
        
        switch (dlg.keypress)
        {
        case SC_ESC:
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = OPTS_EXIT;
            break;
        
        case SC_LEFT:
            if (cur_field)
            {
                if (cur_field == 2)
                    patchflag = 1;
                opt_vol[cur_field - 1] -= 8;
                if (opt_vol[cur_field - 1] < 0)
                    opt_vol[cur_field - 1] = 0;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            break;
        
        case SC_RIGHT:
            if (cur_field)
            {
                if (cur_field == 2)
                    patchflag = 1;
                opt_vol[cur_field - 1] += 8;
                if (opt_vol[cur_field - 1] > 127)
                    opt_vol[cur_field - 1] = 127;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            break;
        
        case SC_UP:
            if (kbactive && cur_field > 0)
                cur_field--;
            kbactive = 1;
            SND_Patch(FX_SWEP, 127);
            SWD_SetFieldItem(opt_window, OPTS_PIC1, -1);
            SWD_SetFieldItem(opt_window, OPTS_PIC2, -1);
            SWD_SetFieldItem(opt_window, OPTS_PIC3, -1);
            SWD_SetFieldItem(opt_window, fpics[cur_field], FILE128_POINT_PIC);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            break;
        
        case SC_DOWN:
            if (kbactive && cur_field < 2)
                cur_field++;
            kbactive = 1;
            SND_Patch(FX_SWEP, 127);
            SWD_SetFieldItem(opt_window, OPTS_PIC1, -1);
            SWD_SetFieldItem(opt_window, OPTS_PIC2, -1);
            SWD_SetFieldItem(opt_window, OPTS_PIC3, -1);
            SWD_SetFieldItem(opt_window, fpics[cur_field], FILE128_POINT_PIC);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            break;
        
        case SC_ENTER:
            if (!cur_field)
            {
                dlg.cur_act = S_FLD_COMMAND;
                dlg.cur_cmd = F_SELECT;
                dlg.field = OPTS_DETAIL;
            }
            break;
        }
        
        if (dlg.viewactive)
        {
            switch (dlg.sfield)
            {
            case OPTS_VMUSIC:
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                  
                {
                    while (!IMS_IsAck())
                    {
                    }
                    SWD_GetFieldXYL(opt_window, OPTS_VMUSIC, &x, &y, &lx, &ly);
                    new_vol = cur_mx - x;
                    if (new_vol < 0)
                        new_vol = 0;
                    if (new_vol > 127)
                        new_vol = 127;
                    opt_vol[MUSIC_VOL] = new_vol;
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            
            case OPTS_VFX:
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                 
                {
                    while (!IMS_IsAck())
                    {
                    }
                    SWD_GetFieldXYL(opt_window, OPTS_VFX, &x, &y, &lx, &ly);
                    new_vol = cur_mx - x;
                    if (new_vol < 0)
                        new_vol = 0;
                    if (new_vol > 127)
                        new_vol = 127;
                    opt_vol[FX_VOL] = new_vol;
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            }
        }
        
        opt_detail = curd;
        
        if (fx_volume != opt_vol[FX_VOL])
            fx_volume = opt_vol[FX_VOL];
        
        if (music_volume != opt_vol[MUSIC_VOL])
        {
            music_volume = opt_vol[MUSIC_VOL];
            MUS_SetVolume(music_volume);
        }
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            switch (dlg.field)
            {
            case OPTS_EXIT:
                if (opt_vol[MUSIC_VOL] >= 0 && opt_vol[MUSIC_VOL] < 128)
                {
                    music_volume = opt_vol[MUSIC_VOL];
                    INI_PutPreferenceLong("Music", "Volume", music_volume);
                }
                if (opt_vol[FX_VOL] >= 0 && opt_vol[FX_VOL] < 128)
                {
                    fx_volume = opt_vol[FX_VOL];
                    INI_PutPreferenceLong("SoundFX", "Volume", fx_volume);
                }
                INI_PutPreferenceLong("Setup", "Detail", opt_detail);
                SND_Patch(FX_SWEP, 127);
                goto exit_opts;
            
            case OPTS_DETAIL:
                SND_Patch(FX_SWEP, 127);
                curd ^= 1;
                SWD_SetFieldText(opt_window, OPTS_DETAIL, detail[curd]);
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                break;
            }
        }
        
        if (patchflag)
            SND_Patch(FX_SWEP, 127);
    }

exit_opts:
    
    SWD_SetWinDrawFunc(opt_window, NULL);
    
    SWD_DestroyWindow(opt_window);
    GFX_DisplayUpdate();
    KBD_Clear();
    
    return;
}

/***************************************************************************
WIN_Pause () - Display a Pause Message Wait until user does something
 ***************************************************************************/
void 
WIN_Pause(
    void
)
{
    int window;

    window = SWD_InitWindow(FILE13a_MSG_SWD);
    
    SWD_SetFieldText(window, INFO_MSG, "GAME PAUSED");
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    SND_Patch(FX_SWEP, 127);
    
    while (!IMS_CheckAck())                                      
    {
    }
    
    SWD_DestroyWindow(window);
    GFX_DisplayUpdate();
    
    JOY_Wait(0);
    KBD_Clear();
    IMS_StartAck();
}

/***************************************************************************
WIN_Order () - Order Window
 ***************************************************************************/
void 
WIN_Order(
    void
)
{
    int window, dchold;

    dchold = g_drawcursor;
    
    if (GAME2 == 0 && GAME3 == 0)
    {
        PTR_DrawCursor(0);
        KBD_Clear();
        GFX_FadeOut(0, 0, 0, 2);
        
        window = SWD_InitWindow(FILE13b_ORDER_SWD);
        SWD_ShowAllWindows();
        GFX_DisplayUpdate();
        GFX_FadeIn(palette, 16);
        
        IMS_WaitTimed(15);
        
        GFX_FadeOut(0, 0, 0, 16);
        SWD_DestroyWindow(window);
        memset(displaybuffer, 0, 64000);
        GFX_DisplayUpdate();
        GFX_SetPalette(palette, 0);
        KBD_Clear();
        
        PTR_DrawCursor(dchold);
    }
}

/***************************************************************************
WIN_Credits () - Credits Window
 ***************************************************************************/
int 
WIN_Credits(
    void
)
{
    int window, rval;
    static int cnt = 1;
    int csng[3] = {
        FILE053_BOSS2_MUS, 
        FILE054_BOSS3_MUS, 
        FILE055_BOSS4_MUS
    };
    
    cnt++;
    
    if (cnt >= 3)
        cnt = 0;
    
    SND_PlaySong(csng[cnt], 1, 1);
    
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 16);
    
    window = SWD_InitWindow(FILE13c_CREDIT_SWD);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 16);
    
    rval = IMS_WaitTimed(25);
    
    GFX_FadeOut(0, 0, 0, 16);
    SWD_DestroyWindow(window);
    memset(displaybuffer, 0, 64000);
    memset(displayscreen, 0, 64000);
    GFX_SetPalette(palette, 0);
    KBD_Clear();
    
    return rval;
}

/***************************************************************************
   WIN_AskBool () - Askes USER a YES/NO Question????
 ***************************************************************************/
int                        // RETURN: TRUE/FALSE
WIN_AskBool(
    const char *question   // INPUT : pointer to message to ask
)
{
    int rval;
    int dchold;
    int ask_window;
    int px, py, lx, ly;
    wdlg_t dlg;

    rval = 0;
    dchold = g_drawcursor;
    
    KBD_Clear();
    ask_window = SWD_InitWindow(FILE135_ASK_SWD);
    
    SWD_SetFieldText(ask_window, ASK_DRAGBAR, question);
    SWD_SetActiveField(ask_window, ASK_YES);
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    SND_Patch(FX_SWEP, 127);
    
    PTR_DrawCursor(1);
    
    SWD_GetFieldXYL(ask_window, ASK_YES, &px, &py, &lx, &ly);
    PTR_SetPos(px + (lx >> 1), py + (ly >> 1));
    
    SWD_SetActiveField(ask_window, ASK_YES);

    while (1)
    {
        SWD_Dialog(&dlg);
        
        if (KBD_IsKey(SC_ESC) || JOY_IsKeyInGameBack(Back))                                                   
        {
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = ASK_NO;
        }
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            switch (dlg.field)
            {
            case ASK_YES:
                rval = 1;
                [[fallthrough]];
            case ASK_NO:
                SWD_DestroyWindow(ask_window);
                GFX_DisplayUpdate();
                PTR_DrawCursor(dchold);
                return rval;
            }
        }
    }
    
    return 0;
}

/***************************************************************************
WIN_AskExit () - Opens Windows and Askes if User wants 2 quit
 ***************************************************************************/
void 
WIN_AskExit(
    void
)
{
    if (WIN_AskBool("EXIT TO DOS"))
    {
        SND_FadeOutSong();
        
        switch (bday_num)
        {
        case 0:
            SND_Patch(FX_MON3, 127);
            while (SND_IsPatchPlaying(FX_MON2))
            {
            }
            break;
        
        case 1:
            SND_Patch(FX_DAVE, 127);
            while (SND_IsPatchPlaying(FX_DAVE))
            {
            }
            break;
        
        case 2:
            SND_Patch(FX_MON4, 127);
            while (SND_IsPatchPlaying(FX_MON4))
            {
            }
            break;
        
        case 3:
            SND_Patch(FX_MON1, 127);
            while (SND_IsPatchPlaying(FX_MON1))
            {
            }
            break;
        
        case 4:
            SND_Patch(FX_MON2, 127);
            while (SND_IsPatchPlaying(FX_MON2))
            {
            }
            SND_Patch(FX_MON2, 127);
            while (SND_IsPatchPlaying(FX_MON2))
            {
            }
            break;
        
        case 5:
            SND_Patch(FX_MON6, 127);
            while (SND_IsPatchPlaying(FX_MON6))
            {
            }
            break;
        }
        
        retraceflag = 1;
        GFX_FadeOut(60, 15, 2, 32);
        GFX_FadeOut(0, 0, 0, 6);
        ShutDown(0);
        EXIT_Clean();
    }
}

/***************************************************************************
WIN_AskDiff () - Difficulty Window
 ***************************************************************************/
int                         // RETURN -1=ABORT 0=TRAIN, 1=EASY, 2=MED, 3=HARD
WIN_AskDiff(
    void
)
{
    wdlg_t dlg;
    int rval, ask_window;
    int px, py, lx, ly;

    rval = -1;
    
    KBD_Clear();
    ask_window = SWD_InitWindow(FILE13d_ASKDIFF_SWD);
    SWD_SetActiveField(ask_window, OKREG_MED);
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    SWD_GetFieldXYL(ask_window, OKREG_MED, &px, &py, &lx, &ly);
    PTR_SetPos(px + (lx >> 1), py + (ly >> 1));
    
    while (1)
    {
        SWD_Dialog(&dlg);
        
        if (KBD_IsKey(SC_ESC) || Back || BButton)                                                      
        {
            rval = -1;
            goto askdiff_exit;
        }
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            switch (dlg.field)
            {
            case OKREG_TRAIN:
                rval = DIFF_0;
                WIN_Msg("TRAIN MODE plays 4 of 9 levels!");
                goto askdiff_exit;
            
            case OKREG_EASY:
                rval = DIFF_1;
                goto askdiff_exit;
            
            case OKREG_MED:
                rval = DIFF_2;
                goto askdiff_exit;
            
            case OKREG_HARD:
                rval = DIFF_3;
                goto askdiff_exit;
            
            case OKREG_ABORT:
                goto askdiff_exit;
            }
        }
    }

askdiff_exit:

    SWD_DestroyWindow(ask_window);
    GFX_DisplayUpdate();

    return rval;
}

/***************************************************************************
WIN_Register () - Register Window
 ***************************************************************************/
int 
WIN_Register(
    void
)
{
    wdlg_t dlg;
    int cur_id, opt, oldopt, rval, window, diff, loop;
    player_t tp;

    cur_id = 0;
    opt = -1;
    oldopt = -99;
    rval = 0;

    PTR_DrawCursor(0);

    SND_PlaySong(FILE058_HANGAR_MUS, 1, 1);
    
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 2);
    
    memset(&tp, 0, sizeof(tp));
    tp.sweapon = -1;
    tp.diff[0] = DIFF_2;
    tp.diff[1] = DIFF_2;
    tp.diff[2] = DIFF_2;
    tp.diff[3] = DIFF_2;
    tp.id_pic = 0;
    
    window = SWD_InitWindow(FILE137_REGISTER_SWD);
    SWD_SetFieldItem(window, REG_IDPIC, sid_pics[tp.id_pic]);
    SWD_SetActiveField(window, REG_NAME);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    GFX_FadeIn(palette, 16);
    
    SWD_SetFieldPtr(window, REG_VIEWID);
    PTR_DrawCursor(1);
    
    while (1)
    {
        SWD_Dialog(&dlg);

        if (joy_ipt_MenuNew)                                                               
        {
            if (LeftShoulder)                                           
            {
                JOY_IsKey(LeftShoulder);
                dlg.keypress = SC_CTRL;
            }
            
            if (RightShoulder)
            {
                JOY_IsKey(RightShoulder);
                dlg.keypress = SC_F1;
            }
        }
        
        if (KBD_Key(SC_ESC) || Back || BButton)
        {
            rval = 0;
            fi_joy_count = 0;
            fi_sec_field = false;
            goto reg_exit;
        }
        
        if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
            WIN_AskExit();

        switch (dlg.keypress)
        {
        case SC_F1:
            HELP_Win("NEWPLAY1_TXT");
            break;
        
        case SC_ALT:
        case SC_CTRL:
            cur_id++;
            cur_id %= 4;
            SWD_SetFieldItem(window, REG_IDPIC, sid_pics[cur_id]);
            tp.id_pic = cur_id;
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            break;
        }
        
        if (dlg.viewactive)
        {
            switch (dlg.sfield)
            {
            case REG_VIEWEXIT:
                opt = dlg.sfield;
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                       
                {
                    while (IMS_IsAck())
                    {
                    }
                    if (RAP_IsSaveFile(&tp))
                        WIN_Msg("Pilot NAME and CALLSIGN Used !");
                    else
                    {
                        rval = 1;
                        goto reg_exit;
                    }
                }
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, REG_TEXT, regtext[2]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            
            case REG_VIEWID:
                opt = dlg.sfield;
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                     
                {
                    while (IMS_IsAck())
                    {
                    }
                    cur_id++;
                    cur_id %= 4;
                    SWD_SetFieldItem(window, REG_IDPIC, sid_pics[cur_id]);
                    tp.id_pic = cur_id;
                }
                SWD_SetFieldText(window, REG_TEXT, regtext[1]);
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                oldopt = opt;
                break;
            
            case REG_VIEWREG:
                opt = dlg.sfield;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, REG_TEXT, regtext[0]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            }
        }
        else
        {
            opt = -1;
            
            if (opt != oldopt)
            {
                SWD_SetFieldText(window, REG_TEXT, " ");
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                oldopt = opt;
            }
        }
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            switch (dlg.field)
            {
            case REG_NAME:
                SWD_GetFieldText(window, REG_NAME, tp.name);
                if (strlen(tp.name) != 0 && dlg.keypress == SC_ENTER)
                {
                    fi_sec_field = true;
                    SWD_SetActiveField(window, REG_CALLSIGN);
                }
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                break;
            
            case REG_CALLSIGN:
                SWD_GetFieldText(window, REG_CALLSIGN, tp.callsign);
                SWD_GetFieldText(window, REG_CALLSIGN, tp.name);
                if (!strlen(tp.name))
                {
                    fi_sec_field = false;
                    SWD_SetActiveField(window, REG_NAME);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                else if (!strlen(tp.callsign))
                {
                    SWD_SetActiveField(window, REG_CALLSIGN);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                else if (dlg.keypress == SC_ENTER || KBD_Key(SC_ENTER))
                {
                    if (RAP_IsSaveFile(&tp))
                        WIN_Msg("Pilot NAME and CALLSIGN Used !");
                    else
                    {
                        fi_sec_field = false;
                        rval = 1;
                        goto reg_exit;
                    }
                }
                break;
            }
        }
    }

reg_exit:
    
    SWD_GetFieldText(window, REG_NAME, tp.name);
    SWD_GetFieldText(window, REG_CALLSIGN, tp.callsign);
    
    if (!strlen(tp.name))
    {
        SWD_SetActiveField(window, REG_NAME);
        rval = 0;
    }
    
    if (!strlen(tp.callsign))
    {
        SWD_SetActiveField(window, REG_CALLSIGN);
        rval = 0;
    }
    
    diff = 1;
    
    if (rval)
    {
        diff = WIN_AskDiff();
        
        if (diff >= 0)
        {
            ingameflag = 0;
            
            if (!RAP_FFSaveFile())
            {
                WIN_Msg("ERROR : YOU MUST DELETE A PILOT");
                rval = 0;
            }
        }
        else
        {
            WIN_Msg("PLAYER ABORTED!");
            rval = 0;
        }
    }
    
    if (rval)
    {
        ingameflag = 0;
        tp.diff[0] = diff;
        tp.diff[1] = diff;
        tp.diff[2] = diff;
        
        if (diff == DIFF_0)
        {
            tp.trainflag = 1;
            tp.fintrain = 0;
        }
        else
        {
            tp.trainflag = 0;
            tp.fintrain = 1;
        }
        
        memcpy(&plr, &tp, sizeof(player_t));
        RAP_SetPlayerDiff();
        OBJS_Add(S_FORWARD_GUNS);
        OBJS_Add(S_ENERGY);
        OBJS_Add(S_ENERGY);
        OBJS_Add(S_ENERGY);
        plr.score = 10000;
        
        if (godmode)
        {
            plr.score += 876543;
            OBJS_Add(S_MEGA_BOMB);
            OBJS_Add(S_MEGA_BOMB);
            OBJS_Add(S_MEGA_BOMB);
            OBJS_Add(S_MEGA_BOMB);
            OBJS_Add(S_DETECT);
            
            for (loop = 1; loop < S_ENERGY; loop++)
                OBJS_Add(loop);
        }
        
        OBJS_GetNext();
    }
    
    PTR_DrawCursor(0);
    GFX_FadeOut(0, 0, 0, 16);
    SWD_DestroyWindow(window);
    memset(displaybuffer, 0, 64000);
    
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    
    hangto = HANGTOSTORE;
    
    if (rval)
        ingameflag = 0;
    
    return rval;
}

/***************************************************************************
WIN_Hangar() - Does the hangar
 ***************************************************************************/
int 
WIN_Hangar(
    void
)
{
    char temp[44];
    wdlg_t dlg;
    int opt, oldopt, pos, kflag, local_cnt, pic_cnt, window;
    int x, y, lx, ly;
    int poslookup[4] = {
        HANG_MISSION, 
        HANG_SUPPLIES, 
        HANG_MAIN_MENU, 
        HANG_QSAVE
    };
    opt = HANG_SUPPLIES;
    oldopt = -1;
    pos = 0;
    kflag = 0;
    local_cnt = GFX_GetFrameCount();
    pic_cnt = 0;
    
    PTR_DrawCursor(0);
    
    KBD_Clear();

    if (plr.trainflag)
    {
        plr.trainflag = 0;
        opt = HANG_SUPPLIES;
    }
    else
    {
        GFX_FadeOut(0, 0, 0, 2);
        
        window = SWD_InitMasterWindow(FILE134_HANGAR_SWD);
        
        SWD_GetFieldItem(window, HANG_PIC);
        
        SND_PlaySong(FILE058_HANGAR_MUS, 1, 1);
        
        SWD_ShowAllWindows();
        GFX_DisplayUpdate();
        
        GFX_FadeIn(palette, 16);
        
        if (control != I_MOUSE)
            kflag = 1;
        
        switch (hangto)
        {
        case HANGTOMISSION:
            pos = 0;
            SWD_SetActiveField(window, poslookup[pos]);
            break;
        
        default:
        case HANGTOSTORE:
            pos = 1;
            SWD_SetActiveField(window, poslookup[pos]);
            break;
        }
        
        hangto = HANGTOMISSION;
        
        SWD_SetWindowPtr(window);
        PTR_DrawCursor(1);
        
        while (1)
        {
            pic_cnt++;
            
            if (pic_cnt > 4)
            {
                pic_cnt = 0;
                
                if ((wrand() % 3) == 0)
                    SWD_SetFieldItem(window, HANG_PIC, FILE10b_HANGP_PIC);
                else
                    SWD_SetFieldItem(window, HANG_PIC, -1);
                
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            else
            {
                local_cnt = GFX_GetFrameCount();
                while (GFX_GetFrameCount() == local_cnt)
                {
                }
            }
            
            SWD_Dialog(&dlg);

            if (joy_ipt_MenuNew)
            {
                if (StickY > 0 || Down)                                                   
                {
                    JOY_IsKey(StickY);
                    dlg.keypress = SC_DOWN;
                }
                
                if (StickY < 0 || Up)
                {
                    JOY_IsKey(StickY);
                    dlg.keypress = SC_UP;
                }
                
                if (StickX > 0 || Right)
                {
                    JOY_IsKey(StickX);
                    dlg.keypress = SC_RIGHT;
                }
                
                if (StickX < 0 || Left)
                {
                    JOY_IsKey(StickX);
                    dlg.keypress = SC_LEFT;
                }
                
                if (AButton)
                {
                    JOY_IsKey(AButton);
                    dlg.keypress = SC_ENTER;
                }
                
                if (RightShoulder)
                {
                    JOY_IsKey(RightShoulder);
                    dlg.keypress = SC_F1;
                }
            }
            
            if (KBD_Key(SC_ESC) || Back || BButton)
            {
                opt = -99;
                goto hangar_exit;
            }
            
            if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
                WIN_AskExit();
            
            switch (dlg.keypress)
            {
            case SC_F1:
                HELP_Win("HANGHLP1_TXT");
                break;
            
            case SC_S:
            case SC_F2:
                pos = 3;
                sprintf(temp, "Save %s - %s ?", plr.name, plr.callsign);
                if (WIN_AskBool(temp))
                    RAP_SavePlayer();
                break;
            
            case SC_TAB:
            case SC_UP:
            case SC_LEFT:
                kflag = 1;
                KBD_Wait(SC_UP);
                KBD_Wait(SC_LEFT);
                pos++;
                pos %= 4;
                break;

            case SC_RIGHT:
            case SC_DOWN:
                kflag = 1;
                KBD_Wait(SC_DOWN);
                KBD_Wait(SC_RIGHT);
                pos--;
                if (pos < 0)
                    pos = 3;
                break;
            
            case SC_ENTER:
            case SC_SPACE:
                KBD_Wait(SC_ENTER);
                KBD_Wait(SC_SPACE);
                opt = poslookup[pos];
                goto keyboard_part;
            }
            
            if (kflag)
            {
                kflag = 0;
                switch (pos)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                    opt = poslookup[pos];
                    if (opt != oldopt)
                    {
                        oldopt = opt;
                        SWD_GetFieldXYL(window, opt, &x, &y, &lx, &ly);
                        PTR_SetPos(x + (lx>>1), y + (ly>>1));
                        oldopt = -1;
                        dlg.sfield = opt;
                        dlg.viewactive = 1;
                    }
                    break;
                
                default:
                    pos = 0;
                    break;
                }
            }
            
            if (dlg.viewactive)
            {
keyboard_part:
                
                switch (dlg.sfield)
                {
                case HANG_MISSION:
                    pos = 0;
                    opt = dlg.sfield;
                    if ((mouseb1) || (dlg.keypress == SC_ENTER) || (AButton && !joy_ipt_MenuNew))                
                    {
                        SND_Patch(FX_DOOR, 60);
                        while (IMS_IsAck())
                        {
                        }
                        goto hangar_exit;
                    }
                    if (opt != oldopt)
                    {
                        SWD_SetFieldText(window, HANG_TEXT, hangtext[pos]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        oldopt = opt;
                    }
                    break;
                
                case HANG_SUPPLIES:
                    pos = 1;
                    opt = dlg.sfield;
                    if ((mouseb1) || (dlg.keypress == SC_ENTER) || (AButton && !joy_ipt_MenuNew))               
                    {
                        SND_Patch(FX_DOOR, 127);
                        while (IMS_IsAck())
                        {
                        }
                        goto hangar_exit;
                    }
                    if (opt != oldopt)
                    {
                        SWD_SetFieldText(window, HANG_TEXT, hangtext[pos]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        oldopt = opt;
                    }
                    break;
                
                case HANG_MAIN_MENU:
                    pos = 2;
                    opt = dlg.sfield;
                    if ((mouseb1) || (dlg.keypress == SC_ENTER) || (AButton && !joy_ipt_MenuNew))             
                    {
                        opt = -99;
                        SND_Patch(FX_DOOR, 200);
                        while (IMS_IsAck())
                        {
                        }
                        goto hangar_exit;
                    }
                    if (opt != oldopt)
                    {
                        SWD_SetFieldText(window, HANG_TEXT, hangtext[pos]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        oldopt = opt;
                    }
                    break;
                
                case HANG_QSAVE:
                    pos = 3;
                    opt = dlg.sfield;
                    if ((mouseb1) || (dlg.keypress == SC_ENTER) || (AButton && !joy_ipt_MenuNew))            
                    {
                        while (IMS_IsAck())
                        {
                        }
                        sprintf(temp, "Save %s - %s ?", plr.name, plr.callsign);
                        if (WIN_AskBool(temp))
                            RAP_SavePlayer();
                        while (IMS_IsAck())
                        {
                        }
                        break;
                    }
                    if (opt != oldopt)
                    {
                        SWD_SetFieldText(window, HANG_TEXT, hangtext[pos]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        oldopt = opt;
                    }
                    break;
                }
            }
            else
            {
                opt = -1;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, HANG_TEXT, " ");
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
            }
        }
    
    hangar_exit:
        
        PTR_DrawCursor(0);
        
        GFX_FadeOut(0, 0, 0, 16);
        
        SWD_DestroyWindow(window);
        memset(displaybuffer, 0, 64000);
        GFX_DisplayUpdate();
        GFX_SetPalette(palette, 0);
    }
    
    return opt;
}

/***************************************************************************
WIN_LoadComp() - Shows computer loading screen
 ***************************************************************************/
void 
WIN_LoadComp(
    void
)
{
    char temp[40];
    int window;
    char sect[3][15] = {
        "BRAVO SECTOR",
        "TANGO SECTOR",
        "OUTER REGIONS"
    };

    window = SWD_InitMasterWindow(FILE140_LOADCOMP_SWD);
    SWD_GetFieldXYL(window, LCOMP_LEVEL, &g_x, &g_y, &g_lx, &g_ly);
    
    sprintf(temp, "WAVE %d", game_wave[cur_game]);
    SWD_SetFieldText(window, LCOMP_WAVE, temp);
    
    SWD_SetFieldText(window, LCOMP_SECTOR, sect[cur_game]);
    
    if (diff_wrap[plr.diff[cur_game]] - 1 == game_wave[cur_game])
    {
        sprintf(temp, "FINAL WAVE %d", game_wave[cur_game] + 1);
        SWD_SetFieldText(window, LCOMP_WAVE, temp);
    }
    else
    {
        sprintf(temp, "WAVE %d", game_wave[cur_game] + 1);
        SWD_SetFieldText(window, LCOMP_WAVE, temp);
    }
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 8);
    
    SWD_DestroyWindow(window);
}

/***************************************************************************
WIN_ShipComp () - Does Game Selection 1, 2 or 3
 ***************************************************************************/
int 
WIN_ShipComp(
    void
)
{
    wdlg_t dlg;
    int rval, secret1, secret2, secret3, secret, cz1, cz2;
    int window;
    int px, py, lx, ly;

    rval = 1;
    secret1 = 0;
    secret2 = 0;
    secret3 = 0;
    secret = 0;

    cz1 = *ltable;
    cz2 = *dtable;
    
    GLB_FreeAll();
    
    PTR_DrawCursor(0);
    
    ltable[0] = 0;
    dtable[0] = 0;
    
    SND_PlaySong(FILE058_HANGAR_MUS, 1, 1);
    
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 2);
    
    cur_diff &= ~(EB_SECRET_1 + EB_SECRET_2 + EB_SECRET_3);
    
    window = SWD_InitMasterWindow(FILE133_SHIPCOMP_SWD);
    
    GLB_LockItem(FILE12a_LIGHTON_PIC);
    GLB_LockItem(FILE12b_LIGHTOFF_PIC);
    
    SWD_GetFieldXYL(window, COMP_AUTO, &px, &py, &lx, &ly);
    PTR_SetPos(px + (lx >> 1), py + (ly >> 1));
    SWD_SetActiveField(window, COMP_AUTO);
    
    SWD_SetFieldItem(window, COMP_LITE1, FILE12b_LIGHTOFF_PIC);
    SWD_SetFieldItem(window, COMP_LITE2, FILE12b_LIGHTOFF_PIC);
    SWD_SetFieldItem(window, COMP_LITE3, FILE12b_LIGHTOFF_PIC);

    if (bday_flag)
    {
        secret = 1;
        secret1 = 1;
        secret2 = 1;
        secret3 = 1;
        SWD_SetFieldItem(window, COMP_LITE1, FILE12a_LIGHTON_PIC);
        SWD_SetFieldItem(window, COMP_LITE2, FILE12b_LIGHTOFF_PIC);
        SWD_SetFieldItem(window, COMP_LITE3, FILE12a_LIGHTON_PIC);
        SND_Patch(FX_EGRAB, 127);
    }
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    GFX_FadeIn(palette, 16);
    
    SWD_SetWindowPtr(window);
    PTR_DrawCursor(1);
    
    while (1)
    {
        SWD_Dialog(&dlg);

        if (joy_ipt_MenuNew)
        {
            if (Back)                                                         
            {
                JOY_IsKey(Back);
                dlg.keypress = SC_ESC;
            }
            
            if (BButton)
            {
                JOY_IsKey(BButton);
                dlg.keypress = SC_ESC;
            }
            
            if (RightShoulder)
            {
                JOY_IsKey(RightShoulder);
                dlg.keypress = SC_F1;
            }
        }
        
        if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
            WIN_AskExit();
        
        switch (dlg.keypress)
        {
        case SC_ESC:
            rval = 0;
            goto abort_shipcomp;
        
        case SC_F1:
            HELP_Win("COMPHLP1_TXT");
            break;
        
        case SC_Z:
            if (godmode || demo_flag)
                cur_game = 0;
            break;
        
        case SC_X:
            if (godmode || demo_flag)
                cur_game = 1;
            break;
        
        case SC_C:
            if (godmode || demo_flag)
                cur_game = 2;
            break;
        
        case SC_Q:
            if (godmode || demo_flag)
                game_wave[cur_game] = 0;
            goto exit_shipcomp;
        
        case SC_W:
            if (godmode || demo_flag)
                game_wave[cur_game] = 1;
            goto exit_shipcomp;
        
        case SC_E:
            if (godmode || demo_flag)
                game_wave[cur_game] = 2;
            goto exit_shipcomp;
        
        case SC_R:
            if (godmode || demo_flag)
                game_wave[cur_game] = 3;
            goto exit_shipcomp;
        
        case SC_T:
            if (godmode || demo_flag)
                game_wave[cur_game] = 4;
            goto exit_shipcomp;
        
        case SC_Y:
            if (godmode || demo_flag)
                game_wave[cur_game] = 5;
            goto exit_shipcomp;
        
        case SC_U:
            if (godmode || demo_flag)
                game_wave[cur_game] = 6;
            goto exit_shipcomp;
        
        case SC_I:
            if (godmode || demo_flag)
                game_wave[cur_game] = 7;
            goto exit_shipcomp;
        
        case SC_O:
            if (godmode || demo_flag)
                game_wave[cur_game] = 8;
            goto exit_shipcomp;
        }
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            switch (dlg.field)
            {
            case COMP_SECRET:
                secret = 1;
                break;
            
            case COMP_AUTO:
                goto exit_shipcomp;
            
            case COMP_GAME1:
                cur_game = 0;
                goto exit_shipcomp;
            
            case COMP_GAME2:
                if (!gameflag[1])
                {
                    WIN_Order();
                    rval = 0;
                    goto abort_shipcomp;
                }
                cur_game = 1;
                goto exit_shipcomp;
            
            case COMP_GAME3:
                if (!gameflag[2])
                {
                    WIN_Order();
                    rval = 0;
                    goto abort_shipcomp;
                }
                cur_game = 2;
                goto exit_shipcomp;
            
            case COMP_B1:
                if (secret)
                {
                    secret1 ^= 1;
                    if (secret1)
                        SWD_SetFieldItem(window, COMP_LITE1, FILE12a_LIGHTON_PIC);
                    else
                        SWD_SetFieldItem(window, COMP_LITE1, FILE12b_LIGHTOFF_PIC);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            
            case COMP_B2:
                if (secret)
                {
                    secret2 ^= 1;
                    if (secret2)
                        SWD_SetFieldItem(window, COMP_LITE2, FILE12a_LIGHTON_PIC);
                    else
                        SWD_SetFieldItem(window, COMP_LITE2, FILE12b_LIGHTOFF_PIC);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            
            case COMP_B3:
                if (secret)
                {
                    secret3 ^= 1;
                    if (secret3)
                        SWD_SetFieldItem(window, COMP_LITE3, FILE12a_LIGHTON_PIC);
                    else
                        SWD_SetFieldItem(window, COMP_LITE3, FILE12b_LIGHTOFF_PIC);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            
            case COMP_CLOSE:
                rval = 0;
                goto abort_shipcomp;
            }
        }
    }

exit_shipcomp:
    
    *ltable = cz1;
    *dtable = cz2;
    
    SWD_SetFieldText(window, COMP_GAME1, 0);
    SWD_SetFieldText(window, COMP_GAME2, 0);
    SWD_SetFieldText(window, COMP_GAME3, 0);
    SWD_SetFieldText(window, COMP_AUTO, 0);

abort_shipcomp:
    
    PTR_DrawCursor(0);
    
    GFX_FadeOut(0, 0, 0, 8);
    
    SWD_DestroyWindow(window);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    if (secret)
    {
        if (secret1 == 1 && secret2 == 1 && secret3 == 1)
        {
            cur_diff |= EB_SECRET_1;
            cur_diff |= EB_SECRET_2;
            cur_diff |= EB_SECRET_3;
            SND_Patch(FX_EGRAB, 127);
        }
    }
    
    if (bday_flag)
    {
        cur_diff |= EB_SECRET_1;
        cur_diff |= EB_SECRET_2;
        cur_diff |= EB_SECRET_3;
        SND_Patch(FX_EGRAB, 127);
    }
    
    hangto = HANGTOMISSION;
    
    GLB_FreeItem(FILE12a_LIGHTON_PIC);
    GLB_FreeItem(FILE12b_LIGHTOFF_PIC);
    
    return rval;
}

/***************************************************************************
WIN_SetLoadLevel()
 ***************************************************************************/
void 
WIN_SetLoadLevel(
    int level
)
{
    int addx, curs;
    
    addx = (g_lx << 16) / 100;
    curs = addx * level;
    
    GFX_ColorBox(g_x, g_y, (curs >> 16) + 1, g_ly, 85);
    GFX_DisplayUpdate();
}

/***************************************************************************
WIN_EndLoad () - Shows Ship computer while loading level
 ***************************************************************************/
void 
WIN_EndLoad(
    void
)
{
    GFX_FadeOut(0, 0, 0, 16);
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
}

/***************************************************************************
WIN_MainLoop() - Handles Locker/Register/Store/Hangar and starting game
 ***************************************************************************/
void 
WIN_MainLoop(
    void
)
{
    int rval, abort_flag, dwrap, loop;
    rval = -1;
    abort_flag = 0;
    
    ingameflag = 1;
    
    SND_PlaySong(FILE058_HANGAR_MUS, 1, 1);
    
    while (1)
    {
        if (demo_flag != DEMO_RECORD)
        {
            if (rval == -1)
                rval = WIN_Hangar();
            
            switch (rval)
            {
            case HANG_MISSION:
                rval = -1;
                break;
            
            default:
                return;
            
            case HANG_SUPPLIES:
                STORE_Enter();
                hangto = HANGTOMISSION;
                rval = -1;
                continue;
            }
        }
        
        if (!WIN_ShipComp())
            continue;
        
        if (demo_flag == DEMO_RECORD)
            DEMO_MakePlayer(cur_game);
        
        switch (cur_game)
        {
        default:
        case 0:
            SND_PlaySong(songsg1[game_wave[cur_game]], 1, 1);
            break;
        case 1:
            SND_PlaySong(songsg2[game_wave[cur_game]], 1, 1);
            break;
        case 2:
            SND_PlaySong(songsg3[game_wave[cur_game]], 1, 1);
            break;
        }
        
        WIN_LoadComp();
        RAP_LoadMap();
        retraceflag = 0;
        
        abort_flag = Do_Game();
        
        retraceflag = 1;
        
        if (OBJS_GetAmt(S_ENERGY) <= 0)
        {
            ingameflag = 0;
            SND_PlaySong(FILE05d_RAP5_MUS, 1, 1);
            INTRO_Death();
            return;
        }
        
        if (abort_flag)
        {
            INTRO_Landing();
            continue;
        }
        
        dwrap = diff_wrap[plr.diff[cur_game]] - 1;
        
        if (game_wave[cur_game] == dwrap)
        {
            if (!plr.diff[cur_game] && !plr.fintrain)
            {
                OBJS_Init();
                plr.sweapon = -1;
                plr.fintrain = 1;
                plr.score = 0;
                OBJS_Add(S_FORWARD_GUNS);
                OBJS_Add(S_ENERGY);
                OBJS_Add(S_ENERGY);
                OBJS_Add(S_ENERGY);
                plr.score = 10000;
                
                if (plr.diff[cur_game] < DIFF_3)
                    plr.diff[cur_game]++;
                
                for (loop = 0; loop < 4; loop++)
                    if (!plr.diff[loop])
                        plr.diff[loop] = DIFF_1;
            }
            else
            {
                if (plr.diff[cur_game] < DIFF_3)
                    plr.diff[cur_game]++;
            }
            
            RAP_SetPlayerDiff();
            
            game_wave[cur_game] = 0;
            
            if (dwrap < 8)
            {
                WIN_WinGame(3);
                cur_game = 0;
                game_wave[0] = 0;
                game_wave[1] = 0;
                game_wave[2] = 0;
            }
            else if (abort_flag == 0)
            {
                INTRO_EndGame(cur_game);
                cur_game++;
            }
            
            if (cur_game >= 3)
                cur_game = 0;
            
            if (!gameflag[cur_game])
                cur_game = 0;
            
            continue;
        }
        
        if (!abort_flag)
            game_wave[cur_game]++;
        
        INTRO_Landing();
    }
}

/***************************************************************************
   WIN_MainAuto()
 ***************************************************************************/
void 
WIN_MainAuto(
    int cur_opt
)
{
    int max_opt, end_flag, dchold;
    max_opt = 5;
    end_flag = 0;
    dchold = g_drawcursor;
    
    PTR_DrawCursor(0);
    
    if (GAME2)
        max_opt += 3;
    
    if (GAME3)
        max_opt += 3;
    
    while (1)
    {
        switch (cur_opt)
        {
        case DEM_INTRO:
            SND_CacheFX();
            SND_PlaySong(FILE056_RINTRO_MUS, 1, 1);
            end_flag = INTRO_PlayMain();
            break;
        
        case DEM_CREDITS:
            end_flag = WIN_Credits();
            break;
        
        case DEM_DEMO1G1:
            DEMO_GLBFile(FILE107_DEMO1G1_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO2G1:
            DEMO_GLBFile(FILE108_DEMO2G1_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO3G1:
            DEMO_GLBFile(FILE109_DEMO3G1_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO1G2:
            DEMO_GLBFile(FILE20b_DEMO1G2_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO2G2:
            DEMO_GLBFile(FILE20c_DEMO2G2_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO3G2:
            DEMO_GLBFile(FILE20d_DEMO3G2_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO1G3:
            DEMO_GLBFile(FILE307_DEMO1G3_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO2G3:
            DEMO_GLBFile(FILE308_DEMO2G3_REC);
            end_flag = DEMO_Play();
            break;
        
        case DEM_DEMO3G3:
            DEMO_GLBFile(FILE309_DEMO3G3_REC);
            end_flag = DEMO_Play();
            break;
        }
        
        if (end_flag)
            break;
        
        cur_opt++;
        
        cur_opt %= max_opt;
    }
    
    PTR_DrawCursor(dchold);
    GLB_FreeAll();
    SND_CacheIFX();
}

/***************************************************************************
WIN_DemoDelay (
 ***************************************************************************/
int 
WIN_DemoDelay(
    int startflag
)
{
    int local_cnt;
    
    if (startflag)
    {
        d_count = 0;
        return 0;
    }
    else
    {
        local_cnt = GFX_GetFrameCount();
        while (local_cnt == GFX_GetFrameCount())
        {
        }
        
        d_count++;
        
        if (d_count < DEMO_DELAY)
            return 0;
        
        return 1;
    }
}

/***************************************************************************
WIN_MainMenu () - Main Menu
 ***************************************************************************/
void 
WIN_MainMenu(
    void
)
{
    int window;
    int cur_opt;
    char cz1;
    wdlg_t dlg;

    cz1 = ltable[0];
    cur_opt = 0;
    
    KBD_Clear();
    
    PTR_DrawCursor(0);
    
    WIN_DemoDelay(1);
    
    if (demo_flag == DEMO_RECORD)
        return;
    
    window = SWD_InitMasterWindow(FILE132_MAIN_SWD);
    
    if (ingameflag)
    {
        SWD_SetFieldSelect(window, MAIN_RETURN, 1);
        SWD_SetFieldItem(window, MAIN_RETURN, FILE129_MENU7_PIC);
        SWD_SetFieldText(window, MAIN_DEMO, 0);
    }
    else
    {
        SWD_SetFieldSelect(window, MAIN_RETURN, 0);
        SWD_SetFieldItem(window, MAIN_RETURN, -1);
    }

    PTR_DrawCursor(0);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    if (ingameflag)
        SND_PlaySong(FILE056_RINTRO_MUS, 1, 1);
    else
        SND_PlaySong(FILE057_MAINMENU_MUS, 1, 1);

    GFX_FadeIn(palette, 10);
    
    SND_CacheIFX();
    
    SWD_SetWindowPtr(window);
    PTR_DrawCursor(1);
    ltable[0] = 0;
    
    do
    {
        SWD_Dialog(&dlg);
        
        if (dlg.keypress == SC_D || YButton)                                    
        {
            cur_opt = DEM_DEMO1G1;
            d_count = DEMO_DELAY + 2;
        }
        
        if (WIN_DemoDelay(0) && !ingameflag)
        {
            GFX_FadeOut(0, 0, 0, 3);
            SWD_DestroyWindow(window);
            memset(displaybuffer, 0, 64000);
            memset(displayscreen, 0, 64000);
            WIN_MainAuto(cur_opt);
            cur_opt = 0;
            window = SWD_InitMasterWindow(FILE132_MAIN_SWD);
            if (ingameflag)
                SWD_SetFieldItem(window, MAIN_RETURN, FILE129_MENU7_PIC);
            else
                SWD_SetFieldItem(window, MAIN_RETURN, -1);
            PTR_DrawCursor(0);
            SND_PlaySong(FILE057_MAINMENU_MUS, 1, 1);
            GFX_FadeOut(0, 0, 0, 2);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            GFX_FadeIn(palette, 16);
            WIN_DemoDelay(1);
            GLB_FreeAll();
            SND_CacheIFX();
            PTR_DrawCursor(1);
        }
        
        if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
            WIN_AskExit();
        
        if ((KBD_Key(SC_ESC) && ingameflag) || (Back && ingameflag) || (BButton && ingameflag))                                   
            goto menu_exit;
        
        if ((dlg.keypress == SC_F1) || (JOY_IsKeyMenu(RightShoulder)))                                                         
            HELP_Win("HELP1_TXT");
        
        if (mouseb1 || mouseb2 || dlg.keypress || (AButton && !joy_ipt_MenuNew))                                              
            WIN_DemoDelay(1);
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            WIN_DemoDelay(1);
            switch (dlg.field)
            {
            case MAIN_NEW:
                if (WIN_Register())
                    goto menu_exit;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                GFX_FadeIn(palette, 16);
                PTR_DrawCursor(1);
                break;
            
            case MAIN_LOAD:
                switch (RAP_LoadWin())
                {
                case -1:
                    WIN_Msg("No Pilots to Load");
                    break;
                
                case 0:
                    break;
                
                case 1:
                    ingameflag = 0;
                    goto menu_exit;
                }
                break;
            
            case MAIN_OPTS:
                WIN_Opts();
                break;
            
            case MAIN_ORDER:
                if (reg_flag)
                    HELP_Win("REG_VER1_TXT");
                else
                    HELP_Win("RAP1_TXT");
                break;
            
            case MAIN_CREDITS:
                PTR_DrawCursor(0);
                WIN_Credits();
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                SND_PlaySong(FILE057_MAINMENU_MUS, 1, 1);
                PTR_DrawCursor(1);
                break;
            
            case MAIN_RETURN:
                if (ingameflag)
                    goto menu_exit;
                break;
            
            case MAIN_QUIT:
                WIN_AskExit();
                break;
            }
        }
    } while (1);

menu_exit:
    
    PTR_DrawCursor(0);
    
    GFX_FadeOut(0, 0, 0, 16);
    SWD_DestroyWindow(window);
    memset(displaybuffer, 0, 64000);
    
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    
    ltable[0] = cz1;
    
    hangto = HANGTOSTORE;
}

