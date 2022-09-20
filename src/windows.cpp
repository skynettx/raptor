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
#include "fileids.h"

int d_count;
int hangto;
int opt_detail;

int DAT_00061a78[2] = { 127, 127 };
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

void WIN_WinGame(int a1)
{
    int v1c;
    int v30[4] = {
        10, 11, 12, 9
    };
    if (a1 > 3)
        return;

    GFX_FadeOut(0, 0, 0, 2);
    v1c = SWD_InitWindow(FILE13e_WINGAME_SWD);
    SWD_SetFieldItem(v1c, 2, v30[a1]);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 16);
    IMS_WaitTimedSwd(30);
    SWD_DestroyWindow(v1c);
    GFX_DisplayUpdate();
}

void WIN_Msg(const char *a1)
{
    int v1c;

    v1c = SWD_InitWindow(FILE13a_MSG_SWD);
    SWD_SetFieldText(v1c, 5, a1);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    SND_Patch(20, 127);
    IMS_WaitTimedSwd(10);
    SWD_DestroyWindow(v1c);
    GFX_DisplayUpdate();
    KBD_Clear();
}

void FUN_000233f8(wdlg_t *a1)
{
    int v1c, v20, v24, v38;
    if (!a1)
        return;
    SWD_GetFieldXYL(opt_window, 11, &v1c, &v20, &v24, &v38);
    GFX_PutSprite((texture_t*)GLB_GetItem(FILE127_SLIDE_PIC), v1c + DAT_00061a78[0] - 2, v20);
    SWD_GetFieldXYL(opt_window, 12, &v1c, &v20, &v24, &v38);
    GFX_PutSprite((texture_t*)GLB_GetItem(FILE127_SLIDE_PIC), v1c + DAT_00061a78[1] - 2, v20);
}

