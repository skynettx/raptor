#include "SDL.h"
#include "stdio.h"
#include "common.h"
#include "prefapi.h"
#include "windows.h"
#include "kbdapi.h"
#include "ptrapi.h"
#include "rap.h"
#include "demo.h"
#include "input.h"
#include "i_video.h"
#include "joyapi.h"

#define MAX_ADDX 10
#define MAX_ADDY 8

int control = 1;
int haptic;
int joy_ipt_MenuNew;

int k_Up;
int k_Down;
int k_Left;
int k_Right;
int k_Fire;
int k_FireSp;
int k_ChangeSp;
int k_Mega;

int m_lookup[3];
int j_lookup[4];

int buttons[4];

int xm, ym;
int g_addx, g_addy;

int ipt_start;
int control_pause;

/*------------------------------------------------------------------------
   IPT_GetButtons () - Reads in Joystick and Keyboard game buttons
  ------------------------------------------------------------------------*/
void 
IPT_GetButtons(
    void
)
{
    static int lasttick;
    int now = SDL_GetTicks();
    
    if (now - lasttick < 1000 / 26)
        return;
    
    lasttick += 1000 / 26;
    
    if (!ipt_start)
        return;

#if 0
    int num;
    
    if (control == I_JOYSTICK)
    {
        num = inp(0x2);
        
        num >>= 4;
        
        if ((num & 1) == 0)
            buttons[j_lookup[0]] = 1;
        if ((num & 2) == 0)
            buttons[j_lookup[1]] = 1;
        if ((num & 4) == 0)
            buttons[j_lookup[2]] = 1;
        if ((num & 8) == 0)
            buttons[j_lookup[3]] = 1;
    }
#endif
    
    if (KBD_Key(k_Fire))
        buttons[0] = 1;
    
    if (KBD_Key(k_FireSp))
        buttons[1] = 1;
    
    if (KBD_Key(k_ChangeSp))
        buttons[2] = 1;
    
    if (KBD_Key(k_Mega))
        buttons[3] = 1;
}

/*------------------------------------------------------------------------
IPT_GetJoyStick()
  ------------------------------------------------------------------------*/
void 
IPT_GetJoyStick(
    void
)
{
    //Get Button
    
    if (AButton)
    {
        if (AButtonconvert == j_lookup[0])                        //Fire
            buttons[0] = 1;
        if (AButtonconvert == j_lookup[1])                        //Fire Special
            buttons[1] = 1;
        if (AButtonconvert == j_lookup[2])                        //Change Special
            buttons[2] = 1;
        if (AButtonconvert == j_lookup[3])                        //Mega
            buttons[3] = 1;
    }
    
    if (BButton)
    {
        if (BButtonconvert == j_lookup[0])                        //Fire
            buttons[0] = 1;
        if (BButtonconvert == j_lookup[1])                        //Fire Special
            buttons[1] = 1;
        if (BButtonconvert == j_lookup[2])                        //Change Special
            buttons[2] = 1;
        if (BButtonconvert == j_lookup[3])                        //Mega
            buttons[3] = 1;
    }
    
    if (XButton)
    {
        if (XButtonconvert == j_lookup[0])                        //Fire
            buttons[0] = 1;
        if (XButtonconvert == j_lookup[1])                        //Fire Special
            buttons[1] = 1;
        if (XButtonconvert == j_lookup[2])                        //Change Special
            buttons[2] = 1;
        if (XButtonconvert == j_lookup[3])                        //Mega
            buttons[3] = 1;
    }
    
    if (YButton)
    {
        if (YButtonconvert == j_lookup[0])                        //Fire
            buttons[0] = 1;
        if (YButtonconvert == j_lookup[1])                        //Fire Special
            buttons[1] = 1;
        if (YButtonconvert == j_lookup[2])                        //Change Special
            buttons[2] = 1;
        if (YButtonconvert == j_lookup[3])                        //Mega
            buttons[3] = 1;
    }
    
    if (TriggerRight > 0)                                         //Fire
        buttons[0] = 1;
    if (TriggerLeft > 0)                                          //Fire Special
        buttons[1] = 1;
    if (LeftShoulder)                                             //Change Special
        buttons[2] = 1;
    if (RightShoulder)                                            //Mega
        buttons[3] = 1;
       
    //Move Player DPad
           
    if (Left)
    {
        if (g_addx >= 0)
            g_addx = -1;
        g_addx--;
        if (-g_addx > MAX_ADDX)
            g_addx = -MAX_ADDX;
    }
    else if (Right)
    {
             if (g_addx <= 0)
                 g_addx = 1;
             g_addx++;
             if (g_addx > MAX_ADDX)
                 g_addx = MAX_ADDX;
    }
    else
    {
        if (g_addx)
            g_addx /= 2;
    }
    
    if (Up)
    {
        if (g_addy >= 0)
            g_addy = -1;
        g_addy--;
        if (-g_addy > MAX_ADDY)
            g_addy = -MAX_ADDY;
    }
    else if (Down)
    {
             if (g_addy <= 0)
                 g_addy = 1;
             g_addy++;
             if (g_addy > MAX_ADDY)
                 g_addy = MAX_ADDY;
    }
    else
    {
        if (g_addy)
            g_addy /= 2;
    }

    //Move Player Analog Stick

    if (StickX != 0)
    {
        if (StickX > 0)
            StickX *= 2;
        if (StickX > MAX_ADDX)
            StickX = MAX_ADDX;
        if (StickX < 0)
            StickX *= 2;
        if (StickX < -MAX_ADDX)
            StickX = -MAX_ADDX;
        g_addx = StickX;
    }
    
    if (StickY != 0)
    {
        if (StickY > 0)
            StickY *= 2;
        if (StickY > MAX_ADDY)
            StickY = MAX_ADDY;
        if (StickY < 0)
            StickY *= 2;
        if (StickY < -MAX_ADDY)
            StickY = -MAX_ADDY;
        g_addy = StickY;
    }
}

