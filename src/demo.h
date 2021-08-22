#pragma once

#pragma pack(push, 1)

struct demo_t
{
    char f_0;
    char f_1;
    char f_2;
    char f_3;
    short f_4;
    short f_6;
    short f_8;
    short f_a;
};
#pragma pack(pop)


void DEMO_MakePlayer(int a1);
void DEMO_GLBFile(int a1);
int DEMO_Play(void);
void DEMO_StartRec(void);
int DEMO_Think(void);
void DEMO_SaveFile(void);
void DEMO_SetFileName(const char *a1);
void DEMO_LoadFile(void);
void DEMO_DisplayStats(void);