void WIN_Opts(void)
{
    wdlg_t vb0;
    int v1c, v20, v24, v3c;
    int v34, v38, v30, v2c;
    int v28;
    int v48[3] = {
        3, 4, 5
    };
    char v68[2][16] = {
        "LOW DETAIL",
        "HIGH DETAIL"
    };

    v34 = 0;
    v38 = 0;
    v30 = opt_detail;
    v2c = 0;

    DAT_00061a78[0] = music_volume;
    DAT_00061a78[1] = fx_volume;
    opt_window = SWD_InitWindow(FILE13f_OPTS_SWD);
    SWD_SetWindowPtr(opt_window);
    SWD_SetFieldText(opt_window, 6, v68[v30]);
    SWD_SetWinDrawFunc(opt_window, FUN_000233f8);
    SWD_SetFieldItem(opt_window, 3, -1);
    SWD_SetFieldItem(opt_window, 4, -1);
    SWD_SetFieldItem(opt_window, 5, -1);
    SWD_ShowAllWindows();
    SND_Patch(20, 127);
    GFX_DisplayUpdate();

    while (1)
    {
        v38 = 0;
        SWD_Dialog(&vb0);

        if (joy_ipt_MenuNew)
        {
            if (StickY > 0)                                                   //Controller Input WIN_Opts
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 80;
            }
            if (StickY < 0)
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 72;
            }
            if (StickX > 0)
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 77;
            }
            if (StickX < 0)
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 75;
            }
            if (Down)
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 80;
            }
            if (Up)
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 72;
            }
            if (Left)
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 75;
            }
            if (Right)
            {
                if (JOY_IsScroll(0) == 1)
                    vb0.f_10 = 77;
            }
            if (Back)
            {
                vb0.f_10 = 1;
                JOY_IsKey(Back);
            }
            if (BButton)
            {
                vb0.f_10 = 1;
                JOY_IsKey(BButton);
            }
            if (AButton)
            {
                vb0.f_10 = 28;
                JOY_IsKey(AButton);
            }
        }
        switch (vb0.f_10)
        {
        case 1:
            vb0.f_8 = 1;
            vb0.f_c = 10;
            vb0.f_4 = 7;
            break;
        case 0x4b:
            if (v2c)
            {
                if (v2c == 2)
                    v38 = 1;
                DAT_00061a78[v2c - 1] -= 8;
                if (DAT_00061a78[v2c - 1] < 0)
                    DAT_00061a78[v2c - 1] = 0;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            break;
        case 0x4d:
            if (v2c)
            {
                if (v2c == 2)
                    v38 = 1;
                DAT_00061a78[v2c - 1] += 8;
                if (DAT_00061a78[v2c - 1] > 127)
                    DAT_00061a78[v2c - 1] = 127;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            break;
        case 0x48:
            if (v34 && v2c > 0)
                v2c--;
            v34 = 1;
            SND_Patch(20, 127);
            SWD_SetFieldItem(opt_window, 3, -1);
            SWD_SetFieldItem(opt_window, 4, -1);
            SWD_SetFieldItem(opt_window, 5, -1);
            SWD_SetFieldItem(opt_window, v48[v2c], FILE128_POINT_PIC);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            break;
        case 0x50:
            if (v34 && v2c < 2)
                v2c++;
            v34 = 1;
            SND_Patch(20, 127);
            SWD_SetFieldItem(opt_window, 3, -1);
            SWD_SetFieldItem(opt_window, 4, -1);
            SWD_SetFieldItem(opt_window, 5, -1);
            SWD_SetFieldItem(opt_window, v48[v2c], FILE128_POINT_PIC);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            break;
        case 0x1c:
            if (!v2c)
            {
                vb0.f_8 = 1;
                vb0.f_c = 10;
                vb0.f_4 = 6;
            }
            break;
        }
        if (vb0.f_30)
        {
            switch (vb0.f_34)
            {
            case 11:
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                  //Fixed ptr input
                {
                    while (!IMS_IsAck())
                    {
                    }
                    SWD_GetFieldXYL(opt_window, 11, &v1c, &v20, &v24, &v3c);
                    v28 = cur_mx- v1c;
                    if (v28 < 0)
                        v28 = 0;
                    if (v28 > 127)
                        v28 = 127;
                    DAT_00061a78[0] = v28;
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            case 12:
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                 //Fixed ptr input
                {
                    while (!IMS_IsAck())
                    {
                    }
                    SWD_GetFieldXYL(opt_window, 12, &v1c, &v20, &v24, &v3c);
                    v28 = cur_mx - v1c;
                    if (v28 < 0)
                        v28 = 0;
                    if (v28 > 127)
                        v28 = 127;
                    DAT_00061a78[1] = v28;
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                break;
            }
        }
        opt_detail = v30;
        if (fx_volume != DAT_00061a78[1])
            fx_volume = DAT_00061a78[1];
        if (music_volume != DAT_00061a78[0])
        {
            music_volume = DAT_00061a78[0];
            MUS_SetVolume(music_volume);
        }
        if (vb0.f_8 == 1 && vb0.f_c == 10)
        {
            switch (vb0.f_4)
            {
            case 7:
                if (DAT_00061a78[0] >= 0 && DAT_00061a78[0] < 128)
                {
                    music_volume = DAT_00061a78[0];
                    INI_PutPreferenceLong("Music", "Volume", music_volume);
                }
                if (DAT_00061a78[1] >= 0 && DAT_00061a78[1] < 128)
                {
                    fx_volume = DAT_00061a78[1];
                    INI_PutPreferenceLong("SoundFX", "Volume", fx_volume);
                }
                INI_PutPreferenceLong("Setup", "Detail", opt_detail);
                SND_Patch(20, 127);
                goto LAB_00023a57;
            case 6:
                SND_Patch(20, 127);
                v30 ^= 1;
                SWD_SetFieldText(opt_window, 6, v68[v30]);
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                break;
            }
        }
        if (v38)
            SND_Patch(20, 127);
    }
LAB_00023a57:
    SWD_SetWinDrawFunc(opt_window, NULL);
    SWD_DestroyWindow(opt_window);
    GFX_DisplayUpdate();
    KBD_Clear();
    return;
}

void WIN_Pause(void)
{
    int v1c;

    v1c = SWD_InitWindow(FILE13a_MSG_SWD);
    SWD_SetFieldText(v1c, 5, "GAME PAUSED");
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    SND_Patch(20, 127);
    while (!IMS_CheckAck())                                      //Pause Screen
    {
    }
    SWD_DestroyWindow(v1c);
    GFX_DisplayUpdate();
    JOY_Wait(0);
    KBD_Clear();
    IMS_StartAck();
}

void WIN_Order(void)
{
    int v1c, v20;

    v20 = g_drawcursor;
    if (gameflag[1] == 0 && gameflag[2] + gameflag[3] == 0)
    {
        PTR_DrawCursor(0);
        KBD_Clear();
        GFX_FadeOut(0, 0, 0, 2);
        v1c = SWD_InitWindow(FILE13b_ORDER_SWD);
        SWD_ShowAllWindows();
        GFX_DisplayUpdate();
        GFX_FadeIn(palette, 16);
        IMS_WaitTimedSwd(15);
        GFX_FadeOut(0, 0, 0, 16);
        SWD_DestroyWindow(v1c);
        memset(displaybuffer, 0, 64000);
        GFX_DisplayUpdate();
        GFX_SetPalette(palette, 0);
        KBD_Clear();
        PTR_DrawCursor(v20);
    }
}

int WIN_Credits(void)
{
    int v20, v1c;
    static int songid = 1;
    int songs[3] = {
        83, 84, 85
    };
    songid++;
    if (songid >= 3)
        songid = 0;
    SND_PlaySong(songs[songid], 1, 1);
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 16);
    v20 = SWD_InitWindow(FILE13c_CREDIT_SWD);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 16);
    v1c = IMS_WaitTimedSwd(25);
    GFX_FadeOut(0, 0, 0, 16);
    SWD_DestroyWindow(v20);
    memset(displaybuffer, 0, 64000);
    memset(displayscreen, 0, 64000);
    GFX_SetPalette(palette, 0);
    KBD_Clear();
    return v1c;
}

