#pragma once

#define MAX_SHIELD 100
#define MAX_SUPER  100

extern int reg_flag;
extern int bday_num;
extern int demo_mode;
extern int demo_flag;
extern char *palette;
extern int ingameflag;
extern int gameflag[4];
extern int godmode;
extern int cur_game;
extern int game_wave[4];
extern int cur_diff;
extern int bday_flag;
extern int g_mapleft;
extern int g_flash;
extern int o_gun1[8];
extern int o_gun2[8];
extern int o_gun3[8];
extern int playerpic;
extern int player_cx, player_cy;
extern int startfadeflag;
extern int startendwave;
extern int gl_cnt;
extern int g_oldsuper, g_oldshield;
extern int playerx, playery;
extern char gdmodestr[];
extern int playerbasepic;
extern char* LASTSCR;

struct player_t {
    char name[20];
    char callsign[12];
    int pilotPicId;
    unsigned int money;
    int currentWeapon;
    int currentGame;
    int waveGameLevel[3];
    int amountOfItems;
    int waveProgression[4];
    int f_50;
    int f_54;
};

struct mobj_t {
    int x;
    int y;
    int dirX;
    int dirY;
    int max_x;
    int max_y;
    int dir_x;
    int dir_y;
    int triggerDelay;
    int f_24;
    int trigger;
};

struct flat_t {
    int f_0; // eitems
    short f_4; // hits
    short f_6; // money
};

extern flat_t *flatlib[4];
extern player_t player;

int wrand(void);

void InitMobj(mobj_t *m);
void MoveMobj(mobj_t* m);
int MoveSobj(mobj_t* m, int a2);
void RAP_FreeMap(void);
void RAP_LoadMap(void);
int Do_Game(void);
int RAP_LoadWin(void);
void ShutDown(int a1);
