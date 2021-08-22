#include <string.h>
#include "common.h"
#include "ptrapi.h"

extern char *cursorsave;
extern char *displaypic;
extern char *cursorstart;
extern int cursorloopx, cursorloopy;

void PTR_Save(void)
{
    int i;
    for (i = 0; i < 16; i++)
    {
        memcpy(&cursorsave[i * 16], &cursorstart[i * 320], 16);
    }
}

void PTR_ClipSave(void)
{
    int i;
    for (i = 0; i < cursorloopy; i++)
    {
        memcpy(&cursorsave[i * 16], &cursorstart[i * 320], 16);
    }
}

void PTR_Erase(void)
{
    int i;
    for (i = 0; i < 16; i++)
    {
        memcpy(&cursorstart[i * 320], &cursorsave[i * 16], 16);
    }
}

void PTR_ClipErase(void)
{
    int i;
    for (i = 0; i < cursorloopy; i++)
    {
        memcpy(&cursorstart[i * 320], &cursorsave[i * 16], cursorloopx);
    }
}

void PTR_Draw(void)
{
    int i, j;
    for (i = 0; i < cursorloopy; i++)
    {
        for (j = cursorloopx - 1; j >= 0; j--)
        {
            if (displaypic[i * 16 + j])
                cursorstart[i * 320 + j] = displaypic[i * 16 + j];
        }
    }
}
