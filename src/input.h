#pragma once

enum ITYPE
{
	I_KEYBOARD,
	I_MOUSE,
	I_JOYSTICK,
	I_FORCE
};

extern int buttons[4];
extern int control;
extern int haptic;
extern int joy_ipt_MenuNew;

void IPT_LoadPrefs(void);
void IPT_GetButtons(void);
void IPT_Start(void);
void IPT_PauseControl(int flag);
void IPT_FMovePlayer(int addx, int addy);
void IPT_MovePlayer(void);
void IPT_End(void);
void IPT_Init(void);
