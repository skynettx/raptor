#pragma once

#define MAX_OBJS 20

enum BUYSTUFF
{
    OBJ_GOTIT,
    OBJ_NOMONEY,
    OBJ_SHIPFULL,
    OBJ_ERROR
};

enum OBJ_TYPE
{
    S_FORWARD_GUNS,   // 00 * DUAL MACHINE GUNS
    S_PLASMA_GUNS,    // 01 * PLASMA GUNS
    S_MICRO_MISSLE,   // 02 * SMALL WING MISSLES
    S_DUMB_MISSLE,    // 03 DUMB FIRE MISSLE         // 1
    S_MINI_GUN,       // 04 AUTO TRACK MINI GUN      // 2
    S_TURRET,         // 05 AUTO TRACK LASER TURRET  // 3
    S_MISSLE_PODS,    // 06 MULTIPLE MISSLE SHOTS    // 4
    S_AIR_MISSLE,     // 07 AIR TO AIR MISSLE        // 5
    S_GRD_MISSLE,     // 08 AIR TO GROUND            // 6
    S_BOMB,           // 09 GROUND BOMB              // 7
    S_ENERGY_GRAB,    // 10 ENERGY GRAB              // 8
    S_MEGA_BOMB,      // 11 MEGA BOMB KILLS ALL AIR  //  
    S_PULSE_CANNON,   // 12 WAVE WEAPON              // 9
    S_FORWARD_LASER,  // 13 ALTERNATING LASER        // MINUS
    S_DEATH_RAY,      // 14 DEATH RAY                // 0
    S_SUPER_SHIELD,   // 15 SUPER SHIELD             //
    S_ENERGY,         // 16 NORMAL SHIELD ENERGY     //
    S_DETECT,         // 18 DAMAGE DETECTOR          //
    S_ITEMBUY1,       // 19 
    S_ITEMBUY2,       // 20
    S_ITEMBUY3,       // 21
    S_ITEMBUY4,       // 22
    S_ITEMBUY5,       // 23
    S_ITEMBUY6,       // 24
    S_LAST_OBJECT     // 25 LAST OBJECT ( NOT USED )
};

#define LAST_WEAPON ( S_DEATH_RAY )
#define FIRST_SPECIAL ( S_DUMB_MISSLE )

struct objlib_t {
    int item;                             // LUMPNUM OF PIC
    int numframes;                        // NUMBER OF FRAMES
    int cost;                             // COST OF OBJECT
    int start_cnt;                        // START AMOUNT OF ITEMS
    int max_cnt;                          // MAX COUNT OF UNITS/ITEMS
    int (*actf)(int);                     // FUNCTION PTR FOR ACTION
    int fxtype;                           // SOUND FX TYPE
    int forever;                          // LAST FOREVER
    int onlyflag;                         // UNIQUE FLAG ( ENERGY )
    int loseit;                           // LOSE OBJECT
    int specialw;                         // SPECIAL WEAPON
    int moneyflag;                        // ADDS MONEY TO PLAYER WHEN PICKED UP
    int game1flag;                        // TRUE=AVALABLE IN ALL GAMES
    int shieldflag;                       // SHOW SHIELD WHEN BONUS
};

struct object_t {
    object_t* prev;                       // LINK LIST PREV
    object_t* next;                       // LINK LIST NEXT
    int num;                              // NUMBER OF ITEM(S)
    int type;                             // OBJECT TYPE
    objlib_t *lib;                        // POINTER TO LIB
    int inuse;                            // CURRENTLY EQUIPED
};

extern object_t first_objs, last_objs;

void OBJS_Init(void);
int OBJS_Add(int type);
void OBJS_GetNext(void);
int OBJS_GetAmt(int type);
objlib_t *OBJS_GetLib(int type);
int OBJS_CanBuy(int type);
int OBJS_GetCost(int type);
int OBJS_CanSell(int type);
int OBJS_GetNum(void);
int OBJS_IsOnly(int type);
int OBJS_GetTotal(int type);
int OBJS_GetResale(int type);
int OBJS_Buy(unsigned int type);
int OBJS_Sell(int type);
void OBJS_Clear(void);
int OBJS_Load(object_t *inobj);
int OBJS_IsEquip(int type);
int OBJS_LoseObj(void);
void OBJS_DisplayStats(void);
int OBJS_MakeSpecial(int type);
void OBJS_Use(int type);
void OBJS_Think(void);
int OBJS_SubEnergy(int amt);
int OBJS_AddEnergy(int amt);
void OBJS_CachePics(void);
void OBJS_FreePics(void);
