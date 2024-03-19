#pragma once
#include <boost/endian.hpp>
#include "rap.h"

using namespace boost::endian;

typedef enum
{
    E_FORWARD,
    E_BACKWARD
}EDIR;

typedef enum
{
    KAMI_FLY,
    KAMI_CHASE,
    KAMI_END
}KAMI;

typedef enum
{
    GANIM_DONE,
    GANIM_OFF,
    GANIM_ON
}GANIM;

typedef enum
{
    MULTI_OFF,
    MULTI_START,
    MULTI_END
}MULTI;

typedef struct
{
    char iname[16];                                    // ITEM NAME
    little_int32_t item;                               // * GLB ITEM #
    little_int32_t bonus;                              // BONUS # ( -1 == NONE )
    little_int32_t exptype;                            // EXPLOSION TYPE
    little_int32_t shootspace;                         // SLOWDOWN SPEED
    little_int32_t ground;                     //NOT USED IS ON GROUND
    little_int32_t suck;                               // CAN SUCK WEAPON AFFECT
    little_int32_t frame_rate;                         // FRAME RATE
    little_int32_t num_frames;                         // NUM FRAMES
    little_int32_t countdown;                          // COUNT DOWN TO START ANIM
    little_int32_t rewind;                             // FRAMES TO REWIND
    little_int32_t animtype;                           // FREE SPACE FOR LATER USE
    little_int32_t shadow;                             // USE SHADOW ( TRUE/FALSE )
    little_int32_t bossflag;                           // SHOULD THIS BE CONSIDERED A BOSS
    little_int32_t hits;                               // HIT POINTS
    little_int32_t money;                              // $$ AMOUNT WHEN KILLED
    little_int32_t shootstart;                         // SHOOT START OFFSET
    little_int32_t shootcnt;                           // HOW MANY TO SHOOT
    little_int32_t shootframe;                         // FRAME RATE TO SHOOT
    little_int32_t movespeed;                          // MOVEMENT SPEED
    little_int32_t numflight;                          // NUMBER OF FLIGHT POSITIONS
    little_int32_t repos;                              // REPEAT TO POSITION
    little_int32_t flighttype;                         // FLIGHT TYPE
    little_int32_t numguns;                            // NUMBER OF GUNS
    little_int32_t numengs;                            // NUMBER OF ENGINES
    little_int32_t sfx;                        //NOT USED SFX # TO PLAY
    little_int32_t song;                               // SONG # TO PLAY
    little_int16_t shoot_type[MAX_GUNS];               // ENEMY SHOOT TYPE
    little_int16_t engx[MAX_GUNS];                     // X POS ENGINE FLAME
    little_int16_t engy[MAX_GUNS];                     // Y POS ENGINE FLAME
    little_int16_t englx[MAX_GUNS];                    // WIDTH OF ENGINE FLAME
    little_int16_t shootx[MAX_GUNS];                   // X POS SHOOT FROM
    little_int16_t shooty[MAX_GUNS];                   // Y POS SHOOT FROM
    little_int16_t flightx[MAX_FLIGHT];                // FLIGHT X POS
    little_int16_t flighty[MAX_FLIGHT];                // FLIGHT Y POS
}SPRITE;

typedef struct SPRITE_SHIP_S
{
    struct SPRITE_SHIP_S *prev;
    struct SPRITE_SHIP_S *next;
    int item;                         // GLB item of current frame
    SPRITE *lib;                      // SPRITE LIB POS
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
    MOVEOBJ move;
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
}SPRITE_SHIP;

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

extern SPRITE_SHIP first_enemy, last_enemy;
void ENEMY_Clear(void);
void ENEMY_LoadLib(void);
void ENEMY_LoadSprites(void);
void ENEMY_FreeSprites(void);
SPRITE_SHIP *ENEMY_GetRandom(void);
SPRITE_SHIP *ENEMY_GetRandomAir(void);
SPRITE_SHIP *ENEMY_DamageEnergy(int x, int y, int damage);
int ENEMY_DamageAll(int x, int y, int damage);
int ENEMY_DamageGround(int x, int y, int damage);
int ENEMY_DamageAir(int x, int y, int damage);
int ENEMY_GetBaseDamage(void);
void ENEMY_Think(void);
void ENEMY_DisplayGround(void);
void ENEMY_DisplaySky(void);
