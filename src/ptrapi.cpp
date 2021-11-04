#include "SDL.h"
#include "common.h"
#include "i_video.h"
#include "ptrapi.h"
#include "gfxapi.h"

int g_drawcursor;
static int mousepresent;
static int joyactive;
static int joy_mx, joy_my;
static int ptrupdate = 1;
static int ptr_pause;
static int joypresent;
static void (*ptrcursorhook)(void);
static void (*ptrboundshook)(void);
static int cursoroffsetx, cursoroffsety;
static int ptrerase;
static int ptrclip;
static int cursorxnew, cursorynew;
int ptractive;
int cursorloopx, cursorloopy;
int cursorx, cursory;
char *cursorsave, *cursorpic;
char *displaypic;
char *cursorstart;
int mouseb1, mouseb2, mouseb3;
int mouse_b1_ack, mouse_b2_ack, mouse_b3_ack;

int cur_mx, cur_my;

void PTR_MouseHandler(void)
{
    static int old_x = -1;
    static int old_y;
    I_GetMousePos(&cur_mx, &cur_my);
    if (cur_mx < 0)
        cur_mx = 0;
    else if (cur_mx >= SCREENWIDTH)
        cur_mx = SCREENWIDTH - 1;
    if (cur_my < 0)
        cur_my = 0;
    else if (cur_my >= SCREENHEIGHT)
        cur_my = SCREENHEIGHT - 1;

    if (old_x != cur_mx || old_y != cur_my)
    {
        old_x = cur_mx;
        old_y = cur_my;
        ptrupdate = 1;
    }
}

void I_HandleMouseEvent(SDL_Event *sdlevent)
{
    switch (sdlevent->type)
    {
    case SDL_MOUSEBUTTONDOWN:
        switch (sdlevent->button.button)
        {
        case SDL_BUTTON_LEFT:
            mouseb1 = 1;
            mouse_b1_ack = 1;
            break;
        case SDL_BUTTON_RIGHT:
            mouseb2 = 2;
            mouse_b2_ack = 1;
            break;
        case SDL_BUTTON_MIDDLE:
            mouseb3 = 4;
            mouse_b3_ack = 1;
            break;
        }
        break;
    case SDL_MOUSEBUTTONUP:
        switch (sdlevent->button.button)
        {
        case SDL_BUTTON_LEFT:
            mouseb1 = 0;
            break;
        case SDL_BUTTON_RIGHT:
            mouseb2 = 0;
            break;
        case SDL_BUTTON_MIDDLE:
            mouseb3 = 0;
            break;
        }
        break;
    }
}

void PTR_ResetJoyStick(void)
{
    if (joyactive)
    {
        joy_mx = 0;
        joy_my = 0;
    }
}

void PTR_ClipCursor(void)
{
    displaypic = cursorpic;
    ptrclip = 0;
    if (cursorxnew + 16 > 320)
    {
        cursorloopx = 320 - cursorxnew;
        ptrclip = 1;
    }
    else if (cursorxnew < 0)
    {
        cursorloopx = cursorxnew + 16;
        displaypic -= cursorxnew;
        cursorxnew = 0;
        ptrclip = 1;
    }
    else
        cursorloopx = 16;
    if (cursorynew + 16 > 200)
    {
        cursorloopy = 200 - cursorynew;
        ptrclip = 1;
    }
    else if (cursorynew < 0)
    {
        cursorloopy = cursorynew + 16;
        displaypic += (-cursorynew) * 16;
        cursorynew = 0;
        ptrclip = 1;
    }
    else
        cursorloopy = 16;
}

void PTR_UpdateCursor(void)
{
    static int lasttick;
    int now = SDL_GetTicks();
    if (now - lasttick < 1000 / 15)
        return;
    lasttick += 1000 / 15;
    if (ptr_pause)
        return;
    // if (joyactive)
    //     PTR_JoyHandler();
    if (ptrupdate)
    {
        ptrupdate = 0;
        if (ptrerase)
        {
            if (ptrclip)
                PTR_ClipErase();
            else
                PTR_Erase();
            ptrerase = 0;
        }
        if (ptrboundshook)
            ptrboundshook();
        cursorxnew = cur_mx - cursoroffsetx;
        cursorynew = cur_my - cursoroffsety;
        if (g_drawcursor)
        {
            PTR_ClipCursor();
            cursorstart = displayscreen + ylookup[cursorynew] + cursorxnew;
            if (cursorloopy < 16)PTR_ClipSave();
            else PTR_Save();
            //PTR_Save();
            PTR_Draw();
            ptrerase = 1;
        }
        if (ptrcursorhook)
            ptrcursorhook();
        cursorx = cursorxnew;
        cursory = cursorynew;
        I_FinishUpdate();
    }
}

