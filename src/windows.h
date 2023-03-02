#pragma once

enum DEMOTYPE
{
	DEM_INTRO,
	DEM_CREDITS,
	DEM_DEMO1G1,
	DEM_DEMO2G1,
	DEM_DEMO3G1,
	DEM_DEMO1G2,
	DEM_DEMO2G2,
	DEM_DEMO3G2,
	DEM_DEMO1G3,
	DEM_DEMO2G3,
	DEM_DEMO3G3
};

extern int opt_detail;
extern int songsg1[];
extern int songsg2[];
extern int songsg3[];

void WIN_Order(void);
void WIN_AskExit(void);
void WIN_MainMenu(void);
void WIN_MainLoop(void);
void WIN_Pause(void);
int WIN_AskBool(const char *question);
void WIN_WinGame(int game);
void WIN_Msg(const char *msg);
void WIN_SetLoadLevel(int level);
void WIN_EndLoad(void);
void WIN_LoadComp(void);