/*------------------------------------------------------------------------
IPT_GetKeyBoard (
  ------------------------------------------------------------------------*/
void 
IPT_GetKeyBoard(
    void
)
{
    if (KBD_Key(k_Left) || KBD_Key(k_Right))
    {
        if (KBD_Key(k_Left))
        {
            if (g_addx >= 0)
                g_addx = -1;
            g_addx--;
            if (-g_addx > MAX_ADDX)
                g_addx = -MAX_ADDX;
        }
        else if (KBD_Key(k_Right))
        {
            if (g_addx <= 0)
                g_addx = 1;
            g_addx++;
            if (g_addx > MAX_ADDX)
                g_addx = MAX_ADDX;
        }
    }
    else
    {
        if (g_addx)
            g_addx /= 2;
    }
    
    if (KBD_Key(k_Up) || KBD_Key(k_Down))
    {
        if (KBD_Key(k_Up))
        {
            if (g_addy >= 0)
                g_addy = -1;
            g_addy--;
            if (-g_addy > MAX_ADDY)
                g_addy = -MAX_ADDY;
        }
        else if (KBD_Key(k_Down))
        {
            if (g_addy <= 0)
                g_addy = 1;
            g_addy++;
            if (g_addy > MAX_ADDY)
                g_addy = MAX_ADDY;
        }
    }
    else
    {
        if (g_addy)
            g_addy /= 2;
    }
}

/*------------------------------------------------------------------------
IPT_GetMouse (
  ------------------------------------------------------------------------*/
void 
IPT_GetMouse(
    void
)
{
    int plx, ply, ptrx, ptry;
    
    plx = playerx + (PLAYERWIDTH / 2);
    ply = playery + (PLAYERHEIGHT / 2);
    
    ptrx = cur_mx;
    ptry = cur_my;
    
    xm = ptrx - plx;
    ym = ptry - ply;
    
    if (xm)
    {
        xm >>= 3;
        
        if (!xm)
            xm = 1;
        else if (xm > 10)
            xm = 10;
        else if (xm < -10)
            xm = -10;
    }
    
    if (ym)
    {
        ym >>= 3;
        
        if (!ym)
            ym = 1;
        else if (ym > 10)
            ym = 10;
        else if (ym < -10)
            ym = -10;
    }
    
    g_addx = xm;
    g_addy = ym;
    
    if (mouseb1)
        buttons[m_lookup[0]] = 1;
    
    if (mouseb2)
        buttons[m_lookup[1]] = 1;
    
    if (mouseb3)
        buttons[m_lookup[2]] = 1;
}

/*------------------------------------------------------------------------
IPT_MouseGrab (
  ------------------------------------------------------------------------*/
bool 
IPT_MouseGrab(
    void
)
{
    return ipt_start;
}

/***************************************************************************
IPT_Init () - Initializes INPUT system
 ***************************************************************************/
void 
IPT_Init(
    void
)
{
    
    I_SetGrabMouseCallback(IPT_MouseGrab);
    // ipt_tsm = TSM_NewService(IPT_GetButtons, 26, 254, 1);
    IPT_CalJoy();
}

/***************************************************************************
IPT_DeInit() - Freeze up resources used by INPUT system
 ***************************************************************************/
void 
IPT_DeInit(
    void
)
{
    // TSM_DelService(ipt_tsm);
}

/***************************************************************************
IPT_Start() - Tranfers control from PTRAPI stuff to IPT stuff
 ***************************************************************************/
void 
IPT_Start(
    void
)
{
    PTR_DrawCursor(0);
    PTR_Pause(1);
    ipt_start = 1;
    // TSM_ResumeService(ipt_tsm);
}

