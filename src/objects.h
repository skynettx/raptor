#pragma once


struct objlib_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    int (*f_14)(int);
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    int f_28;
    int f_2c;
    int f_30;
    int f_34;
};

struct object_t {
    object_t* f_0;
    object_t* f_4;
    int f_8;
    int f_c;
    objlib_t *f_10;
    int f_14;
};

extern object_t first_objs, last_objs;

void OBJS_Init(void);
int OBJS_Add(int a1);
void OBJS_GetNext(void);
int OBJS_GetAmt(int a1);
objlib_t *OBJS_GetLib(int a1);
int OBJS_CanBuy(int a1);
int OBJS_GetCost(int a1);
int OBJS_CanSell(int a1);
int OBJS_GetNum(void);
int OBJS_IsOnly(int a1);
int OBJS_GetTotal(int a1);
int OBJS_GetResale(int a1);
int OBJS_Buy(int a1);
int OBJS_Sell(int a1);
void OBJS_Clear(void);
int OBJS_Load(object_t *a1);
int OBJS_IsEquip(int a1);
int OBJS_LoseObj(void);
void OBJS_DisplayStats(void);
int OBJS_MakeSpecial(int a1);
void OBJS_Use(int a1);
void OBJS_Think(void);
int OBJS_SubEnergy(int a1);
int OBJS_AddEnergy(int a1);
void OBJS_CachePics(void);
void OBJS_FreePics(void);
