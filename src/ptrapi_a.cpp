#include <string.h>
#include "common.h"
#include "ptrapi.h"

#define CURSORHEIGHT  16
#define CURSORWIDTH   16

extern char *cursorsave;
extern char *displaypic;
extern char *cursorstart;
extern int cursorloopx, cursorloopy;

/*------------------------------------------------------------------------
PTR_Save() - Saves screen before a cursor draw
  ------------------------------------------------------------------------*/
void 
PTR_Save(
    void
)
{
    int loop;
    
    for (loop = 0; loop < CURSORHEIGHT; loop++)
    {
        memcpy(&cursorsave[loop * CURSORWIDTH], &cursorstart[loop * SCREENWIDTH], CURSORWIDTH);
    }
}

/*------------------------------------------------------------------------
PTR_ClipSave() - Saves screen before a cursor draw
  ------------------------------------------------------------------------*/
void 
PTR_ClipSave(
    void
)
{
    int loop;
    
    for (loop = 0; loop < cursorloopy; loop++)
    {
        memcpy(&cursorsave[loop * CURSORWIDTH], &cursorstart[loop * SCREENWIDTH], CURSORWIDTH);
    }
}

/*------------------------------------------------------------------------
PTR_Erase() - Erases cursor with stuff from PTR_Save()
  ------------------------------------------------------------------------*/
void 
PTR_Erase(
    void
)
{
    int loop;
    
    for (loop = 0; loop < CURSORHEIGHT; loop++)
    {
        memcpy(&cursorstart[loop * SCREENWIDTH], &cursorsave[loop * CURSORWIDTH], CURSORWIDTH);
    }
}

/*------------------------------------------------------------------------
PTR_ClipErase() - Erases cursor and clips edges of screen
  ------------------------------------------------------------------------*/
void 
PTR_ClipErase(
    void
)
{
    int loop;
    
    for (loop = 0; loop < cursorloopy; loop++)
    {
        memcpy(&cursorstart[loop * SCREENWIDTH], &cursorsave[loop * CURSORWIDTH], cursorloopx);
    }
}

/*------------------------------------------------------------------------
PTR_Draw() - Draws Cursor
  ------------------------------------------------------------------------*/
void 
PTR_Draw(
    void
)
{
    int loop, i;
    
    for (loop = 0; loop < cursorloopy; loop++)
    {
        for (i = cursorloopx - 1; i >= 0; i--)
        {
            if (displaypic[loop * CURSORWIDTH + i])
                cursorstart[loop * SCREENWIDTH + i] = displaypic[loop * CURSORWIDTH + i];
        }
    }
}
