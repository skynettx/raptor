#include "SDL.h"
#include "common.h"
#include "i_video.h"
#include "ptrapi.h"
#include "gfxapi.h"
#include "joyapi.h"
#include "input.h"

#define CURSORHEIGHT  16
#define CURSORWIDTH   16
#define CURSORSIZE    (CURSORHEIGHT*CURSORWIDTH)
#define HOTSPOTCOLOR  255

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
int old_joy_x, old_joy_y;

/*------------------------------------------------------------------------
   PTR_JoyHandler() - Joystick Handler Function
  ------------------------------------------------------------------------*/
void 
PTR_JoyHandler(
    void
)
{
    static int lasttick;
    int now = SDL_GetTicks();
    
    if (now - lasttick < 1000 / 60)
        return;
    
    lasttick += 1000 / 60;

    cur_mx = StickX + old_joy_x;
    cur_my = StickY + old_joy_y;
    
    if (cur_mx < 0)
        cur_mx = 0;
    else if (cur_mx >= SCREENWIDTH)
         cur_mx = SCREENWIDTH - 1;
    
    if (cur_my < 0)
        cur_my = 0;
    else if (cur_my >= SCREENHEIGHT)
        cur_my = SCREENHEIGHT - 1;

    if (old_joy_x != cur_mx || old_joy_y != cur_my)
    {
        old_joy_x = cur_mx;
        old_joy_y = cur_my;
        ptrupdate = 1;
    }
}

/*------------------------------------------------------------------------
   PTR_MouseHandler() - Mouse Handler Function
  ------------------------------------------------------------------------*/
void 
PTR_MouseHandler(
    void
)
{
    static int old_x = -1;
    static int old_y;
    
    I_GetMousePos(&cur_mx, &cur_my);
    
    if (cur_mx < 0)
        cur_mx = 0;
    else if (cur_mx >= SCREENWIDTH)
    {
        cur_mx = SCREENWIDTH - 1;
        if (!g_drawcursor || fullscreen)
            I_SetMousePos(cur_mx, cur_my);
    }
    
    if (cur_my < 0)
        cur_my = 0;
    else if (cur_my >= SCREENHEIGHT)
    {
        cur_my = SCREENHEIGHT - 1;
        if (!g_drawcursor || fullscreen)
            I_SetMousePos(cur_mx, cur_my);
    }

    if (old_x != cur_mx || old_y != cur_my)
    {
        old_x = cur_mx;
        old_y = cur_my;
        ptrupdate = 1;
    }
}

/*------------------------------------------------------------------------
   I_HandleMouseEvent() - Get current mouse status
  ------------------------------------------------------------------------*/
void 
I_HandleMouseEvent(
    SDL_Event *sdlevent
)
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

/*------------------------------------------------------------------------
   PTR_ResetJoystick() - Reset joystick x and y 
  ------------------------------------------------------------------------*/
void 
PTR_ResetJoyStick(
    void
)
{
    if (joyactive)
    {
        joy_mx = 0;
        joy_my = 0;
    }
}

/*------------------------------------------------------------------------
   PTR_ClipCursor () Clips cursor from screen
  ------------------------------------------------------------------------*/
void 
PTR_ClipCursor(
    void
)
{
    displaypic = cursorpic;
    ptrclip = 0;
    
    if (cursorxnew + CURSORWIDTH > SCREENWIDTH)
    {
        cursorloopx = SCREENWIDTH - cursorxnew;
        ptrclip = 1;
    }
    else if (cursorxnew < 0)
    {
        cursorloopx = cursorxnew + CURSORWIDTH;
        displaypic -= cursorxnew;
        cursorxnew = 0;
        ptrclip = 1;
    }
    else
        cursorloopx = CURSORWIDTH;
    
    if (cursorynew + CURSORHEIGHT > SCREENHEIGHT)
    {
        cursorloopy = SCREENHEIGHT - cursorynew;
        ptrclip = 1;
    }
    else if (cursorynew < 0)
    {
        cursorloopy = cursorynew + CURSORHEIGHT;
        displaypic += (-cursorynew) * CURSORWIDTH;
        cursorynew = 0;
        ptrclip = 1;
    }
    else
        cursorloopy = CURSORHEIGHT;
}

/*========================================================================
  PTR_UpdateCursor() - Updates Cursor
  ========================================================================*/
