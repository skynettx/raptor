#include "SDL.h"
#include "common.h"
#include "gssapi.h"
#include "musapi.h"
#include "cards.h"
#include "fx.h"

musdevice_t *gss_device;

int gss_init;
int gssrate = 140;
int gsshack;

int gss_timer;
int gss_cnt;

void *gss_ptr;
int gss_currentptr;
int gss_priority;
int gss_handcnt;
int gss_handle;
int gss_sep;
int gss_volume;
int gss_type;
int gss_lastnote, gss_lastsmp;

/***************************************************************************
GSS_Init () - 
 ***************************************************************************/
void 
GSS_Init(
    int device, 
    int option
)
{
    if (gss_init)
        return;
    
    switch (device)
    {
    case CARD_NONE:
        gss_device = NULL;
        break;
    
    case CARD_ADLIB:
    case CARD_MV:
    case CARD_BLASTER:
        gss_device = &mus_device_fm;
        break;
    
    case CARD_MPU1:
    case CARD_MPU2:
    case CARD_MPU3:
    default:

        #ifdef _WIN32
        gss_device = &mus_device_mpu;
        #endif // _WIN32
        break;
    }

    if (gss_device && gss_device->Init)
        gss_device->Init(option);

    gsshack = 0;

    if (music_device && music_device == gss_device)
        gsshack = 1;

    gss_cnt = 0;
    gss_timer = SDL_GetTicks();
    gss_init = 1;
}

/***************************************************************************
GSS_DeInit () -
 ***************************************************************************/
void 
GSS_DeInit(
    void
)
{
    if (!gss_init)
        return;

    if (gss_device && gss_device->DeInit)
        gss_device->DeInit();
    
    gsshack = 0;
    gss_device = NULL;
    gss_init = 0;
}

/***************************************************************************
GSS_Service () -
 ***************************************************************************/
void 
GSS_Service(
    void
)
{
    if (!gss_init)
        return;
    
    switch (gss_type)
    {
    case 1:
    {
        gss1_t *gss = (gss1_t*)gss_ptr;
        int smp, note, bend;
        
        if (gss_currentptr == 0)
        {
            // setup playback
            if (gss_lastnote)
            {
                if (gss_device && gss_device->KeyOffEvent)
                    gss_device->KeyOffEvent(14, gss_lastnote);
                
                gss_lastnote = 0;
            }
            if (gss_device && gss_device->ControllerEvent)
            {
                gss_device->ControllerEvent(14, 4, gss_sep >> 1);
                gss_device->ControllerEvent(14, 1, gss->bank);
                gss_device->ControllerEvent(14, 0, gss->patch);
            }
            if (gss_device && gss_device->PitchBendEvent)
                gss_device->PitchBendEvent(14, 127);
        }
        else if (gss_currentptr >= gss->len)
        {
            if (gss_lastnote)
            {
                if (gss_device && gss_device->KeyOffEvent)
                    gss_device->KeyOffEvent(14, gss_lastnote);
                
                gss_lastnote = 0;
            }
            gss_type = 0;
            gss_ptr = NULL;
            break;
        }
        smp = gss->data[gss_currentptr++];
        
        if (gss_lastsmp != smp)
        {
            if (smp == 0 && gss_lastnote)
            {
                if (gss_device && gss_device->KeyOffEvent)
                    gss_device->KeyOffEvent(14, gss_lastnote);
                
                gss_lastnote = 0;
            }
            else
            {
                note = 29 + ((smp - 1) >> 1);
                bend = (smp & 1) ? 127 : 159;
                
                if (note != gss_lastnote)
                {
                    if (gss_device && gss_device->KeyOffEvent)
                        gss_device->KeyOffEvent(14, gss_lastnote);
                    
                    if (gss_device && gss_device->PitchBendEvent)
                        gss_device->PitchBendEvent(14, bend);
                    
                    if (gss_device && gss_device->KeyOnEvent)
                        gss_device->KeyOnEvent(14, note, gss_volume);
                    
                    gss_lastnote = note;
                }
                else
                {
                    if (gss_device && gss_device->PitchBendEvent)
                        gss_device->PitchBendEvent(14, bend);
                }
            }
            gss_lastsmp = smp;
        }
        break;
    }
    }
}

/***************************************************************************
GSS_Mix () -
 ***************************************************************************/
void 
GSS_Mix(
    int16_t *stream, 
    int len
)
{
    int i;
    
    if (!gss_init)
        return;

    if (!gss_init || !gss_device || !gss_device->Mix || gsshack)
        return;

    for (i = 0; i < len; i++)
    {
        gss_device->Mix(stream, 1);
        gss_cnt += gssrate;
        
        while (gss_cnt >= fx_freq)
        {
            gss_cnt -= fx_freq;
            GSS_Service();
        }
        stream += 2;
    }
}

/***************************************************************************
GSS_Poll () -
 ***************************************************************************/
void 
GSS_Poll(
    void
)
{
    if (!gss_init || !gss_device || gss_device->Mix)
        return;

    int now = SDL_GetTicks();
    
    while (gss_timer < now)
    {
        gss_cnt += gssrate;
        
        if (gss_cnt >= 1000)
        {
            gss_cnt -= 1000;
            GSS_Service();
        }
        gss_timer++;
    }
}

/***************************************************************************
GSS_PlayPatch () -
 ***************************************************************************/
int 
GSS_PlayPatch(
    void *gss, 
    int sep, 
    int pitch, 
    int volume, 
    int priority
)
{
    int format = *(int16_t*)gss;
    int handle = (gss_handcnt++) & FXHAND_MASK;
    
    if (format != 1 && format != 2)
        return -1;

    SND_Lock();

    if (gss_type != 0 && priority < gss_priority)
    {
        SND_Unlock();
        return -1;
    }

    if (gss_type == 0)
        gss_lastnote = 0;
    
    gss_type = 0;
    gss_ptr = 0;

    gss_priority = priority;
    gss_sep = sep;
    gss_volume = volume;
    
    switch (format)
    {
    case 1:
    {
        gss_currentptr = 0;
        gss_lastsmp = 0;
        gss_ptr = gss;
        gss_type = 1;
        break;
    }
    
    case 2:
        gss_currentptr = 0;
        gss_ptr = gss;
        gss_type = 2;
        break;
    }
    gss_handle = handle;

    SND_Unlock();
    
    return handle | FXHAND_GSS1;
}

/***************************************************************************
GSS_StopPatch () -
 ***************************************************************************/
void 
GSS_StopPatch(
    int handle
)
{
    if (!gss_init)
        return;
    
    handle &= FXHAND_MASK;
    
    SND_Lock();
    
    if (handle == gss_handle)
    {
        switch (gss_type)
        {
        case 1:
        {
            gss1_t *gss = (gss1_t*)gss_ptr;
            gss_currentptr = gss->len;
        }
        }
    }
    SND_Unlock();
}

/***************************************************************************
GSS_PatchIsPlaying () -
 ***************************************************************************/
int 
GSS_PatchIsPlaying(
    int handle
)
{
    int stat = 0;
    
    if (!gss_init)
        return 0;
    
    handle &= FXHAND_MASK;
    
    SND_Lock();
    
    if (handle == gss_handle && gss_type != 0)
        stat = 1;
    
    SND_Unlock();
    
    return stat;
}
