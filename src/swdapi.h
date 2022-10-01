#pragma once

#include "gfxapi.h"

enum CMD
{
    C_IDLE,
    F_DOWN,
    F_UP,
    F_NEXT,
    F_PREV,
    F_RIGHT,
    F_LEFT,
    F_TOP,
    F_BOTTOM,
    F_FIRST,
    F_SELECT,
    F_OBJ_AREA,
    F_VIEW_AREA,
    W_NEW,
    W_NEXT,
    W_MOVE,
    W_CLOSE,
    W_CLOSE_ALL
};

enum ACT
{
    S_IDLE,
    S_FLD_COMMAND,
    S_WIN_COMMAND,
    S_FIRSTCALL,
    S_REDRAW,
    S_UPDATE,
    S_END
};

struct swd_t
{
    int f_0;     //not used
    int f_4;     //not used
    int f_8;
    int f_c;
    int f_10;    //not used
    int f_14;    //not used
    int f_18;
    int f_1c;
    //int f_20;
    //int f_24;
    //int f_28;
    //int f_2c;
    char Name[16];
    char f_30[16]; // TODO: determine size
    int f_40;
    int f_44;
    int f_48;
    int f_4c; // field offset
    int f_50;    //not used
    int f_54; // active field
    int f_58;
    int f_5c;
    int f_60;
    int f_64;
    int f_68;
    int f_6c;
    int f_70;
    int f_74;
};

struct swdfield_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    //int f_20;
    //int f_24;
    //int f_28;
    //int f_2c;
    char Name[16];
    char f_30[16];
    int f_40;
    char f_44[16];
    int f_54;
    int f_58;
    int f_5c;
    int f_60;
    int f_64;
    int f_68;
    int f_6c;
    int f_70;
    int f_74;
    int f_78;
    int f_7c;
    int f_80;
    int f_84;
    int f_88;
    int f_8c;
    texture_t *f_90;
}; // size: 0x94

struct swdfield_32_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    //int f_20;
    //int f_24;
    //int f_28;
    //int f_2c;
    char Name[16];
    char f_30[16];
    int f_40;
    char f_44[16];
    int f_54;
    int f_58;
    int f_5c;
    int f_60;
    int f_64;
    int f_68;
    int f_6c;
    int f_70;
    int f_74;
    int f_78;
    int f_7c;
    int f_80;
    int f_84;
    int f_88;
    int f_8c;
    int PlaceHolder;
}; // size: 0x94

struct wdlg_t {
    int f_0;
    int field;
    int cur_act;
    int cur_cmd;
    int keypress;
    int f_14;
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    int f_28;
    int f_2c;
    int f_30;
    int f_34;
    int f_38;
    int f_3c;
};

struct window_t {
    int f_0;
    int f_4;
    int f_8;
    swd_t *f_c;
};

extern int usekb_flag;
extern int g_button_flag;
extern unsigned int fi_joy_count;
extern bool fi_sec_field;

void SWD_Install(int a1);
void SWD_End(void);
void SWD_Dialog(wdlg_t *a1);
swd_t* SWD_ReformatFieldData(swd_t* swdData, int a1);
int SWD_InitMasterWindow(int a1);
int SWD_ShowAllWindows(void);
void SWD_SetWindowPtr(int a1);
void SWD_SetFieldSelect(int a1, int a2, int a3);
void SWD_SetFieldItem(int a1, int a2, int a3);
int SWD_SetFieldText(int a1, int a2, const char *a3);
void SWD_DestroyWindow(int a1);
int SWD_InitWindow(int a1);
void SWD_SetActiveField(int a1, int a2);
int SWD_GetFieldXYL(int a1, int a2, int* a3, int* a4, int* a5, int* a6);
void SWD_SetWinDrawFunc(int a1, void (*a2)(wdlg_t*));
void SWD_SetFieldPtr(int a1, int a2);
int SWD_GetFieldText(int a1, int a2, char *a3);
int SWD_GetFieldItem(int a1, int a2);
void SWD_SetClearFlag(int a1);