void 
PTR_UpdateCursor(
    void
)
{
    static int lasttick;
    int now = SDL_GetTicks();
    
    if (now - lasttick < 1000 / 15)
        return;
    
    lasttick += 1000 / 15;
    
    if (ptr_pause)
        return;
    
    if ((joyactive) && (!joy_ipt_MenuNew))
        PTR_JoyHandler();
    
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
            
            if (cursorloopy < 16)
                PTR_ClipSave();
            else 
                PTR_Save();
            
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

/*==========================================================================
  PTR_FrameHook() - Framehook Function
 ==========================================================================*/
void 
PTR_FrameHook(
    void (*update)(void)     // INPUT : pointer to function
)
{
    int ck_x2, ck_y2, ck_x1, ck_y1;
    
    if (!g_drawcursor)
    {
        update();
        return;
    }
   
    if (ptrboundshook)
        ptrboundshook();
    
    cursorxnew = cur_mx - cursoroffsetx;
    cursorynew = cur_my - cursoroffsety;
    
    ck_x2 = ud_x + ud_lx;
    ck_y2 = ud_y + ud_ly;
    ck_x1 = ud_x - CURSORWIDTH;
    ck_y1 = ud_y - CURSORHEIGHT;
    
    if (ck_x1 <= cursorxnew && ck_x2 >= cursorxnew && ck_y1 <= cursorynew && ck_y2 >= cursorynew)
    {
        if (ptrerase)
        {
            if (ck_x1 <= cursorx && ck_x2 >= cursorx && ck_y1 <= cursory && ck_y2 >= cursory)
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
        update();
        
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
        update();
    }
    
    if (ptrcursorhook)
        ptrcursorhook();
    
    cursorx = cursorxnew;
    cursory = cursorynew;
}

/***************************************************************************
   PTR_DrawCursor () - Turns Cursor Drawing to ON/OFF ( TRUE/FALSE )
 ***************************************************************************/
void 
PTR_DrawCursor(
    int flag               // INPUT: TRUE/FALSE
)
{
    if (ptractive)
    {
        if (!flag && ptrerase == 1)
        {
            if (ptrclip)
                PTR_ClipErase();
            else
                PTR_Erase();
            ptrerase = 0;
        }
        
        if (flag == 1)
            ptrupdate = 1;
        
        g_drawcursor = flag;
    }
    else
        g_drawcursor = 0;
}

/***************************************************************************
   PTR_SetPic () - Sets up a new cursor picture with hotspot
 ***************************************************************************/
void 
PTR_SetPic(
    texture_t *newp         // INPUT : pointer to new Cursor picture
)
{
    int loop;
    
    cursoroffsetx = 0;
    cursoroffsety = 0;
    
    if (ptractive)
    {
        for (loop = 0; loop < CURSORSIZE; loop++)
        {
            cursorpic[loop] = newp->charofs[loop];
            if ((uint8_t)newp->charofs[loop] == HOTSPOTCOLOR)
            {
                cursoroffsetx = loop % CURSORWIDTH;
                cursoroffsety = loop / CURSORWIDTH;
                cursorpic[loop] = newp->charofs[loop + 1];
            }
        }
        
        if (cursoroffsetx > 16)
            cursoroffsetx = 0;
        
        if (cursoroffsety > 16)
            cursoroffsety = 0;
        
        ptrupdate = 1;
    }
}

/***************************************************************************
 PTR_SetBoundsHook() - Sets User function to OK or change x,y values
 ***************************************************************************/
void                       // RETURN: none
PTR_SetBoundsHook(
    void (*func)(void)     // INPUT : pointer to function
)
{
    ptrboundshook = func;
    ptrupdate = 1;
}

/***************************************************************************
 PTR_SetCursorHook() - Sets User function to call from mouse or joy handler
 ***************************************************************************/
void                       // RETURN: none
PTR_SetCursorHook(
    void (*hook)(void)     // INPUT : pointer to function
)
{
    ptrcursorhook = hook;
    ptrupdate = 1;
}

/***************************************************************************
   PTR_SetUpdateFlag () - Sets cursor to be update next cycle
 ***************************************************************************/
void 
PTR_SetUpdateFlag(
    void
)
{
    ptrupdate = 1;
}

/***************************************************************************
 PTR_SetPos() - Sets Cursor Position
 ***************************************************************************/
void                       // RETURN: none
PTR_SetPos(
    int x,                 // INPUT : x position
    int y                  // INPUT : y position
)
{
    if (mousepresent)
        I_SetMousePos(x, y);
    
    cur_mx = x;
    cur_my = y;
    old_joy_x = x;
    old_joy_y = y;
    
    if (ptractive)
        ptrupdate = 1;
}

/***************************************************************************
PTR_Pause() - Pauses/ Starts PTR routines after already initing
 ***************************************************************************/
void 
PTR_Pause(
    int flag               // INPUT : TRUE / FALSE
)
{
    if (ptractive && flag != ptr_pause)
    {
        if (flag)
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
        
        ptr_pause = flag;
    }
}

/***************************************************************************
 PTR_Init() - Inits control type
 ***************************************************************************/
int                         // RETURN true = Installed, false  = No mouse
PTR_Init(
    int type                // INPUT : Pointer Type to Use
)
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
    
    if (type == P_JOYSTICK)
        joypresent = 1;
    
    if (type == P_AUTO || type == P_MOUSE || type == P_JOYSTICK)
        mousepresent = 1;
    
    if (type == P_JOYSTICK && joypresent)
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

/***************************************************************************
 PTR_End() - End Cursor system
 ***************************************************************************/
void 
PTR_End(
    void
)
{
    // if (ptr_tsm != -1)
    //     TSM_DelService(ptr_tsm);
}
