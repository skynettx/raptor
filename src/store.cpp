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
#include "winids.h"

#define BUY_MODE  0
#define SELL_MODE 1

static int window;
int buy_count, sell_count;
int b_items[S_LAST_OBJECT], s_items[S_LAST_OBJECT];
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

int items[S_LAST_OBJECT] = {
    FILE03a_ITEM00_TXT,
    FILE03b_ITEM01_TXT,
    FILE03c_ITEM02_TXT,
    FILE03d_ITEM03_TXT,
    FILE03e_ITEM04_TXT,
    FILE03f_ITEM05_TXT,
    FILE040_ITEM06_TXT,
    FILE041_ITEM07_TXT,
    FILE042_ITEM08_TXT,
    FILE043_ITEM09_TXT,
    FILE044_ITEM10_TXT,
    FILE045_ITEM11_TXT,
    FILE046_ITEM12_TXT,
    FILE047_ITEM13_TXT,
    FILE048_ITEM14_TXT,
    FILE049_ITEM15_TXT,
    FILE04a_ITEM16_TXT,
    FILE04b_ITEM17_TXT,
    -1,
    -1,
    -1,
    -1,
    -1,
    0
};

char storetext[8][20] = {
    "      EXIT",           //0
    "ACTIVATE SELL MODE",   //1
    "ACTIVATE BUY MODE",    //2
    "    NEXT ITEM",        //3
    "  PREVIOUS ITEM",      //4
    "  BUY AN ITEM",        //5
    "  SELL AN ITEM",       //6
    " HDE MAIN DISPLAY"     //7
};

char yh_hold[16];

static int cur_item, mode;

/*-------------------------------------------------------------------------*
MakeBuyItems () - Makes items you can Buy
 *-------------------------------------------------------------------------*/
int 
MakeBuyItems(
    void
)
{
    int loop, flag, cost1, cost2, num;
    
    buy_count = 0;
    memset(b_items, 0, sizeof(b_items));
    
    for (loop = 0; loop < S_ITEMBUY1; loop++)
    {
        if (OBJS_CanBuy(loop))
        {
            b_items[buy_count] = loop;
            buy_count++;
        }
    }
    
    // SORT BY $ AMOUNT ============================
    
    if (buy_count > 1)
    {
        do
        {
            flag = 0;
            for (loop = 0; loop < buy_count - 1; loop++)
            {
                cost1 = OBJS_GetCost(b_items[loop]);
                cost2 = OBJS_GetCost(b_items[loop+1]);
                
                if (cost1 > cost2)
                {
                    flag = 1;
                    num = b_items[loop];
                    b_items[loop] = b_items[loop + 1];
                    b_items[loop + 1] = num;
                }
            }
        } while (flag);
    }
    
    return buy_count;
}

/*-------------------------------------------------------------------------*
MakeSellItems() - Makes the items that you can sell
 *-------------------------------------------------------------------------*/
int 
MakeSellItems(
    void
)
{
    int loop, flag, cost1, cost2, num;
    
    memset(s_items, 0, sizeof(s_items));
    
    sell_count = 0;
    for (loop = 0; loop < S_LAST_OBJECT; loop++)
    {
        if (OBJS_CanSell(loop))
        {
            s_items[sell_count] = loop;
            sell_count++;
        }
    }
    
    // SORT BY $ AMOUNT ============================
    
    if (sell_count > 1)
    {
        do
        {
            flag = 0;
            for (loop = 0; loop < sell_count - 1; loop++)
            {
                cost1 = OBJS_GetCost(s_items[loop]);
                cost2 = OBJS_GetCost(s_items[loop+1]);
                
                if (cost1 > cost2)
                {
                    flag = 1;
                    num = s_items[loop];
                    s_items[loop] = s_items[loop + 1];
                    s_items[loop + 1] = num;
                }
            }
        } while (flag);
    }
    
    return sell_count;
}

/*-------------------------------------------------------------------------*
Harrold() - Lets Harrold do Some Talking
 *-------------------------------------------------------------------------*/
