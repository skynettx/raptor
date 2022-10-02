#include <stdio.h>
#include "i_video.h"
#include "common.h"
#include "ptrapi.h"
#include "imsapi.h"
#include "kbdapi.h"
#include "joyapi.h"

/***************************************************************************
IMS_StartAck () - Starts up checking for a happening
 ***************************************************************************/
void 
IMS_StartAck(
    void
)
{
    KBD_Clear();
    
    mouse_b1_ack = 0;
    mouse_b2_ack = 0;
    mouse_b3_ack = 0;
    kbd_ack = 0;
    joy_ack = 0;
}

/***************************************************************************
IMS_CheckAck () - Tells if something has happend since last IMS_StartAck
 ***************************************************************************/
int 
IMS_CheckAck(
    void
)                                 
{
    I_GetEvent();
    int rval;
    rval = 0;
    
    if (mouse_b1_ack)
        rval = 1;
    
    if (mouse_b2_ack)
        rval = 1;
    
    if (kbd_ack)
        rval = 1;
    
    if (joy_ack)
        rval = 1;
    
    return rval;
}

/***************************************************************************
IMS_IsAck() - Returns TRUE if ptr button or key pressed
 ***************************************************************************/
int 
IMS_IsAck(
    void
)
{
    I_GetEvent();
    
    if (Up || Down || Left || Right || Start || Back || LeftShoulder || RightShoulder || AButton || BButton || XButton || YButton)
    {
        return 1;
    }
    
    if (lastscan)
    {
        lastscan = 0;
        return 1;
    }
    
    if (mouseb1 || mouseb2 || mouseb3)
        return 1;
    
    return 0;
}

/***************************************************************************
IMS_WaitTimed() - Wait for aprox secs
 ***************************************************************************/
int                        // RETURN: keypress (lastscan)
IMS_WaitTimed(
    int secs               // INPUT : seconds to wait
)
{
    int loop, hold, rval;
    rval = 0;
    lastscan = 0;
    
    IMS_StartAck();
    
    while (secs > 0)
    {
        for (loop = 0; loop < 55; loop++)
        {
            hold = GFX_GetFrameCount();
            while (GFX_GetFrameCount() == hold)
            {
            }
            
            if (IMS_CheckAck())
            {
                rval = 1;
                goto end_func;
            }
        }
        secs--;
    }

end_func:
    loop = 100;
    while (IMS_IsAck())
    {
        loop--;
    }
    
    IMS_StartAck();
    
    return rval;
}
