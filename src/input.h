#pragma once

extern int buttons[4];
extern int control;

void IPT_LoadPrefs(void);
void IPT_GetButtons(void);
void IPT_Start(void);
void IPT_PauseControl(int a1);
void IPT_FMovePlayer(int a1, int a2);
void IPT_MovePlayer(void);
void IPT_End(void);
void IPT_Init(void);
