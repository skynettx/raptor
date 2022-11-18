#pragma once
#include "common.h"
#include "gfxapi.h"

enum PTRTYPE
{
	P_AUTO,
	P_MOUSE,
	P_JOYSTICK
};

extern int mouseb1, mouseb2, mouseb3;
extern int cur_mx, cur_my;
extern int ptractive;
extern int mouse_b1_ack, mouse_b2_ack, mouse_b3_ack;
extern int g_drawcursor;

void PTR_JoyHandler(void);
void PTR_MouseHandler(void);
void PTR_ResetJoyStick(void);
void PTR_SetPos(int x, int y);
void PTR_Pause(int flag);
void PTR_DrawCursor(int flag);
void PTR_SetPic(texture_t *newp);
int PTR_Init(int type);

void PTR_Erase(void);
void PTR_ClipErase(void);
void PTR_Save(void);
void PTR_ClipSave(void);
void PTR_Draw(void);
void PTR_UpdateCursor(void);
