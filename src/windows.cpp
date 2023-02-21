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
#include "fileids.h"

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
        
        if (keyboard[SC_ESC] || Back || BButton)
        {
            rval = 0;
            fi_joy_count = 0;
            fi_sec_field = false;
            goto reg_exit;
        }
        
        if (keyboard[SC_X] && keyboard[SC_ALT])
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
                else if (dlg.keypress == SC_ENTER || keyboard[SC_ENTER])
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

int WIN_Hangar(void)
{
    char v88[44];
    wdlg_t vd0;
    int v2c, v30, v34, v38, v3c, v40, v28;
    int v1c, v20, v48, v24;
    int v5c[4] = {
        2, 3, 4, 5
    };
    v2c = 3;
    v30 = -1;
    v34 = 0;
    v38 = 0;
    v3c = GFX_GetFrameCount();
    v40 = 0;
    PTR_DrawCursor(0);
    KBD_Clear();

    if (plr.trainflag)
    {
        plr.trainflag = 0;
        v2c = 3;
    }
    else
    {
        GFX_FadeOut(0, 0, 0, 2);
        v28 = SWD_InitMasterWindow(FILE134_HANGAR_SWD);
        SWD_GetFieldItem(v28, 0);
        SND_PlaySong(88, 1, 1);
        SWD_ShowAllWindows();
        GFX_DisplayUpdate();
        GFX_FadeIn(palette, 16);
        if (control != 1)
            v38 = 1;
        switch (hangto)
        {
        case 1:
            v34 = 0;
            SWD_SetActiveField(v28, v5c[v34]);
            break;
        default:
        case 0:
            v34 = 1;
            SWD_SetActiveField(v28, v5c[v34]);
            break;
        }
        hangto = 1;
        SWD_SetWindowPtr(v28);
        PTR_DrawCursor(1);
        while (1)
        {
            v40++;
            if (v40 > 4)
            {
                v40 = 0;
                if ((wrand() % 3) == 0)
                    SWD_SetFieldItem(v28, 0, FILE10b_HANGP_PIC);
                else
                    SWD_SetFieldItem(v28, 0, -1);
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            else
            {
                v3c = GFX_GetFrameCount();
                while (GFX_GetFrameCount() == v3c)
                {
                }
            }
            SWD_Dialog(&vd0);

            if (joy_ipt_MenuNew)
            {
                if (StickY > 0)                                                   //Controller Input WIN_Hangar
                {
                    JOY_IsKey(StickY);
                    vd0.keypress = 80;
                }
                if (StickY < 0)
                {
                    JOY_IsKey(StickY);
                    vd0.keypress = 72;
                }
                if (StickX > 0)
                {
                    JOY_IsKey(StickX);
                    vd0.keypress = 77;
                }
                if (StickX < 0)
                {
                    JOY_IsKey(StickX);
                    vd0.keypress = 75;
                }
                if (Down)
                {
                    JOY_IsKey(Down);
                    vd0.keypress = 80;
                }
                if (Up)
                {
                    JOY_IsKey(Up);
                    vd0.keypress = 72;
                }
                if (Left)
                {
                    JOY_IsKey(Left);
                    vd0.keypress = 75;
                }
                if (Right)
                {
                    JOY_IsKey(Right);
                    vd0.keypress = 77;
                }
                if (AButton)
                {
                    JOY_IsKey(AButton);
                    vd0.keypress = 28;
                }
                if (RightShoulder)
                {
                    JOY_IsKey(RightShoulder);
                    vd0.keypress = 59;
                }
            }
            if (keyboard[1] || Back || BButton)
            {
                v2c = -99;
                goto LAB_00024d9c;
            }
            if (keyboard[45] && keyboard[56])
                WIN_AskExit();
            switch (vd0.keypress)
            {
            case 0x3b:
                HELP_Win("HANGHLP1_TXT");
                break;
            case 0x1f:
            case 0x3e:
                v34 = 3;
                sprintf(v88, "Save %s - %s ?", plr.name, plr.callsign);
                if (WIN_AskBool(v88))
                    RAP_SavePlayer();
                break;
            case 0xf:
            case 0x48:
            case 0x4b:
                v38 = 1;
                KBD_Wait(0x48);
                KBD_Wait(0x4b);
                v34++;
                v34 %= 4;
                break;

            case 0x4d:
            case 0x50:
                v38 = 1;
                KBD_Wait(0x50);
                KBD_Wait(0x4d);
                v34--;
                if (v34 < 0)
                    v34 = 3;
                break;
            case 0x1c:
            case 0x39:
                KBD_Wait(0x1c);
                KBD_Wait(0x39);
                v2c = v5c[v34];
                goto LAB_00024b33;
            }
            if (v38)
            {
                v38 = 0;
                switch (v34)
                {
                case 0:
                case 1:
                case 2:
                case 3:
                    v2c = v5c[v34];
                    if (v2c != v30)
                    {
                        v30 = v2c;
                        SWD_GetFieldXYL(v28, v2c, &v1c, &v20, &v48, &v24);
                        PTR_SetPos(v1c + (v48>>1), v20 + (v24>>1));
                        v30 = -1;
                        vd0.sfield = v2c;
                        vd0.viewactive = 1;
                    }
                    break;
                default:
                    v34 = 0;
                    break;
                }
            }
            if (vd0.viewactive)
            {
LAB_00024b33:
                switch (vd0.sfield)
                {
                case 2:
                    v34 = 0;
                    v2c = vd0.sfield;
                    if ((mouseb1) || (vd0.keypress == 0x1c) || (AButton && !joy_ipt_MenuNew))                //Fixed ptr input
                    {
                        SND_Patch(12, 60);
                        while (IMS_IsAck())
                        {
                        }
                        goto LAB_00024d9c;
                    }
                    if (v2c != v30)
                    {
                        SWD_SetFieldText(v28, 1, hangtext[v34]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        v30 = v2c;
                    }
                    break;
                case 3:
                    v34 = 1;
                    v2c = vd0.sfield;
                    if ((mouseb1) || (vd0.keypress == 0x1c) || (AButton && !joy_ipt_MenuNew))               //Fixed ptr input
                    {
                        SND_Patch(12, 127);
                        while (IMS_IsAck())
                        {
                        }
                        goto LAB_00024d9c;
                    }
                    if (v2c != v30)
                    {
                        SWD_SetFieldText(v28, 1, hangtext[v34]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        v30 = v2c;
                    }
                    break;
                case 4:
                    v34 = 2;
                    v2c = vd0.sfield;
                    if ((mouseb1) || (vd0.keypress == 0x1c) || (AButton && !joy_ipt_MenuNew))             //Fixed ptr input
                    {
                        v2c = -99;
                        SND_Patch(12, 200);
                        while (IMS_IsAck())
                        {
                        }
                        goto LAB_00024d9c;
                    }
                    if (v2c != v30)
                    {
                        SWD_SetFieldText(v28, 1, hangtext[v34]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        v30 = v2c;
                    }
                    break;
                case 5:
                    v34 = 3;
                    v2c = vd0.sfield;
                    if ((mouseb1) || (vd0.keypress == 0x1c) || (AButton && !joy_ipt_MenuNew))            //Fixed ptr input
                    {
                        while (IMS_IsAck())
                        {
                        }
                        sprintf(v88, "Save %s - %s ?", plr.name, plr.callsign);
                        if (WIN_AskBool(v88))
                            RAP_SavePlayer();
                        while (IMS_IsAck())
                        {
                        }
                        break;
                    }
                    if (v2c != v30)
                    {
                        SWD_SetFieldText(v28, 1, hangtext[v34]);
                        SWD_ShowAllWindows();
                        GFX_DisplayUpdate();
                        v30 = v2c;
                    }
                    break;
                }
            }
            else
            {
                v2c = -1;
                if (v2c != v30)
                {
                    SWD_SetFieldText(v28, 1, " ");
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v30 = v2c;
                }
            }
        }
    LAB_00024d9c:
        PTR_DrawCursor(0);
        GFX_FadeOut(0, 0, 0, 16);
        SWD_DestroyWindow(v28);
        memset(displaybuffer, 0, 64000);
        GFX_DisplayUpdate();
        GFX_SetPalette(palette, 0);
    }
    return v2c;
}

void WIN_LoadComp(void)
{
    char v44[40];
    int v1c;
    char v74[3][15] = {
        "BRAVO SECTOR",
        "TANGO SECTOR",
        "OUTER REGIONS"
    };

    v1c = SWD_InitMasterWindow(FILE140_LOADCOMP_SWD);
    SWD_GetFieldXYL(v1c, 11, &g_x, &g_y, &g_lx, &g_ly);
    sprintf(v44, "WAVE %d", game_wave[cur_game]);
    SWD_SetFieldText(v1c, 10, v44);
    SWD_SetFieldText(v1c, 9, v74[cur_game]);
    if (diff_wrap[plr.diff[cur_game]] - 1 == game_wave[cur_game])
    {
        sprintf(v44, "FINAL WAVE %d", game_wave[cur_game] + 1);
        SWD_SetFieldText(v1c, 10, v44);
    }
    else
    {
        sprintf(v44, "WAVE %d", game_wave[cur_game] + 1);
        SWD_SetFieldText(v1c, 10, v44);
    }
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 8);
    SWD_DestroyWindow(v1c);
}

int WIN_ShipComp(void)
{
    wdlg_t v94;
    int v24, v28, v2c, v30, v34,v1c,v20;
    int v3c;
    int v40, v44, v38, v4c;

    v24 = 1;
    v28 = 0;
    v2c = 0;
    v30 = 0;
    v34 = 0;

    v1c = *ltable;
    v20 = *dtable;
    GLB_FreeAll();
    PTR_DrawCursor(0);
    ltable[0] = 0;
    dtable[0] = 0;
    SND_PlaySong(88, 1, 1);
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 2);
    cur_diff &= ~7;
    v3c = SWD_InitMasterWindow(FILE133_SHIPCOMP_SWD);
    GLB_LockItem(FILE12a_LIGHTON_PIC);
    GLB_LockItem(FILE12b_LIGHTOFF_PIC);
    SWD_GetFieldXYL(v3c, 11, &v40, &v44, &v38, &v4c);
    PTR_SetPos(v40 + (v38 >> 1), v44 + (v4c >> 1));
    SWD_SetActiveField(v3c, 11);
    SWD_SetFieldItem(v3c, 0, FILE12b_LIGHTOFF_PIC);
    SWD_SetFieldItem(v3c, 1, FILE12b_LIGHTOFF_PIC);
    SWD_SetFieldItem(v3c, 2, FILE12b_LIGHTOFF_PIC);

    if (bday_flag)
    {
        v34 = 1;
        v28 = 1;
        v2c = 1;
        v30 = 1;
        SWD_SetFieldItem(v3c, 0, FILE12a_LIGHTON_PIC);
        SWD_SetFieldItem(v3c, 1, FILE12a_LIGHTON_PIC);
        SWD_SetFieldItem(v3c, 2, FILE12a_LIGHTON_PIC);
        SND_Patch(14, 127);
    }
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 16);
    SWD_SetWindowPtr(v3c);
    PTR_DrawCursor(1);
    while (1)
    {
        SWD_Dialog(&v94);

        if (joy_ipt_MenuNew)
        {
            if (Back)                                                         //Input Controller WIN_ShipComp
            {
                JOY_IsKey(Back);
                v94.keypress = 1;
            }
            if (BButton)
            {
                JOY_IsKey(BButton);
                v94.keypress = 1;
            }
            if (RightShoulder)
            {
                JOY_IsKey(RightShoulder);
                v94.keypress = 59;
            }
        }
        if (keyboard[45] && keyboard[56])
            WIN_AskExit();
        switch (v94.keypress)
        {
        case 1:
            v24 = 0;
            goto LAB_000255a5;
        case 0x3b:
            HELP_Win("COMPHLP1_TXT");
            break;
        case 0x2c:
            if (godmode || demo_flag)
                cur_game = 0;
            break;
        case 0x2d:
            if (godmode || demo_flag)
                cur_game = 1;
            break;
        case 0x2e:
            if (godmode || demo_flag)
                cur_game = 2;
            break;
        case 0x10:
            if (godmode || demo_flag)
                game_wave[cur_game] = 0;
            goto LAB_00025553;
        case 0x11:
            if (godmode || demo_flag)
                game_wave[cur_game] = 1;
            goto LAB_00025553;
        case 0x12:
            if (godmode || demo_flag)
                game_wave[cur_game] = 2;
            goto LAB_00025553;
        case 0x13:
            if (godmode || demo_flag)
                game_wave[cur_game] = 3;
            goto LAB_00025553;
        case 0x14:
            if (godmode || demo_flag)
                game_wave[cur_game] = 4;
            goto LAB_00025553;
        case 0x15:
            if (godmode || demo_flag)
                game_wave[cur_game] = 5;
            goto LAB_00025553;
        case 0x16:
            if (godmode || demo_flag)
                game_wave[cur_game] = 6;
            goto LAB_00025553;
        case 0x17:
            if (godmode || demo_flag)
                game_wave[cur_game] = 7;
            goto LAB_00025553;
        case 0x18:
            if (godmode || demo_flag)
                game_wave[cur_game] = 8;
            goto LAB_00025553;
        }
        if (v94.cur_act == 1 && v94.cur_cmd == 10)
        {
            switch (v94.field)
            {
            case 6:
                v34 = 1;
                break;
            case 11:
                goto LAB_00025553;
            case 4:
                cur_game = 0;
                goto LAB_00025553;
            case 5:
                if (!gameflag[1])
                {
                    WIN_Order();
                    v24 = 0;
                    goto LAB_000255a5;
                }
                cur_game = 1;
                goto LAB_00025553;
            case 10:
                if (!gameflag[2])
                {
                    WIN_Order();
                    v24 = 0;
                    goto LAB_000255a5;
                }
                cur_game = 2;
                goto LAB_00025553;
            case 7:
                if (v34)
                {
                    v28 ^= 1;
                    if (v28)
                        SWD_SetFieldItem(v3c, 0, FILE12a_LIGHTON_PIC);
                    else
                        SWD_SetFieldItem(v3c, 0, FILE12b_LIGHTOFF_PIC);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            case 8:
                if (v34)
                {
                    v2c ^= 1;
                    if (v2c)
                        SWD_SetFieldItem(v3c, 1, FILE12a_LIGHTON_PIC);
                    else
                        SWD_SetFieldItem(v3c, 1, FILE12b_LIGHTOFF_PIC);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            case 9:
                if (v34)
                {
                    v30 ^= 1;
                    if (v30)
                        SWD_SetFieldItem(v3c, 2, FILE12a_LIGHTON_PIC);
                    else
                        SWD_SetFieldItem(v3c, 2, FILE12b_LIGHTOFF_PIC);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            case 3:
                v24 = 0;
                goto LAB_000255a5;
            }
        }
    }
LAB_00025553:
    *ltable = v1c;
    *dtable = v20;
    SWD_SetFieldText(v3c, 4, 0);
    SWD_SetFieldText(v3c, 5, 0);
    SWD_SetFieldText(v3c, 10, 0);
    SWD_SetFieldText(v3c, 11, 0);
LAB_000255a5:
    PTR_DrawCursor(0);
    GFX_FadeOut(0, 0, 0, 8);
    SWD_DestroyWindow(v3c);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    if (v34)
    {
        if (v28 == 1 && v2c == 1 && v30 == 1)
        {
            cur_diff |= 1;
            cur_diff |= 2;
            cur_diff |= 4;
            SND_Patch(14, 127);
        }
    }
    if (bday_flag)
    {
        cur_diff |= 1;
        cur_diff |= 2;
        cur_diff |= 4;
        SND_Patch(14, 127);
    }
    hangto = 1;
    GLB_FreeItem(FILE12a_LIGHTON_PIC);
    GLB_FreeItem(FILE12b_LIGHTOFF_PIC);
    return v24;
}

void WIN_SetLoadLevel(int a1)
{
    int v1c, v20;
    v1c = (g_lx << 16) / 100;
    v20 = v1c * a1;
    GFX_ColorBox(g_x, g_y, (v20 >> 16) + 1, g_ly, 85);
    GFX_DisplayUpdate();
}

void WIN_EndLoad(void)
{
    GFX_FadeOut(0, 0, 0, 16);
    memset(displaybuffer, 0, 64000);
    GFX_MarkUpdate(0, 0, 320, 200);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
}

void WIN_MainLoop(void)
{
    int v1c, v20, v24, v28;
    v1c = -1;
    v20 = 0;
    ingameflag = 1;
    SND_PlaySong(88, 1, 1);
    while (1)
    {
        if (demo_flag != 1)
        {
            if (v1c == -1)
                v1c = WIN_Hangar();
            switch (v1c)
            {
            case 2:
                v1c = -1;
                break;
            default:
                return;
            case 3:
                STORE_Enter();
                hangto = 1;
                v1c = -1;
                continue;
            }
        }
        if (!WIN_ShipComp())
            continue;
        if (demo_flag == 1)
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
        v20 = Do_Game();
        retraceflag = 1;
        if (OBJS_GetAmt(16) <= 0)
        {
            ingameflag = 0;
            SND_PlaySong(93, 1, 1);
            INTRO_Death();
            return;
        }
        if (v20)
        {
            INTRO_Landing();
            continue;
        }
        v24 = diff_wrap[plr.diff[cur_game]] - 1;
        if (game_wave[cur_game] == v24)
        {
            if (!plr.diff[cur_game] && !plr.fintrain)
            {
                OBJS_Init();
                plr.sweapon = -1;
                plr.fintrain = 1;
                plr.score = 0;
                OBJS_Add(0);
                OBJS_Add(16);
                OBJS_Add(16);
                OBJS_Add(16);
                plr.score = 0x2710;
                if (plr.diff[cur_game] < 3)
                    plr.diff[cur_game]++;
                for (v28 = 0; v28 < 4; v28++)
                    if (!plr.diff[v28])
                        plr.diff[v28] = 1;
            }
            else
            {
                if (plr.diff[cur_game] < 3)
                    plr.diff[cur_game]++;
            }
            RAP_SetPlayerDiff();
            game_wave[cur_game] = 0;
            if (v24 < 8)
            {
                WIN_WinGame(3);
                cur_game = 0;
                game_wave[0] = 0;
                game_wave[1] = 0;
                game_wave[2] = 0;
            }
            else if (v20 == 0)
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
        if (!v20)
            game_wave[cur_game]++;
        INTRO_Landing();
    }
}

void WIN_MainAuto(int a1)
{
    int v1c, v20, v24;
    v1c = 5;
    v20 = 0;
    v24 = g_drawcursor;
    PTR_DrawCursor(0);
    if (gameflag[1])
        v1c += 3;
    if (gameflag[2] + gameflag[3])
        v1c += 3;
    while (1)
    {
        switch (a1)
        {
        case 0:
            SND_CacheFX();
            SND_PlaySong(86, 1, 1);
            v20 = INTRO_PlayMain();
            break;
        case 1:
            v20 = WIN_Credits();
            break;
        case 2:
            DEMO_GLBFile(FILE107_DEMO1G1_REC);
            v20 = DEMO_Play();
            break;
        case 3:
            DEMO_GLBFile(FILE108_DEMO2G1_REC);
            v20 = DEMO_Play();
            break;
        case 4:
            DEMO_GLBFile(FILE109_DEMO3G1_REC);
            v20 = DEMO_Play();
            break;
        case 5:
            DEMO_GLBFile(FILE20b_DEMO1G2_REC);
            v20 = DEMO_Play();
            break;
        case 6:
            DEMO_GLBFile(FILE20c_DEMO2G2_REC);
            v20 = DEMO_Play();
            break;
        case 7:
            DEMO_GLBFile(FILE20d_DEMO3G2_REC);
            v20 = DEMO_Play();
            break;
        case 8:
            DEMO_GLBFile(FILE307_DEMO1G3_REC);
            v20 = DEMO_Play();
            break;
        case 9:
            DEMO_GLBFile(FILE308_DEMO2G3_REC);
            v20 = DEMO_Play();
            break;
        case 10:
            DEMO_GLBFile(FILE309_DEMO3G3_REC);
            v20 = DEMO_Play();
            break;
        }
        if (v20)
            break;
        a1++;
        a1 %= v1c;
    }
    PTR_DrawCursor(v24);
    GLB_FreeAll();
    SND_CacheIFX();
}

int FUN_00025c70(int a1)
{
    int now;
    if (a1)
    {
        d_count = 0;
        return 0;
    }
    else
    {
        now = GFX_GetFrameCount();
        while (now == GFX_GetFrameCount())
        {
        }
        d_count++;
        if (d_count < 4000)
            return 0;
        return 1;
    }
}

void WIN_MainMenu(void)
{
    int v20;
    int v24;
    char v1c;
    wdlg_t v6c;

    v1c = ltable[0];
    v24 = 0;
    KBD_Clear();
    PTR_DrawCursor(0);
    FUN_00025c70(1);
    if (demo_flag == 1)
        return;
    v20 = SWD_InitMasterWindow(FILE132_MAIN_SWD);
    if (ingameflag)
    {
        SWD_SetFieldSelect(v20, 7, 1);
        SWD_SetFieldItem(v20, 7, FILE129_MENU7_PIC);
        SWD_SetFieldText(v20, 8, 0);
    }
    else
    {
        SWD_SetFieldSelect(v20, 7, 0);
        SWD_SetFieldItem(v20, 7, -1);
    }

    PTR_DrawCursor(0);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    if (ingameflag)
        SND_PlaySong(0x56, 1, 1);
    else
        SND_PlaySong(0x57, 1, 1);

    GFX_FadeIn(palette, 10);
    SND_CacheIFX();
    SWD_SetWindowPtr(v20);
    PTR_DrawCursor(1);
    ltable[0] = 0;
    do
    {
        SWD_Dialog(&v6c);
        if (v6c.keypress == 0x20 || YButton)                                    //Get Button to start Demo Mode
        {
            v24 = 2;
            d_count = 4002;
        }
        if (FUN_00025c70(0) && !ingameflag)
        {
            GFX_FadeOut(0, 0, 0, 3);
            SWD_DestroyWindow(v20);
            memset(displaybuffer, 0, 64000);
            memset(displayscreen, 0, 64000);
            WIN_MainAuto(v24);
            v24 = 0;
            v20 = SWD_InitMasterWindow(FILE132_MAIN_SWD);
            if (ingameflag)
                SWD_SetFieldItem(v20, 7, FILE129_MENU7_PIC);
            else
                SWD_SetFieldItem(v20, 7, -1);
            PTR_DrawCursor(0);
            SND_PlaySong(87, 1, 1);
            GFX_FadeOut(0, 0, 0, 2);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            GFX_FadeIn(palette, 16);
            FUN_00025c70(1);
            GLB_FreeAll();
            SND_CacheIFX();
            PTR_DrawCursor(1);
        }
        if (keyboard[45] && keyboard[56])
            WIN_AskExit();
        if ((keyboard[1] && ingameflag) || (Back && ingameflag) || (BButton && ingameflag))                                   //Go back to Hangar when mission before started
            goto LAB_000260df;
        if ((v6c.keypress == 0x3b) || (JOY_IsKeyMenu(RightShoulder)))                                                         //Input Help Screen
            HELP_Win("HELP1_TXT");
        if (mouseb1 || mouseb2 || v6c.keypress || (AButton && !joy_ipt_MenuNew))                                              //Fixed ptr input
            FUN_00025c70(1);
        if (v6c.cur_act == 1 && v6c.cur_cmd == 10)
        {
            FUN_00025c70(1);
            switch (v6c.field)
            {
            case 1:
                if (WIN_Register())
                    goto LAB_000260df;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                GFX_FadeIn(palette, 16);
                PTR_DrawCursor(1);
                break;
            case 2:
                switch (RAP_LoadWin())
                {
                case -1:
                    WIN_Msg("No Pilots to Load");
                    break;
                case 0:
                    break;
                case 1:
                    ingameflag = 0;
                    goto LAB_000260df;
                }
                break;
            case 3:
                WIN_Opts();
                break;
            case 4:
                if (reg_flag)
                    HELP_Win("REG_VER1_TXT");
                else
                    HELP_Win("RAP1_TXT");
                break;
            case 5:
                PTR_DrawCursor(0);
                WIN_Credits();
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                SND_PlaySong(87, 1, 1);
                PTR_DrawCursor(1);
                break;
            case 7:
                if (ingameflag)
                    goto LAB_000260df;
                break;
            case 6:
                WIN_AskExit();
                break;
            }
        }
    } while (1);
LAB_000260df:
    PTR_DrawCursor(0);
    GFX_FadeOut(0, 0, 0, 16);
    SWD_DestroyWindow(v20);
    memset(displaybuffer, 0, 64000);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    ltable[0] = v1c;
    hangto = 0;
}

