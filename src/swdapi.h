#pragma once

#include "gfxapi.h"

struct swd_t
{
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    int f_28;
    int f_2c;
    char f_30[16]; // TODO: determine size
    int f_40;
    int f_44;
    int f_48;
    int f_4c; // field offset
    int f_50;
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
    int f_20;
    int f_24;
    int f_28;
    int f_2c;
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

struct wdlg_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
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

void SWD_Install(int a1);
void SWD_End(void);
void SWD_Dialog(wdlg_t *a1);
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