void 
Harrold(
    int item               // INPUT : GLB item of harrold text
)
{
    int prev, next, buy, buyit, sell;
    
    prev = SWD_GetFieldItem(window, STOR_PREV);
    next = SWD_GetFieldItem(window, STOR_NEXT);
    buy = SWD_GetFieldItem(window, STOR_BUY);
    buyit = SWD_GetFieldItem(window, STOR_BUYIT);
    sell = SWD_GetFieldItem(window, STOR_SELL);
    
    SWD_SetFieldItem(window, STOR_PREV, -1);
    SWD_SetFieldItem(window, STOR_NEXT, -1);
    SWD_SetFieldItem(window, STOR_BUY, -1);
    SWD_SetFieldItem(window, STOR_BUYIT, -1);
    SWD_SetFieldItem(window, STOR_SELL, -1);
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    KBD_Clear();
    
    SWD_SetFieldText(window, STOR_STATS, 0);
    SWD_SetFieldText(window, STOR_TEXTCOST, 0);
    SWD_SetFieldText(window, STOR_NUM, 0);
    SWD_SetFieldText(window, STOR_COST, 0);
    SWD_SetFieldItem(window, STOR_COMP, item);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    
    IMS_WaitTimed(10);
    
    SWD_SetFieldItem(window, STOR_COMP, -1);
    
    SWD_SetFieldItem(window, STOR_PREV, prev);
    SWD_SetFieldItem(window, STOR_NEXT, next);
    SWD_SetFieldItem(window, STOR_BUY, buy);
    SWD_SetFieldItem(window, STOR_BUYIT, buyit);
    SWD_SetFieldItem(window, STOR_SELL, sell);
}

/***************************************************************************
STORE_Enter () - Lets User go in store and buy and sell things
 ***************************************************************************/
