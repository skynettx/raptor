#pragma once
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


struct player_t {
    char f_0[20];
    char f_14[12];
    int f_20;
    int f_24;
    int f_28;
    int f_2c;
    int f_30[3];
    int f_3c;
    int f_40[4];
    int f_50;
    int f_54;
};

struct mobj_t {
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
};

struct flat_t {
    int f_0; // eitems
    short f_4; // hits
    short f_6; // money
};

extern flat_t *flatlib[4];
extern player_t plr;

int wrand(void);

void InitMobj(mobj_t *m);
void MoveMobj(mobj_t* m);
int MoveSobj(mobj_t* m, int a2);
void RAP_FreeMap(void);
void RAP_LoadMap(void);
int Do_Game(void);
int RAP_LoadWin(void);
