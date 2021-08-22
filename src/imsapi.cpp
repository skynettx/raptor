#include <stdio.h>
#include "i_video.h"
#include "common.h"
#include "ptrapi.h"
#include "imsapi.h"
#include "kbdapi.h"

void IMS_StartAck(void)
{
    KBD_Clear();
    mouse_b1_ack = 0;
    mouse_b2_ack = 0;
    mouse_b3_ack = 0;
    kbd_ack = 0;
}

int IMS_CheckAck(void)
{
    I_GetEvent();
    int va;
    va = 0;
    if (mouse_b1_ack)
        va = 1;
    if (mouse_b2_ack)
        va = 1;
    if (kbd_ack)
        va = 1;
    return va;
}

int IMS_IsAck(void)
{
    I_GetEvent();
    if (lastscan)
    {
        lastscan = 0;
        return 1;
    }
    if (mouseb1 || mouseb2 || mouseb3)
        return 1;
    return 0;
}

int IMS_WaitTimedSwd(int a1)
{
    int i, v10, vb;
    vb = 0;
    lastscan = 0;
    IMS_StartAck();
    while (a1 > 0)
    {
        for (i = 0; i < 55; i++)
        {
            v10 = GFX_GetFrameCount();
            while (GFX_GetFrameCount() == v10)
            {
            }
            if (IMS_CheckAck())
            {
                vb = 1;
                goto _LABEL1;
            }
        }
        a1--;
    }
_LABEL1:
    i = 100;
    while (IMS_IsAck())
    {
        i--;
    }
    IMS_StartAck();
    return vb;
}
