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

int control = 1;
int haptic;

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

void IPT_GetButtons(void)
{
    static int lasttick;
    int now = SDL_GetTicks();
    if (now - lasttick < 1000 / 26)
        return;
    lasttick += 1000 / 26;
    if (!ipt_start)
        return;

#if 0
    int v1c;
    if (control == 2)
    {
        v1c = inp(0x2);
        v1c >>= 4;
        if ((v1c & 1) == 0)
            buttons[j_lookup[0]] = 1;
        if ((v1c & 2) == 0)
            buttons[j_lookup[1]] = 1;
        if ((v1c & 4) == 0)
            buttons[j_lookup[2]] = 1;
        if ((v1c & 8) == 0)
            buttons[j_lookup[3]] = 1;
    }
#endif
    
    if (keyboard[k_Fire])
        buttons[0] = 1;
    if (keyboard[k_FireSp])
        buttons[1] = 1;
    if (keyboard[k_ChangeSp])
        buttons[2] = 1;
    if (keyboard[k_Mega])
        buttons[3] = 1;
}

void IPT_GetJoyStick(void)
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
        if (-g_addx > 10)
            g_addx = -10;
    }
    else if (Right)
    {
             if (g_addx <= 0)
                 g_addx = 1;
             g_addx++;
             if (g_addx > 10)
                 g_addx = 10;
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
        if (-g_addy > 8)
            g_addy = -8;
    }
    else if (Down)
    {
             if (g_addy <= 0)
                 g_addy = 1;
             g_addy++;
             if (g_addy > 8)
                 g_addy = 8;
    }
    else
    {
        if (g_addy)
            g_addy /= 2;
    }

    //Move Player Analog Stick

    if (StickX != 0)
    {
        StickX >>= 3;
        if (!StickX)
            StickX = 1;
        if (StickX > 0)
            StickX = 8;
        if (StickX < 0)
            StickX = -8;
        g_addx = StickX;
    }
    if (StickY != 0)
    {
        StickY >>= 3;
        if (!StickY)
            StickY = 1;
        if (StickY > 0)
            StickY = 8;
        if (StickY < 0)
            StickY = -8;
        g_addy = StickY;
    }
}

void IPT_GetKeyBoard(void)
{
    if (keyboard[k_Left] || keyboard[k_Right])
    {
        if (keyboard[k_Left])
        {
            if (g_addx >= 0)
                g_addx = -1;
            g_addx--;
            if (-g_addx > 10)
                g_addx = -10;
        }
        else if (keyboard[k_Right])
        {
            if (g_addx <= 0)
                g_addx = 1;
            g_addx++;
            if (g_addx > 10)
                g_addx = 10;
        }
    }
    else
    {
        if (g_addx)
            g_addx /= 2;
    }
    if (keyboard[k_Up] || keyboard[k_Down])
    {
        if (keyboard[k_Up])
        {
            if (g_addy >= 0)
                g_addy = -1;
            g_addy--;
            if (-g_addy > 8)
                g_addy = -8;
        }
        else if (keyboard[k_Down])
        {
            if (g_addy <= 0)
                g_addy = 1;
            g_addy++;
            if (g_addy > 8)
                g_addy = 8;
        }
    }
    else
    {
        if (g_addy)
            g_addy /= 2;
    }
}

void IPT_GetMouse(void)
{
    int v1c, v20, v24, v28;
    v1c = playerx + 16;
    v20 = playery + 16;
    v24 = cur_mx;
    v28 = cur_my;
    xm = v24 - v1c;
    ym = v28 - v20;
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

bool IPT_MouseGrab(void)
{
    return ipt_start;
}

void IPT_Init(void)
{
    
    I_SetGrabMouseCallback(IPT_MouseGrab);
    // ipt_tsm = TSM_NewService(IPT_GetButtons, 26, 254, 1);
    IPT_CalJoy();
}

void IPT_DeInit(void)
{
    // TSM_DelService(ipt_tsm);
}

void IPT_Start(void)
{
    PTR_DrawCursor(0);
    PTR_Pause(1);
    ipt_start = 1;
    // TSM_ResumeService(ipt_tsm);
}

void IPT_End(void)
{
    ipt_start = 0;
    // TSM_PauseService(ipt_tsm);
    PTR_Pause(0);
    PTR_DrawCursor(0);
}

void IPT_MovePlayer(void)
{
    static int oldx = 144;
    int v1c;
    if (demo_mode == 2)
        return;
    if (!control_pause)
    {
        switch (control)
        {
        case 0:
        default:
            IPT_GetKeyBoard();
            break;
        case 2:
            IPT_GetJoyStick();
            break;
        case 1:
            IPT_GetMouse();
            break;
        }
    }
    playerx += g_addx;
    playery += g_addy;
    if (startendwave == -1)
    {
        if (playery < 0)
        {
            playery = 0;
            g_addy = 0;
        }
        else if (playery > 160)
        {
            playery = 160;
            g_addy = 0;
        }
        if (playerx < 5)
        {
            playerx = 5;
            g_addx = 0;
        }
        else if (playerx + 32 > 314)
        {
            playerx = 314 - 32;
            g_addx = 0;
        }
    }
    v1c = abs(playerx - oldx);
    v1c >>= 2;
    if (v1c > 3)
        v1c = 3;
    if (playerx < oldx)
    {
        if (playerbasepic + v1c > playerpic)
            playerpic++;
    }
    else if (playerx > oldx)
    {
        if (playerbasepic - v1c < playerpic)
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
    player_cx = playerx + 16;
    player_cy = playery + 16;
}

void IPT_PauseControl(int a1)
{
    control_pause = a1;
}

void IPT_FMovePlayer(int a1, int a2)
{
    g_addx = a1;
    g_addy = a2;
    IPT_MovePlayer();
}

void IPT_LoadPrefs(void)
{
    opt_detail = INI_GetPreferenceLong("Setup", "Detail", 1);
    control = INI_GetPreferenceLong("Setup", "Control", 0);
    haptic = INI_GetPreferenceLong("Setup", "Haptic", 1);
    k_Up = INI_GetPreferenceLong("Keyboard", "MoveUp", 0x48);
    k_Down = INI_GetPreferenceLong("Keyboard", "MoveDn", 0x50);
    k_Left = INI_GetPreferenceLong("Keyboard", "MoveLeft", 0x4b);
    k_Right = INI_GetPreferenceLong("Keyboard", "MoveRight", 0x4d);
    k_Fire = INI_GetPreferenceLong("Keyboard", "Fire", 0x1d);
    k_FireSp = INI_GetPreferenceLong("Keyboard", "FireSp", 0x38);
    k_ChangeSp = INI_GetPreferenceLong("Keyboard", "ChangeSp", 0x39);
    k_Mega = INI_GetPreferenceLong("Keyboard", "MegaFire", 0x36);
    m_lookup[0] = INI_GetPreferenceLong("Mouse", "Fire", 0);
    m_lookup[1] = INI_GetPreferenceLong("Mouse", "FireSp", 1);
    m_lookup[2] = INI_GetPreferenceLong("Mouse", "ChangeSp", 2);
    j_lookup[0] = INI_GetPreferenceLong("JoyStick", "Fire", 0);
    j_lookup[1] = INI_GetPreferenceLong("JoyStick", "FireSp", 1);
    j_lookup[2] = INI_GetPreferenceLong("JoyStick", "ChangeSp", 2);
    j_lookup[3] = INI_GetPreferenceLong("JoyStick", "MegaFire", 3);
}
