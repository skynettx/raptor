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
    int bonus;                              // BONUS # ( -1 == NONE )
    int exptype;                            // EXPLOSION TYPE 
    int shootspace;                         // SLOWDOWN SPEED
    int ground;                     //NOT USED IS ON GROUND
    int suck;                               // CAN SUCK WEAPON AFFECT
    int frame_rate;                         // FRAME RATE
    int num_frames;                         // NUM FRAMES
    int countdown;                          // COUNT DOWN TO START ANIM
    int rewind;                             // FRAMES TO REWIND
    int animtype;                           // FREE SPACE FOR LATER USE
    int shadow;                             // USE SHADOW ( TRUE/FALSE )
    int bossflag;                           // SHOULD THIS BE CONSIDERED A BOSS
    int hits;                               // HIT POINTS
    int money;                              // $$ AMOUNT WHEN KILLED
    int shootstart;                         // SHOOT START OFFSET
    int shootcnt;                           // HOW MANY TO SHOOT
    int shootframe;                         // FRAME RATE TO SHOOT
    int movespeed;                          // MOVEMENT SPEED
    int numflight;                          // NUMBER OF FLIGHT POSITIONS
    int repos;                              // REPEAT TO POSITION
    int flighttype;                         // FLIGHT TYPE
    int numguns;                            // NUMBER OF GUNS
    int numengs;                            // NUMBER OF ENGINES
    int sfx;                        //NOT USED SFX # TO PLAY
    int song;                               // SONG # TO PLAY
    short shoot_type[MAX_GUNS];             // ENEMY SHOOT TYPE
    short engx[MAX_GUNS];                   // X POS ENGINE FLAME
    short engy[MAX_GUNS];                   // Y POS ENGINE FLAME
    short englx[MAX_GUNS];                  // WIDTH OF ENGINE FLAME
    short shootx[MAX_GUNS];                 // X POS SHOOT FROM
    short shooty[MAX_GUNS];                 // Y POS SHOOT FROM
    short flightx[MAX_FLIGHT];              // FLIGHT X POS
    short flighty[MAX_FLIGHT];              // FLIGHT Y POS
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
    int doneflag;
    mobj_t mobj;
    int countdown;
    int curframe;
    int eframe;
    int num_frames;
    int anim_on;
    int edir;
    int kami;
    int frame_rate;
    int shoot_on;
    int shoot_disable;
    int multi;
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
enemy_t *ENEMY_GetRandomAir(void);
enemy_t *ENEMY_DamageEnergy(int x, int y, int damage);
int ENEMY_DamageAll(int x, int y, int damage);
int ENEMY_DamageGround(int x, int y, int damage);
int ENEMY_DamageAir(int x, int y, int damage);
int ENEMY_GetBaseDamage(void);
void ENEMY_Think(void);
void ENEMY_DisplayGround(void);
void ENEMY_DisplaySky(void);