/***************************************************************************
IPT_End() - Tranfers control from IPT stuff to PTR stuff
 ***************************************************************************/
void 
IPT_End(
    void
)
{
    ipt_start = 0;
    // TSM_PauseService(ipt_tsm);
    PTR_Pause(0);
    PTR_DrawCursor(0);
}

/***************************************************************************
IPT_MovePlayer() - Perform player movement for current input device
 ***************************************************************************/
void 
IPT_MovePlayer(
    void
)
{
    static int oldx = PLAYERINITX;
    int delta;
    
    if (demo_mode == DEMO_PLAYBACK)
        return;
    
    if (!control_pause)
    {
        switch (control)
        {
        case I_KEYBOARD:
        default:
            IPT_GetKeyBoard();
            break;
        
        case I_JOYSTICK:
            IPT_GetJoyStick();
            break;

        case I_MOUSE:
            IPT_GetMouse();
            break;
        }
    }
    
    playerx += g_addx;
    playery += g_addy;
    
    if (startendwave == -1)
    {
        if (playery < MINPLAYERY)
        {
            playery = MINPLAYERY;
            g_addy = 0;
        }
        else if (playery > MAXPLAYERY)
        {
            playery = MAXPLAYERY;
            g_addy = 0;
        }
        
        if (playerx < PLAYERMINX)
        {
            playerx = PLAYERMINX;
            g_addx = 0;
        }
        else if (playerx + PLAYERWIDTH > PLAYERMAXX)
        {
            playerx = PLAYERMAXX - PLAYERWIDTH;
            g_addx = 0;
        }
    }
    
    delta = abs(playerx - oldx);
    delta >>= 2;
    
    if (delta > 3)
        delta = 3;
    
    if (playerx < oldx)
    {
        if (playerbasepic + delta > playerpic)
            playerpic++;
    }
    else if (playerx > oldx)
    {
        if (playerbasepic - delta < playerpic)
            playerpic--;
    }
    else
    {
        if (playerpic > playerbasepic)
            playerpic--;
        else if (playerpic < playerbasepic)
            playerpic++;
    }
    
    oldx = playerx;
    
    player_cx = playerx + (PLAYERWIDTH / 2);
    player_cy = playery + (PLAYERHEIGHT / 2);
}

/***************************************************************************
IPT_PauseControl() - Lets routines run without letting user control anyting
 ***************************************************************************/
void 
IPT_PauseControl(
    int flag
)
{
    control_pause = flag;
}

/***************************************************************************
IPT_FMovePlayer() - Forces player to move addx/addy
 ***************************************************************************/
void 
IPT_FMovePlayer(
    int addx,              // INPUT : add to x pos
    int addy               // INPUT : add to y pos
)
{
    g_addx = addx;
    g_addy = addy;
    
    IPT_MovePlayer();
}

/***************************************************************************
IPT_LoadPrefs() - Load Input Prefs from setup.ini
 ***************************************************************************/
void 
IPT_LoadPrefs(
    void
)
{
    opt_detail = INI_GetPreferenceLong("Setup", "Detail", 1);
    control = INI_GetPreferenceLong("Setup", "Control", 0);
    haptic = INI_GetPreferenceLong("Setup", "Haptic", 1);
    joy_ipt_MenuNew = INI_GetPreferenceLong("Setup", "joy_ipt_MenuNew", 0);
    
    k_Up = INI_GetPreferenceLong("Keyboard", "MoveUp", SC_UP);
    k_Down = INI_GetPreferenceLong("Keyboard", "MoveDn", SC_DOWN);
    k_Left = INI_GetPreferenceLong("Keyboard", "MoveLeft", SC_LEFT);
    k_Right = INI_GetPreferenceLong("Keyboard", "MoveRight", SC_RIGHT);
    k_Fire = INI_GetPreferenceLong("Keyboard", "Fire", SC_CTRL);
    k_FireSp = INI_GetPreferenceLong("Keyboard", "FireSp", SC_ALT);
    k_ChangeSp = INI_GetPreferenceLong("Keyboard", "ChangeSp", SC_SPACE);
    k_Mega = INI_GetPreferenceLong("Keyboard", "MegaFire", SC_RIGHT_SHIFT);
    
    m_lookup[0] = INI_GetPreferenceLong("Mouse", "Fire", 0);
    m_lookup[1] = INI_GetPreferenceLong("Mouse", "FireSp", 1);
    m_lookup[2] = INI_GetPreferenceLong("Mouse", "ChangeSp", 2);
    
    j_lookup[0] = INI_GetPreferenceLong("JoyStick", "Fire", 0);
    j_lookup[1] = INI_GetPreferenceLong("JoyStick", "FireSp", 1);
    j_lookup[2] = INI_GetPreferenceLong("JoyStick", "ChangeSp", 2);
    j_lookup[3] = INI_GetPreferenceLong("JoyStick", "MegaFire", 3);
}
