#include <stdio.h>
#include <string.h>
#include "common.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "swdapi.h"
#include "rap.h"
#include "store.h"
#include "windows.h"
#include "objects.h"
#include "kbdapi.h"
#include "imsapi.h"
#include "ptrapi.h"
#include "help.h"
#include "fx.h"
#include "joyapi.h"
#include "input.h"
#include "fileids.h"

static int window;
int buy_count, s_count;
int buy_items[24], sell_items[24];
int id_pics[4] = {
    FILE111_WMALE_PIC, FILE114_BMALE_PIC, FILE112_WFEMALE_PIC, FILE113_BFEMALE_PIC
};

int mainbut[2] = {
    FILE124_BUTTON4_PIC, FILE135_SELLITEM_PIC
};

int buybut[2] = {
    FILE130_BUYLGT_PIC, FILE131_BUYDRK_PIC
};

int sellbut[2] = {
    FILE132_SELLGT_PIC, FILE133_SELLDRK_PIC
};

char saying[2][9] = {
    "COST",
    "RESALE"
};

int items[24] = {
    0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x40,0x41,0x42,0x43,
    0x44,0x45,0x46,0x47,0x48,0x49,0x4a,0x4b,-1,-1,-1,-1,-1,0
};

char storetext[8][20] = {
    "      EXIT",
    "ACTIVATE SELL MODE",
    "ACTIVATE BUY MODE",
    "    NEXT ITEM",
    "  PREVIOUS ITEM",
    "  BUY AN ITEM",
    "  SELL AN ITEM",
    " HDE MAIN DISPLAY"
};

char yh_hold[16];

static int cur_item, mode;

int MakeBuyItems(void)
{
    int i, v24, v28, v2c, v20;
    buy_count = 0;
    memset(buy_items, 0, sizeof(buy_items));
    for (i = 0; i < 18; i++)
    {
        if (OBJS_CanBuy(i))
        {
            buy_items[buy_count] = i;
            buy_count++;
        }
    }
    if (buy_count > 1)
    {
        do
        {
            v24 = 0;
            for (i = 0; i < buy_count - 1; i++)
            {
                v28 = OBJS_GetCost(buy_items[i]);
                v2c = OBJS_GetCost(buy_items[i+1]);
                if (v28 > v2c)
                {
                    v24 = 1;
                    v20 = buy_items[i];
                    buy_items[i] = buy_items[i + 1];
                    buy_items[i + 1] = v20;
                }
            }
        } while (v24);
    }
    return buy_count;
}

int MakeSellItems(void)
{
    int i, v24, v28, v2c, v20;
    memset(sell_items, 0, sizeof(sell_items));
    s_count = 0;
    for (i = 0; i < 18; i++)
    {
        if (OBJS_CanSell(i))
        {
            sell_items[s_count] = i;
            s_count++;
        }
    }
    if (s_count > 1)
    {
        do
        {
            v24 = 0;
            for (i = 0; i < s_count - 1; i++)
            {
                v28 = OBJS_GetCost(sell_items[i]);
                v2c = OBJS_GetCost(sell_items[i+1]);
                if (v28 > v2c)
                {
                    v24 = 1;
                    v20 = sell_items[i];
                    sell_items[i] = sell_items[i + 1];
                    sell_items[i + 1] = v20;
                }
            }
        } while (v24);
    }
    return s_count;
}


void Harrold(int a1)
{
    int v1c, v20, v24, v28, v2c;
    v1c = SWD_GetFieldItem(window, 2);
    v20 = SWD_GetFieldItem(window, 3);
    v24 = SWD_GetFieldItem(window, 4);
    v28 = SWD_GetFieldItem(window, 5);
    v2c = SWD_GetFieldItem(window, 6);
    SWD_SetFieldItem(window, 2, -1);
    SWD_SetFieldItem(window, 3, -1);
    SWD_SetFieldItem(window, 4, -1);
    SWD_SetFieldItem(window, 5, -1);
    SWD_SetFieldItem(window, 6, -1);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    KBD_Clear();
    SWD_SetFieldText(window, 11, 0);
    SWD_SetFieldText(window, 12, 0);
    SWD_SetFieldText(window, 13, 0);
    SWD_SetFieldText(window, 9, 0);
    SWD_SetFieldItem(window, 7, a1);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    IMS_WaitTimedSwd(10);
    IMS_WaitTimedSwd(10);
    SWD_SetFieldItem(window, 7, -1);
    SWD_SetFieldItem(window, 2, v1c);
    SWD_SetFieldItem(window, 3, v20);
    SWD_SetFieldItem(window, 4, v24);
    SWD_SetFieldItem(window, 5, v28);
    SWD_SetFieldItem(window, 6, v2c);
}