int WIN_AskBool(const char *a1)
{
    int v1c;
    int v40;
    int v24;
    int v30, v34, v20, v3c;
    wdlg_t v8c;

    v1c = 0;
    v40 = g_drawcursor;
    KBD_Clear();
    v24 = SWD_InitWindow(FILE135_ASK_SWD);
    SWD_SetFieldText(v24, 5, a1);
    SWD_SetActiveField(v24, 6);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    SND_Patch(20, 127);
    PTR_DrawCursor(1);
    SWD_GetFieldXYL(v24, 6, &v30, &v34, &v20, &v3c);
    PTR_SetPos(v30 + (v20 >> 1), v34 + (v3c >> 1));
    SWD_SetActiveField(v24, 6);

    while (1)
    {
        SWD_Dialog(&v8c);
        if (KBD_IsKey(1) || JOY_IsKeyInGameBack(Back))                                                   //Fixed Line Gamepad Abort Mission Screen
        {
            v8c.f_8 = 1;
            v8c.f_c = 10;
            v8c.f_4 = 7;
        }
        if (v8c.f_8 == 1 && v8c.f_c == 10)
        {
            switch (v8c.f_4)
            {
            case 6:
                v1c = 1;
                [[fallthrough]];
            case 7:
                SWD_DestroyWindow(v24);
                GFX_DisplayUpdate();
                PTR_DrawCursor(v40);
                return v1c;
            }
        }
    }
    return 0;
}