void PTR_FrameHook(void (*a1)(void))
{
    int vc, va, vd, vb;
    if (!g_drawcursor)
    {
        a1();
        return;
    }
    // if (joyactive)
    //     PTR_JoyHandler();
    if (ptrboundshook)
        ptrboundshook();
    cursorxnew = cur_mx - cursoroffsetx;
    cursorynew = cur_my - cursoroffsety;
    vc = ud_x + ud_lx;
    va = ud_y + ud_ly;
    vd = ud_x - 16;
    vb = ud_y - 16;
    if (vd <= cursorxnew && vc >= cursorxnew && vb <= cursorynew && va >= cursorynew)
    {
        if (ptrerase)
        {
            if (vd <= cursorx && vc >= cursorx && vb <= cursory && va >= cursory)
            {
                GFX_MarkUpdate(cursorx, cursory, cursorloopx, cursorloopy);
            }
            else
            {
                if (ptrclip)
                    PTR_ClipErase();
                else
                    PTR_Erase();
                ptrerase = 0;
            }
        }
        PTR_ClipCursor();
        GFX_MarkUpdate(cursorxnew, cursorynew, cursorloopx, cursorloopy);
        cursorstart = displaybuffer + ylookup[cursorynew] + cursorxnew;
        if (cursorloopy < 16)
            PTR_ClipSave();
        else
            PTR_Save();
        PTR_Draw();
        a1();
        if (ptrclip)
            PTR_ClipErase();
        else
            PTR_Erase();
        cursorstart = displayscreen + ylookup[cursorynew] + cursorxnew;
        ptrerase = 1;
    }
    else
    {
        if (ptrupdate)
        {
            if (ptrerase)
            {
                if (ptrclip)
                    PTR_ClipErase();
                else
                    PTR_Erase();
                ptrerase = 0;
            }
            PTR_ClipCursor();
            cursorstart = displayscreen + ylookup[cursorynew] + cursorxnew;
            PTR_Save();
            PTR_Draw();
            ptrerase = 1;
        }
        a1();
    }
    if (ptrcursorhook)
        ptrcursorhook();
    cursorx = cursorxnew;
    cursory = cursorynew;
}

void PTR_DrawCursor(int draw)
{
    if (ptractive)
    {
        if (!draw && ptrerase == 1)
        {
            if (ptrclip)
                PTR_ClipErase();
            else
                PTR_Erase();
            ptrerase = 0;
        }
        if (draw == 1)
            ptrupdate = 1;
        g_drawcursor = draw;
    }
    else
        g_drawcursor = 0;
}

void PTR_SetPic(texture_t *a1)
{
    int i;
    char *dat, *vs;
    cursoroffsetx = 0;
    cursoroffsety = 0;
    if (ptractive)
    {
        dat = a1->f_14;
        vs = cursorpic;
        for (i = 0; i < 256; i++)
        {
            cursorpic[i] = a1->f_14[i];
            if ((uint8_t)a1->f_14[i] == 255)
            {
                cursoroffsetx = i % 16;
                cursoroffsety = i / 16;
                cursorpic[i] = a1->f_14[i + 1];
            }
        }
        if (cursoroffsetx > 16)
            cursoroffsetx = 0;
        if (cursoroffsety > 16)
            cursoroffsety = 0;
        ptrupdate = 1;
    }
}

void PTR_SetBoundsHook(void (*a1)(void))
{
    ptrboundshook = a1;
    ptrupdate = 1;
}

void PTR_SetCursorHook(void (*a1)(void))
{
    ptrcursorhook = a1;
    ptrupdate = 1;
}

void PTR_SetUpdateFlag(void)
{
    ptrupdate = 1;
}

void PTR_SetPos(int x, int y)
{
    if (mousepresent)
        I_SetMousePos(x, y);
    cur_mx = x;
    cur_my = y;
    if (ptractive)
        ptrupdate = 1;
}

void PTR_Pause(int pause)
{
    if (ptractive && pause != ptr_pause)
    {
        if (pause)
        {
            PTR_DrawCursor(0);
            // TSM_PauseService(ptr_tsm);
        }
        else
        {
            g_drawcursor = 0;
            // TSM_ResumeService(ptr_tsm);
            PTR_SetPos(160, 100);
            PTR_DrawCursor(0);
        }
        ptr_pause = pause;
    }
}

int PTR_Init(int control)
{
    g_drawcursor = 0;
    cursorsave = (char*)malloc(256);
    if (!cursorsave)
        EXIT_Error("PTR_Init() - malloc");
    cursorpic = (char*)malloc(256);
    if (!cursorpic)
        EXIT_Error("PTR_Init() - malloc");
    joyactive = 0;
    mousepresent = 0;
    joypresent = 0;
    if (control == 2)
        joypresent = 1;
    if (control == 0 || control == 1 || control == 2)        // || Control == 2 Added while Hangar menu broken without 
        mousepresent = 1;
    if (control == 2 && joypresent)
        joyactive = 1;
    if (mousepresent || joyactive)
    {
        ptractive = 1;
        // ptr_tsm = TSM_NewService(PTR_UpdateCursor, 15, 254, 0);
        GFX_SetFrameHook(PTR_FrameHook);
    }
    // else
    //     ptr_tsm = -1;

    // if (joypresent)
    //     PTR_CalJoy();
    PTR_SetPos(100, 160);
    if (mousepresent || joyactive)
        return 1;
    return 0;
}

void PTR_End(void)
{
    // if (ptr_tsm != -1)
    //     TSM_DelService(ptr_tsm);
}