void STORE_Enter(void)
{
    int v24, v28, v1c, v30, v2c, v34, v38, v3c;
    wdlg_t vec;
    char v70[50], va4[50];

    v24 = 0;
    v28 = -1;
    v1c = -1;
    v30 = 0;
    PTR_DrawCursor(0);
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 5);
    g_button_flag = 0;
    window = SWD_InitMasterWindow(FILE131_STORE_SWD);
    SWD_SetFieldItem(window, 0, id_pics[player.id_pic]);
    SWD_SetFieldItem(window, 5, mainbut[mode]);
    SWD_GetFieldText(window, 11, yh_hold);
    SWD_SetFieldText(window, 11, NULL);
    SWD_SetFieldText(window, 1, player.callsign);
    sprintf(v70, "%07d", player.score);
    SWD_SetFieldText(window, 10, v70);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 0x10);
    SWD_SetFieldPtr(window, 0x14);
    PTR_DrawCursor(1);
    OBJS_GetNum();
    Harrold(0x4c);
    cur_item = 0;
    mode = 0;
    if (mode == 0)
    {
        SWD_SetFieldItem(window, 4, buybut[0]);
        SWD_SetFieldItem(window, 6, sellbut[1]);
    }
    else
    {
        SWD_SetFieldItem(window, 4, buybut[1]);
        SWD_SetFieldItem(window, 6, sellbut[0]);
    }
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    v24 = 1;
    while (1)
    {
        MakeSellItems();
        MakeBuyItems();
        if (mode == 0)
            v30 = buy_count;
        else
            v30 = s_count;
        if (v30 < 1)
        {
            v24 = 1;
            if (mode == 0)
                EXIT_Error("STORE Error ( BUY_MODE )");
            else
            {
                Harrold(80);
                mode = 0;
                v30 = MakeBuyItems();
                if (v30 < 1)
                    EXIT_Error("STORE THING 2");
            }
        }
        if (v24)
        {
            v24 = 0;
            if (cur_item < 0)
                cur_item = v30 - 1;
            if (cur_item >= v30)
                cur_item = 0;
            if (mode == 0)
            {
                v2c = buy_items[cur_item];
                SWD_SetFieldItem(window, 4, buybut[0]);
                SWD_SetFieldItem(window, 6, sellbut[1]);
                v34 = OBJS_GetCost(v2c);
                if (OBJS_IsOnly(v2c))
                    v3c = OBJS_GetAmt(v2c);
                else
                    v3c = OBJS_GetTotal(v2c);
                sprintf(v70, "%02d", v3c);
                sprintf(va4, "%02d", v34);
            }
            else
            {
                v2c = sell_items[cur_item];
                SWD_SetFieldItem(window, 4, buybut[1]);
                SWD_SetFieldItem(window, 6, sellbut[0]);
                v34 = OBJS_GetResale(v2c);
                if (OBJS_IsOnly(v2c))
                    v3c = OBJS_GetAmt(v2c);
                else
                    v3c = OBJS_GetTotal(v2c);
                sprintf(v70, "%02d", v3c);
                sprintf(va4, "%02d", v34);
            }
            SWD_SetFieldText(window, 11, yh_hold);
            SWD_SetFieldText(window, 12, saying[mode]);
            SWD_SetFieldText(window, 13, v70);
            SWD_SetFieldText(window, 9, va4);
            sprintf(v70, "%07d", player.score);
            SWD_SetFieldText(window, 10, v70);
            SWD_SetFieldItem(window, 5, FILE134_BUYITEM_PIC);
            if (v2c < 24)
                SWD_SetFieldItem(window, 7, items[v2c]);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
        }
        SWD_Dialog(&vec);
        if (keyboard[45] && keyboard[56])
            WIN_AskExit();
        if (vec.f_30)
        {
            switch (vec.f_34)
            {
            case 20:
                v28 = vec.f_34;
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                  //Fixed ptr input
                    goto LAB_000209e5;
                if (v28 != v1c)
                {
                    SWD_SetFieldText(window, 8, storetext[0]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v28;
                }
                break;
            case 14:
                v28 = vec.f_34;
                if (v28 != v1c)
                {
                    SWD_SetFieldText(window, 8, storetext[2]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v28;
                }
                break;
            case 19:
                v28 = vec.f_34;
                if (v28 != v1c)
                {
                    SWD_SetFieldText(window, 8, storetext[1]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v28;
                }
                break;
            case 16:
                v28 = vec.f_34;
                if (v28 != v1c)
                {
                    SWD_SetFieldText(window, 8, storetext[3]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v28;
                }
                break;
            case 15:
                v28 = vec.f_34;
                if (v28 != v1c)
                {
                    SWD_SetFieldText(window, 8, storetext[4]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v28;
                }
                break;
            case 18:
                v28 = vec.f_34;
                if (v28 != v1c)
                {
                    if (mode == 0)
                        SWD_SetFieldText(window, 8, storetext[5]);
                    else
                        SWD_SetFieldText(window, 8, storetext[6]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v28;
                }
                break;
            case 17:
                v28 = vec.f_34;
                if (v28 != v1c)
                {
                    SWD_SetFieldText(window, 8, storetext[7]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    v1c = v28;
                }
                break;
            }
        }
        else
        {
            v28 = -1;
            if (v28 != v1c)
            {
                SWD_SetFieldText(window, 8, " ");
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                v1c = v28;
            }
        }
        if (joy_ipt_MenuNew)
        {
            if (StickY > 0)                                                   //Controller Input Store
            {
                JOY_IsKey(StickY);
                vec.f_10 = 80;
            }
            if (StickY < 0)
            {
                JOY_IsKey(StickY);
                vec.f_10 = 72;
            }
            if (StickX > 0)
            {
                JOY_IsKey(StickX);
                vec.f_10 = 77;
            }
            if (StickX < 0)
            {
                JOY_IsKey(StickX);
                vec.f_10 = 75;
            }
            if (Down)
            {
                JOY_IsKey(Down);
                vec.f_10 = 80;
            }
            if (Up)
            {
                JOY_IsKey(Up);
                vec.f_10 = 72;
            }
            if (Left)
            {
                JOY_IsKey(Left);
                vec.f_10 = 75;
            }
            if (Right)
            {
                JOY_IsKey(Right);
                vec.f_10 = 77;
            }
            if (AButton)
            {
                JOY_IsKey(AButton);
                vec.f_10 = 28;
            }
            if (Back)
            {
                JOY_IsKey(Back);
                vec.f_10 = 1;
            }
            if (BButton)
            {
                JOY_IsKey(BButton);
                vec.f_10 = 1;
            }
            if (LeftShoulder)
            {
                JOY_IsKey(LeftShoulder);
                vec.f_10 = 57;
            }
            if (RightShoulder)
            {
                JOY_IsKey(RightShoulder);
                vec.f_10 = 59;
            }
        }
        switch (vec.f_10)
        {
        case 1:
            goto LAB_000209e5;
        case 0x3b:
            HELP_Win("STORHLP1_TXT");
            break;
        case 0x39:
            KBD_Wait(vec.f_10);
            mode ^= 1;
            if (mode == 0)
            {
                vec.f_8 = 1;
                vec.f_c = 10;
                vec.f_4 = 4;
            }
            else
            {
                vec.f_8 = 1;
                vec.f_c = 10;
                vec.f_4 = 6;
            }
            break;
        case 0x1c:
            KBD_Wait(vec.f_10);
            vec.f_8 = 1;
            vec.f_c = 10;
            vec.f_4 = 5;
            break;
        case 0x48:
        case 0x49:
        case 0x4d:
            KBD_Wait(vec.f_10);
            vec.f_8 = 1;
            vec.f_c = 10;
            vec.f_4 = 3;
            break;
        case 0x4b:
        case 0x50:
        case 0x51:
            KBD_Wait(vec.f_10);
            vec.f_8 = 1;
            vec.f_c = 10;
            vec.f_4 = 2;
            break;
        }
        if (vec.f_8 == 1 && vec.f_c == 10)
        {
            v24 = 1;
            switch (vec.f_4)
            {
            case 3:
                SND_Patch(20, 127);
                if (v30)
                    cur_item++;
                break;
            case 2:
                SND_Patch(20, 127);
                if (v30)
                    cur_item--;
                break;
            case 4:
                SND_Patch(20, 127);
                mode = 0;
                v30 = MakeBuyItems();
                cur_item = 0;
                break;
            case 6:
                SND_Patch(20, 127);
                mode = 1;
                v30 = MakeSellItems();
                cur_item = 0;
                break;
            case 5:
                if (mode == 0)
                {
                    v2c = buy_items[cur_item];
                    switch (OBJS_Buy(v2c))
                    {
                    case 0:
                        SND_Patch(20, 127);
                        break;
                    case 1:
                        SND_Patch(22, 127);
                        Harrold(0x4f);
                        break;
                    case 2:
                        SND_Patch(22, 127);
                        Harrold(0x51);
                        break;
                    case 3:
                        SND_Patch(22, 127);
                        break;
                    }
                    MakeBuyItems();
                    for (v38 = 0; v38 < buy_count; v38++)
                    {
                        if (buy_items[v38] == v2c)
                            cur_item = v38;
                        break;
                    }
                }
                else
                {
                    v2c = sell_items[cur_item];
                    OBJS_Sell(v2c);
                    MakeSellItems();
                    SND_Patch(20, 127);
                    for (v38 = 0; v38 < s_count; v38++)
                    {
                        if (sell_items[v38] == v2c)
                            cur_item = v38;
                        break;
                    }
                }
                break;
            }
        }
    }
LAB_000209e5:
    SND_Patch(12, 127);
    while (IMS_IsAck())
    {
    }
    g_button_flag = 1;
    GFX_FadeOut(0, 0, 0, 16);
    PTR_DrawCursor(0);
    SWD_DestroyWindow(window);
    memset(displaybuffer, 0, 64000);
    GFX_DisplayUpdate();
    GFX_SetPalette(palette, 0);
}
