#include <stdio.h>
#include "common.h"
#include "glbapi.h"
#include "rap.h"
#include "kbdapi.h"
#include "swdapi.h"
#include "gfxapi.h"
#include "fx.h"
#include "i_video.h"
#include "windows.h"
#include "joyapi.h"
#include "input.h"
#include "fileids.h"
#include "winids.h"

static int startitem;
static int curpage;
static int maxpages;

/***************************************************************************
HELP_Init() - inits the help stuff
 ***************************************************************************/
void 
HELP_Init(
    void
)
{
    int enditem;
    
    startitem = GLB_GetItemID("STARTHELP");
    enditem = GLB_GetItemID("ENDHELP");
    
    if (!reg_flag)
        startitem += 2;
    
    maxpages = enditem - startitem - 1;
    
    startitem++;
    
    curpage = 0;
}

/***************************************************************************
HELP_Win() - Displays the help window at the specified page
 ***************************************************************************/
void 
HELP_Win(
    const char *strpage        // INPUT : GLB string item
)
{
    wdlg_t dlg;
    char temp[20];
    int update, item, window;

    update = 1;
    item = GLB_GetItemID(strpage);
    
    if (item == -1)
        EXIT_Error("HELP() - Invalid Page");

    curpage = item - startitem;

    KBD_Clear();
    window = SWD_InitWindow(FILE138_HELP_SWD);
    
    SND_Patch(FX_DOOR, 127);
    
    while (1)
    {
        if (update)
        {
            update = 0;
            
            if (curpage >= 0)
                curpage = curpage % maxpages;
            else
                curpage += maxpages;
            
            SWD_SetFieldItem(window, HELP_TEXT, startitem + curpage);
            
            sprintf(temp, "PAGE : %02u", curpage + 1);
            SWD_SetFieldText(window, HELP_HEADER, temp);
            
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
        }
        
        SWD_Dialog(&dlg);
        
        switch (dlg.keypress)
        {
        case SC_HOME:
            update = 1;
            curpage = 0;
            break;
        
        case SC_F1:
            update = 1;
            curpage = 1;
            break;
        
        case SC_END:
            update = 1;
            curpage = maxpages - 1;
            break;
        
        case SC_DOWN:
        case SC_RIGHT:
        case SC_PAGEDN:
            update = 1;
            curpage++;
            break;
        
        case SC_UP:
        case SC_LEFT:
        case SC_PAGEUP:
            update = 1;
            curpage--;
            break;
        }
        
        if ((KBD_IsKey(SC_ESC)) || (JOY_IsKeyMenu(Back) && joy_ipt_MenuNew) || (JOY_IsKeyMenu(BButton) && joy_ipt_MenuNew))
            break;
        
        if (KBD_Key(SC_X) && KBD_Key(SC_ALT))
            WIN_AskExit();

        if (dlg.cur_act == S_FLD_COMMAND && dlg.cur_cmd == F_SELECT)
        {
            switch (dlg.field)
            {
            case HELP_DONE:
                goto func_exit;
            
            case HELP_DOWN:
                update = 1;
                curpage++;
                break;
            
            case HELP_UP:
                update = 1;
                curpage--;
                break;
            }
        }
    }

func_exit:
    
    SWD_DestroyWindow(window);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
}
