#pragma once
#include "rap.h"

enum EDIR
{
    E_FORWARD,
    E_BACKWARD
};

enum KAMI
{
    KAMI_FLY,
    KAMI_CHASE,
    KAMI_END
};

enum GANIM
{
    GANIM_DONE,
    GANIM_OFF,
    GANIM_ON
};

enum MULTI
{
    MULTI_OFF,
    MULTI_START,
    MULTI_END
};

struct slib_t {
    char iname[16];                         // ITEM NAME
    int item;                               // * GLB ITEM #
    int f_14;
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    int frame_rate;                         // FRAME RATE
    int num_frames;                         // NUM FRAMES
    int countdown;                          // COUNT DOWN TO START ANIM
    int rewind;                             // FRAMES TO REWIND
    int animtype;                           // FREE SPACE FOR LATER USE
    int f_3c;
    int bossflag;                           // SHOULD THIS BE CONSIDERED A BOSS
    int hits;                               // HIT POINTS
    int f_48;
    int shootstart;                         // SHOOT START OFFSET
    int shootcnt;                           // HOW MANY TO SHOOT
    int f_54;
    int movespeed;                          // MOVEMENT SPEED
    int f_5c;
    int f_60;
    int flighttype;                         // FLIGHT TYPE
    int f_68;
    int f_6c;
    int f_70;
    int song;                               // SONG # TO PLAY
    short f_78[24];
    short f_a8[24];
    short f_d8[24];
    short f_108[24];
    short f_138[24];
    short f_168[24];
    short flightx[MAX_FLIGHT];             // FLIGHT X POS
    short flighty[MAX_FLIGHT];             // FLIGHT Y POS
}; // size 0x210

struct enemy_t {
    enemy_t *prev;
    enemy_t *next;
    int item;                         // GLB item of current frame
    slib_t *lib;                      // SPRITE LIB POS
    int sx;                           // START X
    int sy;                           // START Y
    int x;                            // CENTER X POS;
    int y;                            // Y POS
    int x2;                           // WIDTH
    int y2;                           // HEIGHT           
    int width;                        // WIDTH
    int height;                       // HEIGHT
    int hlx;                          // WIDTH/2
    int hly;                          // HEIGHT/2
    int movepos;                      // CUR POSITION IN FLIGHT (X/Y)
    int shootagain;                   // * COUNT DOWN TO SHOOT
    int shootcount;                   // * COUNT TO SHOOT
    int shootflag;                    // * COUNTER TO SPACE SHOTS
    int hits;                         // *
    int groundflag;                   // *
    int f_58;
    mobj_t mobj;
    int countdown;
    int f_8c;
    int f_90;
    int num_frames;
    int anim_on;
    int edir;
    int kami;
    int frame_rate;
    int shoot_on;
    int shoot_disable;
    int f_b0;
    int speed;
    int suckagain;
};

#define E_NUM_DIFF    4

#define E_SECRET_1    0
#define E_SECRET_2    1
#define E_SECRET_3    2
#define E_EASY_LEVEL  3
#define E_MED_LEVEL   4
#define E_HARD_LEVEL  5
#define E_TRAIN_LEVEL 6

#define EB_SECRET_1    1
#define EB_SECRET_2    2
#define EB_SECRET_3    4
#define EB_EASY_LEVEL  8
#define EB_MED_LEVEL   16
#define EB_HARD_LEVEL  32
#define EB_NOT_USED    64

extern enemy_t first_enemy, last_enemy;
void ENEMY_Clear(void);
void ENEMY_LoadLib(void);
void ENEMY_LoadSprites(void);
void ENEMY_FreeSprites(void);
enemy_t *ENEMY_GetRandom(void);
enemy_t* ENEMY_GetRandomAir(void);
enemy_t *ENEMY_DamageEnergy(int a1, int a2, int a3);
int ENEMY_DamageAll(int a1, int a2, int a3);
int ENEMY_DamageGround(int a1, int a2, int a3);
int ENEMY_DamageAir(int a1, int a2, int a3);
int ENEMY_GetBaseDamage(void);
void ENEMY_Think(void);
void ENEMY_DisplayGround(void);
void ENEMY_DisplaySky(void);