void 
STORE_Enter(
    void
)
{
    int update, opt, oldopt, max_items, pos, cost, loop, num;
    wdlg_t dlg;
    char youhave[50], coststr[50];

    update = 0;
    opt = -1;
    oldopt = -1;
    max_items = 0;
    
    PTR_DrawCursor(0);
    KBD_Clear();
    GFX_FadeOut(0, 0, 0, 5);
    
    g_button_flag = 0;
    window = SWD_InitMasterWindow(FILE131_STORE_SWD);
    SWD_SetFieldItem(window, STOR_ID, id_pics[plr.id_pic]);
    SWD_SetFieldItem(window, STOR_BUYIT, mainbut[mode]);
    SWD_GetFieldText(window, STOR_STATS, yh_hold);
    SWD_SetFieldText(window, STOR_STATS, NULL);
    SWD_SetFieldText(window, STOR_CALLSIGN, plr.callsign);
    sprintf(youhave, "%07d", plr.score);
    SWD_SetFieldText(window, STOR_SCORE, youhave);
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    GFX_FadeIn(palette, 16);
    
    SWD_SetFieldPtr(window, STOR_VEXIT);
    PTR_DrawCursor(1);
    
    OBJS_GetNum();
    
    Harrold(FILE04c_HAR1_TXT);
    
    cur_item = 0;
    mode = BUY_MODE;
    
    if (mode == BUY_MODE)
    {
        SWD_SetFieldItem(window, STOR_BUY, buybut[0]);
        SWD_SetFieldItem(window, STOR_SELL, sellbut[1]);
    }
    else
    {
        SWD_SetFieldItem(window, STOR_BUY, buybut[1]);
        SWD_SetFieldItem(window, STOR_SELL, sellbut[0]);
    }
    
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
    update = 1;
    
    while (1)
    {
        MakeSellItems();
        MakeBuyItems();
        
        if (mode == BUY_MODE)
            max_items = buy_count;
        else
            max_items = sell_count;
        
        if (max_items < 1)
        {
            update = 1;
            
            if (mode == 0)
                EXIT_Error("STORE Error ( BUY_MODE )");
            else
            {
                Harrold(FILE050_HAR5_TXT);
                mode = BUY_MODE;
                max_items = MakeBuyItems();
                if (max_items < 1)
                    EXIT_Error("STORE THING 2");
            }
        }
        
        if (update)
        {
            update = 0;
            
            if (cur_item < 0)
                cur_item = max_items - 1;
            
            if (cur_item >= max_items)
                cur_item = 0;
            
            if (mode == BUY_MODE)
            {
                pos = b_items[cur_item];
                SWD_SetFieldItem(window, STOR_BUY, buybut[0]);
                SWD_SetFieldItem(window, STOR_SELL, sellbut[1]);
                cost = OBJS_GetCost(pos);
                if (OBJS_IsOnly(pos))
                    num = OBJS_GetAmt(pos);
                else
                    num = OBJS_GetTotal(pos);
                sprintf(youhave, "%02d", num);
                sprintf(coststr, "%02d", cost);
            }
            else
            {
                pos = s_items[cur_item];
                SWD_SetFieldItem(window, STOR_BUY, buybut[1]);
                SWD_SetFieldItem(window, STOR_SELL, sellbut[0]);
                cost = OBJS_GetResale(pos);
                if (OBJS_IsOnly(pos))
                    num = OBJS_GetAmt(pos);
                else
                    num = OBJS_GetTotal(pos);
                sprintf(youhave, "%02d", num);
                sprintf(coststr, "%02d", cost);
            }
            
            SWD_SetFieldText(window, STOR_STATS, yh_hold);
            SWD_SetFieldText(window, STOR_TEXTCOST, saying[mode]);
            SWD_SetFieldText(window, STOR_NUM, youhave);
            SWD_SetFieldText(window, STOR_COST, coststr);
            sprintf(youhave, "%07d", plr.score);
            SWD_SetFieldText(window, STOR_SCORE, youhave);
            
            SWD_SetFieldItem(window, STOR_BUYIT, FILE134_BUYITEM_PIC);
            
            if (pos < S_LAST_OBJECT)
                SWD_SetFieldItem(window, STOR_COMP, items[pos]);
            
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
        }
        
        SWD_Dialog(&dlg);
        
        if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
            WIN_AskExit();
        
        if (dlg.viewactive)
        {
            switch (dlg.sfield)
            {
            case STOR_VEXIT:
                opt = dlg.sfield;
                if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                                  //Fixed ptr input
                    goto store_exit;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, STOR_TEXT, storetext[0]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            
            case STOR_VBUY:
                opt = dlg.sfield;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, STOR_TEXT, storetext[2]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            
            case STOR_VSELL:
                opt = dlg.sfield;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, STOR_TEXT, storetext[1]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            
            case STOR_VNEXT:
                opt = dlg.sfield;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, STOR_TEXT, storetext[3]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            
            case STOR_VPREV:
                opt = dlg.sfield;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, STOR_TEXT, storetext[4]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            
            case STOR_VACCEPT:
                opt = dlg.sfield;
                if (opt != oldopt)
                {
                    if (mode == BUY_MODE)
                        SWD_SetFieldText(window, STOR_TEXT, storetext[5]);
                    else
                        SWD_SetFieldText(window, STOR_TEXT, storetext[6]);
                    SWD_ShowAllWindows();
                    GFX_DisplayUpdate();
                    oldopt = opt;
                }
                break;
            
            case STOR_VSCREEN:
                opt = dlg.sfield;
                if (opt != oldopt)
                {
                    SWD_SetFieldText(window, STOR_TEXT, storetext[7]);
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
                SWD_SetFieldText(window, STOR_TEXT, " ");
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
                oldopt = opt;
            }
        }
        
        if (joy_ipt_MenuNew)
        {
            if (StickY > 0)                                                   //Controller Input Store
            {
                JOY_IsKey(StickY);
                dlg.keypress = SC_DOWN;
            }
            if (StickY < 0)
            {
                JOY_IsKey(StickY);
                dlg.keypress = SC_UP;
            }
            if (StickX > 0)
            {
                JOY_IsKey(StickX);
                dlg.keypress = SC_RIGHT;
            }
            if (StickX < 0)
            {
                JOY_IsKey(StickX);
                dlg.keypress = SC_LEFT;
            }
            if (Down)
            {
                JOY_IsKey(Down);
                dlg.keypress = SC_DOWN;
            }
            if (Up)
            {
                JOY_IsKey(Up);
                dlg.keypress = SC_UP;
            }
            if (Left)
            {
                JOY_IsKey(Left);
                dlg.keypress = SC_LEFT;
            }
            if (Right)
            {
                JOY_IsKey(Right);
                dlg.keypress = SC_RIGHT;
            }
            if (AButton)
            {
                JOY_IsKey(AButton);
                dlg.keypress = SC_ENTER;
            }
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
            if (LeftShoulder)
            {
                JOY_IsKey(LeftShoulder);
                dlg.keypress = SC_SPACE;
            }
            if (RightShoulder)
            {
                JOY_IsKey(RightShoulder);
                dlg.keypress = SC_F1;
            }
        }
        
        switch (dlg.keypress)
        {
        case SC_ESC:
            goto store_exit;
        case SC_F1:
            HELP_Win("STORHLP1_TXT");
            break;
        
        case SC_SPACE:
            KBD_Wait(dlg.keypress);
            mode ^= SELL_MODE;
            if (mode == BUY_MODE)
            {
                dlg.cur_act = S_FLD_COMMAND;
                dlg.cur_cmd = F_SELECT;
                dlg.field = STOR_BUY;
            }
            else
            {
                dlg.cur_act = S_FLD_COMMAND;
                dlg.cur_cmd = F_SELECT;
                dlg.field = STOR_SELL;
            }
            break;
        
        case SC_ENTER:
            KBD_Wait(dlg.keypress);
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = STOR_BUYIT;
            break;
        
        case SC_UP:
        case SC_PAGEUP:
        case SC_RIGHT:
            KBD_Wait(dlg.keypress);
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = STOR_NEXT;
            break;
        
        case SC_LEFT:
        case SC_DOWN:
        case SC_PAGEDN:
            KBD_Wait(dlg.keypress);
            dlg.cur_act = S_FLD_COMMAND;
            dlg.cur_cmd = F_SELECT;
            dlg.field = STOR_PREV;
            break;
        }
        
        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            update = 1;
            switch (dlg.field)
            {
            case STOR_NEXT:
                SND_Patch(FX_SWEP, 127);
                if (max_items)
                    cur_item++;
                break;
            
            case STOR_PREV:
                SND_Patch(FX_SWEP, 127);
                if (max_items)
                    cur_item--;
                break;
            
            case STOR_BUY:
                SND_Patch(FX_SWEP, 127);
                mode = BUY_MODE;
                max_items = MakeBuyItems();
                cur_item = 0;
                break;
            
            case STOR_SELL:
                SND_Patch(FX_SWEP, 127);
                mode = SELL_MODE;
                max_items = MakeSellItems();
                cur_item = 0;
                break;
            
            case STOR_BUYIT:
                if (mode == BUY_MODE)
                {
                    pos = b_items[cur_item];
                    switch (OBJS_Buy(pos))
                    {
                    case OBJ_GOTIT:
                        SND_Patch(FX_SWEP, 127);
                        break;
                    
                    case OBJ_NOMONEY:
                        SND_Patch(FX_WARNING, 127);
                        Harrold(FILE04f_HAR4_TXT);
                        break;
                    
                    case OBJ_SHIPFULL:
                        SND_Patch(FX_WARNING, 127);
                        Harrold(FILE051_HAR7_TXT);
                        break;
                    
                    case OBJ_ERROR:
                        SND_Patch(FX_WARNING, 127);
                        break;
                    }
                    MakeBuyItems();
                    for (loop = 0; loop < buy_count; loop++)
                    {
                        if (b_items[loop] == pos)
                            cur_item = loop;
                        break;
                    }
                }
                else
                {
                    pos = s_items[cur_item];
                    OBJS_Sell(pos);
                    MakeSellItems();
                    SND_Patch(FX_SWEP, 127);
                    for (loop = 0; loop < sell_count; loop++)
                    {
                        if (s_items[loop] == pos)
                            cur_item = loop;
                        break;
                    }
                }
                break;
            }
        }
    }

store_exit:
    SND_Patch(FX_DOOR, 127);
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
