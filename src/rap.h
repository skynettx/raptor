#pragma once

#define MAX_SHIELD 100
#define MAX_SUPER  100

#define MAX_FLIGHT      30

#define MAP_ROWS        150
#define MAP_ONSCREEN    8
#define MAP_COLS        9
#define MAP_BLOCKSIZE   32
#define MAP_BOTTOM      200 - 18
#define MAP_TOP         2
#define MAP_LEFT        16
#define MAP_RIGHT       ( 320 - 16 )
#define MAP_SIZE        ( MAP_ROWS * MAP_COLS )

#define MAX_TILES       2048
#define MAX_GUNS        24

#define ENGINE_COLOR    254
#define SHOT_COLOR      255

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
    char name[20];
    char callsign[12];
    int id_pic;
    unsigned int score;                   // MONEY/POINTS
    int sweapon;                          // CURENT SPECIAL WEAPON
    int cur_game;                         // CURRENT GAME
    int game_wave[3];                     // CURRENT WAVE
    int numobjs;                          // NUMBER OF OBJECTS TO FOLLOW
    int diff[4];                          // DIFFICULTY LEVEL
    int trainflag;
    int fintrain;
};

#define END_DURATION ( 20 * 3 )
#define END_EXPLODE  ( 24 )
#define END_FLYOFF   ( 20 * 2 )

#define  DIFF_0   0  // TRAINING MODE
#define  DIFF_1   1  // EASY MODE
#define  DIFF_2   2  // NORMAL MODE
#define  DIFF_3   3  // HARD MODE

#define GAME1 ( gameflag[0] )
#define GAME2 ( gameflag[1] )
#define GAME3 ( gameflag[2] + gameflag[3] )

#define SHIELD_COLOR_RUN 9
#define SHIELD_LOW       10

#define PLAYERWIDTH  32
#define PLAYERHEIGHT 32
#define PLAYERMINX   5
#define PLAYERMAXX   314
#define PLAYERINITX  160-(PLAYERWIDTH/2)
#define PLAYERINITY  160

#define MINPLAYERY   0
#define MAXPLAYERY   160

#define GREEN        100
#define RED          34
#define YELLOW       50

#define MAP_ABORT    -1
#define MAP_NEXT     -2

#define MAX_ONSCREEN 30

struct mobj_t {
    int x;
    int y;
    int x2;
    int y2;
    int delx;
    int dely;
    int addx;
    int addy;
    int maxloop;
    int err;
    int done;
};

enum FLIGHT_TYPE
{
    F_REPEAT,
    F_LINEAR,
    F_KAMI,
    F_GROUND,
    F_GROUNDLEFT,
    F_GROUNDRIGHT,
};

enum ANIMTYPE
{
    GANIM_NORM,
    GANIM_SHOOT,
    GANIM_MULTI
};

enum EXP_TYPE
{
    EXP_AIRSMALL1, // 0  
    EXP_AIRMED,    // 1 NORMAL AIR    ( norm )
    EXP_AIRLARGE,  // 2 bigger that 32 by 32
    EXP_GRDSMALL,  // 3 smaller than 32 by 32          
    EXP_GRDMED,    // 4 NORMAL GROUND ( norm )
    EXP_GRDLARGE,  // 5 bigger than 32 by 32
    EXP_BOSS,      // 6 BIG BOSS SHIP ( AIR ) 
    EXP_PERSON,    // 7 ONE LITTLE PERSON BLWING UP
    EXP_ENERGY,    // 8 ENERGY SHIP BLOWING UP
    EXP_PLATOON,   // 9 ONE LITTLE PERSON BLWING UP
    EXP_AIRSMALL2  // 10  
};

enum KEYOPTS
{
    K_OK,
    K_NEXTFRAME,
    K_SKIPALL,
    K_EXITDOS
};

struct flat_t {
    int linkflat; 
    short bonus; 
    short bounty;
};

extern flat_t *flatlib[4];
extern player_t plr;

int wrand(void);

void InitMobj(mobj_t *cur);
void MoveMobj(mobj_t* cur);
int MoveSobj(mobj_t* cur, int speed);
void RAP_FreeMap(void);
void RAP_LoadMap(void);
int Do_Game(void);
int RAP_LoadWin(void);
void ShutDown(int errcode);