void WIN_AskExit(void)
{
    if (WIN_AskBool("EXIT TO DOS"))
    {
        SND_FadeOutSong();
        switch (bday_num)
        {
        case 0:
            SND_Patch(3, 127);
            while (SND_IsPatchPlaying(2))
            {
            }
            break;
        case 1:
            SND_Patch(7, 127);
            while (SND_IsPatchPlaying(7))
            {
            }
            break;
        case 2:
            SND_Patch(4, 127);
            while (SND_IsPatchPlaying(4))
            {
            }
            break;
        case 3:
            SND_Patch(1, 127);
            while (SND_IsPatchPlaying(1))
            {
            }
            break;
        case 4:
            SND_Patch(2, 127);
            while (SND_IsPatchPlaying(2))
            {
            }
            SND_Patch(2, 127);
            while (SND_IsPatchPlaying(2))
            {
            }
            break;
        case 5:
            SND_Patch(6, 127);
            while (SND_IsPatchPlaying(6))
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

int WIN_AskDiff(void)
{
    wdlg_t v7c;
    int v1c, v24;
    int v28, v2c, v20, v30;

    v1c = -1;
    KBD_Clear();
    v24 = SWD_InitWindow(FILE13d_ASKDIFF_SWD);
    SWD_SetActiveField(v24, 8);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    SWD_GetFieldXYL(v24, 8, &v28, &v2c, &v20, &v30);
    PTR_SetPos(v28 + (v20 >> 1), v2c + (v30 >> 1));
    while (1)
    {
        SWD_Dialog(&v7c);
        if (KBD_IsKey(1) || Back || BButton)                                                      //Fixed Line Gamepad Difficulty Menü
        {
            v1c = -1;
            goto LAB_00024094;
        }
        if (v7c.f_8 == 1 && v7c.f_c == 10)
        {
            switch (v7c.f_4)
            {
            case 6:
                v1c = 0;
                WIN_Msg("TRAIN MODE plays 4 of 9 levels!");
                goto LAB_00024094;
            case 7:
                v1c = 1;
                goto LAB_00024094;
            case 8:
                v1c = 2;
                goto LAB_00024094;
            case 9:
                v1c = 3;
                goto LAB_00024094;
            case 10:
                goto LAB_00024094;
            }
        }
    }
LAB_00024094:
    SWD_DestroyWindow(v24);
    GFX_DisplayUpdate();
    return v1c;
}

int sid_pics[4] = {
    FILE116_WMALEID_PIC,
    FILE119_BMALEID_PIC,
    FILE117_WFMALEID_PIC,
    FILE118_BFMALEID_PIC
};

int WIN_Register(void)
{
    wdlg_t v80;
    int v28, v2c, v1c, v20, v24, v30, v38;
    player_t vd8;

    v28 = 0;
    v2c = -1;
    v1c = -99;
    v20 = 0;

    PTR_DrawCursor(0);

    SND_PlaySong(88, 1, 1);
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 2);
    memset(&vd8, 0, sizeof(vd8));
    vd8.sweapon = -1;
    vd8.diff[0] = 2;
    vd8.diff[1] = 2;
    vd8.diff[2] = 2;
    vd8.diff[3] = 2;
    vd8.id_pic = 0;
    v24 = SWD_InitWindow(FILE137_REGISTER_SWD);
    SWD_SetFieldItem(v24, 0, sid_pics[vd8.id_pic]);
    SWD_SetActiveField(v24, 1);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 16);
    SWD_SetFieldPtr(v24, 5);
    PTR_DrawCursor(1);
    while (1)
    {
        SWD_Dialog(&v80);

        if (joy_ipt_MenuNew)                                                               //Controller Input WIN_Register
        {
            if (LeftShoulder)                                           
            {
                JOY_IsKey(LeftShoulder);
                v80.f_10 = 29;
            }
            if (RightShoulder)
            {
                JOY_IsKey(RightShoulder);
                v80.f_10 = 59;
            }
        }
        if (keyboard[1] || Back || BButton)
        {
            v20 = 0;
            fi_joy_count = 0;
            fi_sec_field = false;
            goto LAB_000244ea;
        }
        if (keyboard[45] && keyboard[56])
            WIN_AskExit();


        switch (v80.f_10)
        {
        case 0x3b:
            HELP_Win("NEWPLAY1_TXT");
            break;
        case 0x38:
        case 0x1d:
            v28++;
            v28 %= 4;
            SWD_SetFieldItem(v24, 0, sid_pics[v28]);
            vd8.id_pic = v28;
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
            break;
        }
        if (v80.f_30)
        {
            switch (v80.f_34)
            {
            case 4:
                v2c = v80.f_34;
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                       //Fixed ptr input
                {
                    while (IMS_IsAck())
                    {
                    }
                    if (RAP_IsSaveFile(&vd8))
                        WIN_Msg("Pilot NAME and CALLSIGN Used !");
                    else
                    {
                        v20 = 1;
                        goto LAB_000244ea;
                    }
                }
                if (v2c != v1c)
                {
                    SWD_SetFieldText(v24, 3, regtext[2]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v2c;
                }
                break;
            case 5:
                v2c = v80.f_34;
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                     //Fixed ptr input
                {
                    while (IMS_IsAck())
                    {
                    }
                    v28++;
                    v28 %= 4;
                    SWD_SetFieldItem(v24, 0, sid_pics[v28]);
                    vd8.id_pic = v28;
                }
                SWD_SetFieldText(v24, 3, regtext[1]);
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                v1c = v2c;
                break;
            case 6:
                v2c = v80.f_34;
                if (v2c != v1c)
                {
                    SWD_SetFieldText(v24, 3, regtext[0]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v2c;
                }
                break;
            }
        }
        else
        {
            v2c = -1;
            if (v2c != v1c)
            {
                SWD_SetFieldText(v24, 3, " ");
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                v1c = v2c;
            }
        }
        if (v80.f_8 == 1 && v80.f_c == 10)
        {
            switch (v80.f_4)
            {
            case 1:
                SWD_GetFieldText(v24, 1, vd8.name);
                if (strlen(vd8.name) != 0 && v80.f_10 == 0x1c)
                {
                    fi_sec_field = true;
                    SWD_SetActiveField(v24, 2);
                }
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                break;
            case 2:
                SWD_GetFieldText(v24, 2, vd8.callsign);
                SWD_GetFieldText(v24, 2, vd8.name);
                if (!strlen(vd8.name))
                {
                    fi_sec_field = false;
                    SWD_SetActiveField(v24, 1);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                else if (!strlen(vd8.callsign))
                {
                    SWD_SetActiveField(v24, 2);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                }
                else if (v80.f_10 == 0x1c || keyboard[28])
                {
                    if (RAP_IsSaveFile(&vd8))
                        WIN_Msg("Pilot NAME and CALLSIGN Used !");
                    else
                    {
                        fi_sec_field = false;
                        v20 = 1;
                        goto LAB_000244ea;
                    }
                }
                break;
            }
        }
    }
LAB_000244ea:
    SWD_GetFieldText(v24, 1, vd8.name);
    SWD_GetFieldText(v24, 2, vd8.callsign);
    if (!strlen(vd8.name))
    {
        SWD_SetActiveField(v24, 1);
        v20 = 0;
    }
    if (!strlen(vd8.callsign))
    {
        SWD_SetActiveField(v24, 2);
        v20 = 0;
    }
    v30 = 1;
    if (v20)
    {
        v30 = WIN_AskDiff();
        if (v30 >= 0)
        {
            ingameflag = 0;
            if (!RAP_FFSaveFile())
            {
                WIN_Msg("ERROR : YOU MUST DELETE A PILOT");
                v20 = 0;
            }
        }
        else
        {
            WIN_Msg("PLAYER ABORTED!");
            v20 = 0;
        }
    }
    if (v20)
    {
        ingameflag = 0;
        vd8.diff[0] = v30;
        vd8.diff[1] = v30;
        vd8.diff[2] = v30;
        if (v30 == 0)
        {
            vd8.trainflag = 1;
            vd8.fintrain = 0;
        }
        else
        {
            vd8.trainflag = 0;
            vd8.fintrain = 1;
        }
        memcpy(&player, &vd8, sizeof(player_t));
        RAP_SetPlayerDiff();
        OBJS_Add(0);
        OBJS_Add(16);
        OBJS_Add(16);
        OBJS_Add(16);
        player.score = 10000;
        if (godmode)
        {
            player.score += 0xd5fff;
            OBJS_Add(11);
            OBJS_Add(11);
            OBJS_Add(11);
            OBJS_Add(11);
            OBJS_Add(17);
            for (v38 = 1; v38 < 16; v38++)
                OBJS_Add(v38);
        }
        OBJS_GetNext();
    }
    PTR_DrawCursor(0);
    GFX_FadeOut(0, 0, 0, 16);
    SWD_DestroyWindow(v24);
    memset(displaybuffer, 0, 64000);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
    hangto = 0;
    if (v20)
        ingameflag = 0;
    return v20;
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

    if (player.trainflag)
    {
        player.trainflag = 0;
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
                    vd0.f_10 = 80;
                }
                if (StickY < 0)
                {
                    JOY_IsKey(StickY);
                    vd0.f_10 = 72;
                }
                if (StickX > 0)
                {
                    JOY_IsKey(StickX);
                    vd0.f_10 = 77;
                }
                if (StickX < 0)
                {
                    JOY_IsKey(StickX);
                    vd0.f_10 = 75;
                }
                if (Down)
                {
                    JOY_IsKey(Down);
                    vd0.f_10 = 80;
                }
                if (Up)
                {
                    JOY_IsKey(Up);
                    vd0.f_10 = 72;
                }
                if (Left)
                {
                    JOY_IsKey(Left);
                    vd0.f_10 = 75;
                }
                if (Right)
                {
                    JOY_IsKey(Right);
                    vd0.f_10 = 77;
                }
                if (AButton)
                {
                    JOY_IsKey(AButton);
                    vd0.f_10 = 28;
                }
                if (RightShoulder)
                {
                    JOY_IsKey(RightShoulder);
                    vd0.f_10 = 59;
                }
            }
            if (keyboard[1] || Back || BButton)
            {
                v2c = -99;
                goto LAB_00024d9c;
            }
            if (keyboard[45] && keyboard[56])
                WIN_AskExit();
            switch (vd0.f_10)
            {
            case 0x3b:
                HELP_Win("HANGHLP1_TXT");
                break;
            case 0x1f:
            case 0x3e:
                v34 = 3;
                sprintf(v88, "Save %s - %s ?", player.name, player.callsign);
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
                        vd0.f_34 = v2c;
                        vd0.f_30 = 1;
                    }
                    break;
                default:
                    v34 = 0;
                    break;
                }
            }
            if (vd0.f_30)
            {
LAB_00024b33:
                switch (vd0.f_34)
                {
                case 2:
                    v34 = 0;
                    v2c = vd0.f_34;
                    if ((mouseb1) || (vd0.f_10 == 0x1c) || (AButton && !joy_ipt_MenuNew))                //Fixed ptr input
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
                    v2c = vd0.f_34;
                    if ((mouseb1) || (vd0.f_10 == 0x1c) || (AButton && !joy_ipt_MenuNew))               //Fixed ptr input
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
                    v2c = vd0.f_34;
                    if ((mouseb1) || (vd0.f_10 == 0x1c) || (AButton && !joy_ipt_MenuNew))             //Fixed ptr input
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
                    v2c = vd0.f_34;
                    if ((mouseb1) || (vd0.f_10 == 0x1c) || (AButton && !joy_ipt_MenuNew))            //Fixed ptr input
                    {
                        while (IMS_IsAck())
                        {
                        }
                        sprintf(v88, "Save %s - %s ?", player.name, player.callsign);
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

int g_x, g_y, g_lx, g_ly;
int diff_wrap[4] = { 4, 9, 9, 9 };

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
    if (diff_wrap[player.diff[cur_game]] - 1 == game_wave[cur_game])
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
                v94.f_10 = 1;
            }
            if (BButton)
            {
                JOY_IsKey(BButton);
                v94.f_10 = 1;
            }
            if (RightShoulder)
            {
                JOY_IsKey(RightShoulder);
                v94.f_10 = 59;
            }
        }
        if (keyboard[45] && keyboard[56])
            WIN_AskExit();
        switch (v94.f_10)
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
        if (v94.f_8 == 1 && v94.f_c == 10)
        {
            switch (v94.f_4)
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

int songsg1[] = {
    0x60,0x5a,0x5c,0x5f,0x5e,0x5a,0x5b,0x5c,0x5e
};

int songsg2[] = {
    0x5b,0x5a,0x5c,0x60,0x5e,0x5a,0x60,0x59,0x5e
};

int songsg3[] = {
    0x60,0x5c,0x59,0x5f,0x5e,0x5a,0x5b,0x5c,0x5e
};

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
        v24 = diff_wrap[player.diff[cur_game]] - 1;
        if (game_wave[cur_game] == v24)
        {
            if (!player.diff[cur_game] && !player.fintrain)
            {
                OBJS_Init();
                player.sweapon = -1;
                player.fintrain = 1;
                player.score = 0;
                OBJS_Add(0);
                OBJS_Add(16);
                OBJS_Add(16);
                OBJS_Add(16);
                player.score = 0x2710;
                if (player.diff[cur_game] < 3)
                    player.diff[cur_game]++;
                for (v28 = 0; v28 < 4; v28++)
                    if (!player.diff[v28])
                        player.diff[v28] = 1;
            }
            else
            {
                if (player.diff[cur_game] < 3)
                    player.diff[cur_game]++;
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
        if (v6c.f_10 == 0x20 || YButton)                                    //Get Button to start Demo Mode
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
        if ((v6c.f_10 == 0x3b) || (JOY_IsKeyMenu(RightShoulder)))                                                             //Input Help Screen
            HELP_Win("HELP1_TXT");
        if (mouseb1 || mouseb2 || v6c.f_10 || (AButton && !joy_ipt_MenuNew))                                                  //Fixed ptr input
            FUN_00025c70(1);
        if (v6c.f_8 == 1 && v6c.f_c == 10)
        {
            FUN_00025c70(1);
            switch (v6c.f_4)
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

