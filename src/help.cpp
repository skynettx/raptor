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

static int startitem;
static int curpage;
static int maxpages;

void HELP_Init(void)
{
    int v1c;
    startitem = GLB_GetItemID("STARTHELP");
    v1c = GLB_GetItemID("ENDHELP");
    if (!reg_flag)
        startitem += 2;
    maxpages = v1c - startitem - 1;
    startitem++;
    curpage = 0;
}

void HELP_Win(const char *a1)
{
    wdlg_t v84;
    char v3c[20];
    int v1c, v20, v24;

    v1c = 1;
    v20 = GLB_GetItemID(a1);
    if (v20 == -1)
        EXIT_Error("HELP() - Invalid Page");

    curpage = v20 - startitem;

    KBD_Clear();

    v24 = SWD_InitWindow(0x10038);
    SND_Patch(12, 127);
    while (1)
    {
        if (v1c)
        {
            v1c = 0;
            if (curpage >= 0)
                curpage = curpage % maxpages;
            else
                curpage += maxpages;
            SWD_SetFieldItem(v24, 8, startitem + curpage);
            sprintf(v3c, "PAGE : %02u", curpage + 1);
            SWD_SetFieldText(v24, 9, v3c);
            SWD_ShowAllWindows();
            GFX_DisplayUpdate();
        }
        SWD_Dialog(&v84);
        switch (v84.f_10)
        {
        case 0x47:
            v1c = 1;
            curpage = 0;
            break;
        case 0x3b:
            v1c = 1;
            curpage = 1;
            break;
        case 0x4f:
            v1c = 1;
            curpage = maxpages - 1;
            break;
        case 0x50:
        case 0x51:
        case 0x4d:
            v1c = 1;
            curpage++;
            break;
        case 0x48:
        case 0x49:
        case 0x4b:
            v1c = 1;
            curpage--;
            break;
        }
        if (KBD_IsKey(1))
            break;
        if (keyboard[45] && keyboard[56])
            WIN_AskExit();

        if (v84.f_8 == 1 && v84.f_c == 10)
        {
            switch (v84.f_4)
            {
            case 7:
                goto LAB_0002325c;
            case 6:
                v1c = 1;
                curpage++;
                break;
            case 5:
                v1c = 1;
                curpage--;
                break;
            }
        }
    }
LAB_0002325c:
    SWD_DestroyWindow(v24);
    SWD_ShowAllWindows();
    GFX_DisplayUpdate();
}
