#include <stdio.h>
#include <string.h>
#include "SDL.h"
#include "common.h"
#include "glbapi.h"
#include "i_video.h"
#include "i_oplmusic.h"
#include "musapi.h"
#include "prefapi.h"
#include "cards.h"
#include "fx.h"
#include "dspapi.h"
#include "rap.h"
#include "gssapi.h"
#include "fileids.h"
#include "entypes.h"

int music_volume;
int dig_flag;
int fx_device;
int fx_volume;
static int fx_init = 0;
static int lockcount;
int fx_freq = 44100;
int music_song = -1;
int fx_gus;
int fx_channels;
int sys_midi, winmm_mpu_device, core_dls_synth, core_midi_port, alsaclient, alsaport;

typedef struct
{
    int item;         // GLB ITEM
    int pri;          // PRIORITY 0=LOW
    int pitch;        // PITCH TO PLAY PATCH
    int rpflag;       // TRUE = RANDOM PITCHES
    int sid;          // DMX EFFECT ID
    int vol;          // VOLUME
    int gcache;       // CACHE FOR IN GAME USE ?
    int odig;         // TRUE = ONLY PLAY DIGITAL
}DFX;

DFX fx_items[FX_LAST_SND];
int fx_loaded;

SDL_AudioDeviceID fx_dev;

char cards[M_LAST][23] = {
    "None",
    "PC Speaker",
    "Adlib",
    "Gravis Ultra Sound",
    "Pro Audio Spectrum",
    "Sound Blaster",
    "WaveBlaster",
    "Roland Sound Canvas",
    "General Midi",
    "Sound Blaster AWE 32",
};

/***************************************************************************
FX_Fill() -
 ***************************************************************************/
static void 
FX_Fill(
    void *userdata, 
    Uint8 *stream, 
    int len
)
{
    memset(stream, 0, len);
    int16_t *stream16 = (int16_t*)stream;
    len /= 4;
    MUS_Mix(stream16, len);
    GSS_Mix(stream16, len);
    DSP_Mix(stream16, len);
}

/***************************************************************************
SND_InitSound () - Does bout all i can think of for Music/FX initing
 ***************************************************************************/
int 
SND_InitSound(
    void
)
{
    int music_card, fx_card, fx_chans;
    char *genmidi = NULL;
    SDL_AudioSpec spec = {}, actual = {};
    
    if (fx_init)
        return 0;

    if (SDL_Init(SDL_INIT_AUDIO) < 0)
        return 0;

    spec.freq = fx_freq;
    spec.format = AUDIO_S16SYS;
    spec.channels = 2;
    spec.samples = 512;
    spec.callback = FX_Fill;
    spec.userdata = NULL;

    if ((fx_dev = SDL_OpenAudioDevice(NULL, 0, &spec, &actual, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE)) == 0)
    {
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }

    fx_freq = actual.freq;
    
    if (actual.format != AUDIO_S16SYS || actual.channels != 2)
    {
        SDL_CloseAudio();
        SDL_QuitSubSystem(SDL_INIT_AUDIO);
        return 0;
    }

    dig_flag = 0;
    fx_device = SND_NONE;

    music_volume = INI_GetPreferenceLong("Music", "Volume", 127);
    music_card = INI_GetPreferenceLong("Music", "CardType", M_NONE);
    sys_midi = INI_GetPreferenceLong("Setup", "sys_midi", 0);
    winmm_mpu_device = INI_GetPreferenceLong("Setup", "winmm_mpu_device", 0);
    core_dls_synth = INI_GetPreferenceLong("Setup", "core_dls_synth", 1);
    core_midi_port = INI_GetPreferenceLong("Setup", "core_midi_port", 0);
    alsaclient = INI_GetPreferenceLong("Setup", "alsa_output_client", 128);
    alsaport = INI_GetPreferenceLong("Setup", "alsa_output_port", 0);

    switch (music_card)
    {
    case M_ADLIB:
    case M_PAS:
    case M_SB:
        genmidi = GLB_GetItem(FILE00e_GENMIDI_OP2);
        if (genmidi)
        {
            LoadInstrumentTable(genmidi);
            GLB_FreeItem(14);
        }
        break;
    }

    printf("Music Enabled (%s)\n", cards[music_card]);
    
    if (music_card != M_NONE)                               
    {
        MUS_Init(music_card, 0);
        MUS_SetVolume(music_volume);
    }

    fx_volume = INI_GetPreferenceLong("SoundFX", "Volume", 127);
    fx_card = INI_GetPreferenceLong("SoundFX", "CardType", 0);
    fx_chans = INI_GetPreferenceLong("SoundFX", "Channels", 2);
    
    switch (fx_card)
    {
    default:
    case M_NONE:
        fx_device = SND_NONE;
        break;
    
    case M_PC:
        fx_device = SND_PC;
        break;
    
    case M_ADLIB:
        fx_device = SND_MIDI;
        if (!genmidi)
        {
            genmidi = GLB_GetItem(FILE00e_GENMIDI_OP2);
            if (genmidi)
            {
                LoadInstrumentTable(genmidi);
                GLB_FreeItem(14);
            }
        }
        break;
    
    case M_GUS:
    case M_PAS:
    case M_SB:
        fx_device = SND_DIGITAL;
        dig_flag = 1;
        break;
    
    case M_WAVE:
    case M_CANVAS:
    case M_GMIDI:
        fx_device = SND_MIDI;
        break;
    }

    printf("SoundFx Enabled (%s)\n", cards[fx_card]);

    if (fx_chans < 1 || fx_chans > 8)
        fx_chans = 2;

    if (fx_card == M_SB || fx_card == M_GUS || fx_card == M_PAS)
    {
        fx_channels = fx_chans;
        if (fx_card == M_GUS && fx_channels < 2)
            fx_gus = 1;
        DSP_Init(fx_channels, 11025);
    }
    else
        fx_channels = 1;

    if (fx_card == M_ADLIB || fx_card == M_WAVE || fx_card == M_CANVAS || fx_card == M_GMIDI)
        GSS_Init(fx_card, 0);

    SDL_PauseAudioDevice(fx_dev, 0);

    fx_init = 1;
    
    return 1;
}

/***************************************************************************
SND_DeInit () -
 ***************************************************************************/
void SND_DeInit(
    void
)
{
    if (!fx_init)
        return;

    SDL_QuitSubSystem(SDL_INIT_AUDIO);
    fx_init = 0;
}

/***************************************************************************
SND_Setup() - Inits SND System  called after SND_InitSound() and GLB_Init
 ***************************************************************************/
void 
SND_Setup(
    void
)
{
    int loop;
    DFX *lib;
    
    memset(fx_items, 0, sizeof(fx_items));
    
    if (fx_device == SND_NONE)
    {
        fx_loaded = 0;
        return;
    }
    
    fx_loaded = 1;

    // MONKEY 1 EFFECT ======================
    lib = &fx_items[FX_MON1];
    lib->item = GLB_GetItemID("MON1_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // MONKEY 2 EFFECT ======================
    lib = &fx_items[FX_MON2];
    lib->item = GLB_GetItemID("MON2_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // MONKEY 3 EFFECT ======================
    lib = &fx_items[FX_MON3];
    lib->item = GLB_GetItemID("MON3_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // MONKEY 4 EFFECT ======================
    lib = &fx_items[FX_MON4];
    lib->item = GLB_GetItemID("MON4_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // MONKEY 5 EFFECT ======================
    lib = &fx_items[FX_MON5];
    lib->item = GLB_GetItemID("MON5_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // MONKEY 6 EFFECT ======================
    lib = &fx_items[FX_MON6];
    lib->item = GLB_GetItemID("MON6_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // DAVE =================================
    lib = &fx_items[FX_DAVE];
    lib->item = GLB_GetItemID("DAVE_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // THEME SONG ======================
    lib = &fx_items[FX_THEME];
    lib->item = GLB_GetItemID("THEME_FX");
    lib->pri = 0;
    lib->pitch = 128;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 1;

    // AIR EXPLOSION ======================
    lib = &fx_items[FX_AIREXPLO];
    lib->item = GLB_GetItemID("EXPLO_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // AIR EXPLOSION 2 ( BOSS ) ============
    lib = &fx_items[FX_AIREXPLO2];
    lib->item = GLB_GetItemID("EXPLO2_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // PICK UP BONUS ======================
    lib = &fx_items[FX_BONUS];
    lib->item = GLB_GetItemID("BONUS_FX");
    lib->pri = 0;
    lib->pitch = 128;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // SHIP LOSES SOMTHING CRASH ==========
    lib = &fx_items[FX_CRASH];
    lib->item = GLB_GetItemID("CRASH_FX");
    lib->pri = 0;
    lib->pitch = 128;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // DOOR OPENING =======================
    lib = &fx_items[FX_DOOR];
    lib->item = GLB_GetItemID("DOOR_FX");
    lib->pri = 0;
    lib->pitch = 120;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 0;

    // FLY BY SOUND =======================
    lib = &fx_items[FX_FLYBY];
    lib->item = GLB_GetItemID("FLYBY_FX");
    lib->pri = 0;
    lib->pitch = 120;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // ENERGY GRAB ========================
    lib = &fx_items[FX_EGRAB];
    lib->item = GLB_GetItemID("EGRAB_FX");
    lib->pri = 2;
    lib->pitch = 128;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 40;
    lib->gcache = 1;
    lib->odig = 0;

    // GROUND EXPLOSION ===================
    lib = &fx_items[FX_GEXPLO];
    lib->item = GLB_GetItemID("GEXPLO_FX");
    lib->pri = 2;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // NORM GUN ===========================
    lib = &fx_items[FX_GUN];
    lib->item = GLB_GetItemID("GUN_FX");
    lib->pri = 10;
    lib->pitch = 125;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 30;
    lib->gcache = 1;
    lib->odig = 0;

    // JET SOUND ==========================
    lib = &fx_items[FX_JETSND];
    lib->item = GLB_GetItemID("JETSND_FX");
    lib->pri = 4;
    lib->pitch = 120;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 120;
    lib->gcache = 0;
    lib->odig = 0;

    // ====================================
    lib = &fx_items[FX_LASER];
    lib->item = GLB_GetItemID("LASER_FX");
    lib->pri = 2;
    lib->pitch = 120;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 50;
    lib->gcache = 1;
    lib->odig = 0;

    // ====================================
    lib = &fx_items[FX_MISSLE];
    lib->item = GLB_GetItemID("MISSLE_FX");
    lib->pri = 3;
    lib->pitch = 120;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 50;
    lib->gcache = 1;
    lib->odig = 0;

    // ====================================
    lib = &fx_items[FX_SWEP];
    lib->item = GLB_GetItemID("SWEP_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // ====================================
    lib = &fx_items[FX_TURRET];
    lib->item = GLB_GetItemID("TURRET_FX");
    lib->pri = 1;
    lib->pitch = 128;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 60;
    lib->gcache = 1;
    lib->odig = 0;

    // ====================================
    lib = &fx_items[FX_WARNING];
    lib->item = GLB_GetItemID("WARN_FX");
    lib->pri = 2;
    lib->pitch = 128;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 100;
    lib->gcache = 1;
    lib->odig = 0;

    // ====================================
    lib = &fx_items[FX_BOSS1];
    lib->item = GLB_GetItemID("BOSS_FX");
    lib->pri = 1;
    lib->pitch = 127;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 1;

    // INSIDE JET SOUND ==========================
    lib = &fx_items[FX_IJETSND];
    lib->item = GLB_GetItemID("JETSND_FX");
    lib->pri = 1;
    lib->pitch = 235;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 45;
    lib->gcache = 0;
    lib->odig = 0;

    // ENEMY JET SOUND ==========================
    lib = &fx_items[FX_EJETSND];
    lib->item = GLB_GetItemID("JETSND_FX");
    lib->pri = 1;
    lib->pitch = 65;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 120;
    lib->gcache = 0;
    lib->odig = 0;

    // INTRO E HIT ===========================
    lib = &fx_items[FX_INTROHIT];
    lib->item = GLB_GetItemID("GUN_FX");
    lib->pri = 1;
    lib->pitch = 215;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 0;

    // INTRO GUN ===========================
    lib = &fx_items[FX_INTROGUN];
    lib->item = GLB_GetItemID("GUN_FX");
    lib->pri = 1;
    lib->pitch = 110;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 0;
    lib->odig = 0;

    // ENEMY SHOT ==========================
    lib = &fx_items[FX_ENEMYSHOT];
    lib->item = GLB_GetItemID("ESHOT_FX");
    lib->pri = 1;
    lib->pitch = 100;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 50;
    lib->gcache = 1;
    lib->odig = 0;

    // ENEMY LASER ==========================
    lib = &fx_items[FX_ENEMYLASER];
    lib->item = GLB_GetItemID("LASER_FX");
    lib->pri = 1;
    lib->pitch = 70;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 120;
    lib->gcache = 1;
    lib->odig = 0;

    // ENEMY MISSLE =========================
    lib = &fx_items[FX_ENEMYMISSLE];
    lib->item = GLB_GetItemID("MISSLE_FX");
    lib->pri = 2;
    lib->pitch = 140;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 55;
    lib->gcache = 1;
    lib->odig = 0;

    // ENEMY SHOT ==========================
    lib = &fx_items[FX_ENEMYPLASMA];
    lib->item = GLB_GetItemID("ESHOT_FX");
    lib->pri = 2;
    lib->pitch = 127;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // SHIELD HIT =========================
    lib = &fx_items[FX_SHIT];
    lib->item = GLB_GetItemID("HIT_FX");
    lib->pri = 1;
    lib->pitch = 132;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 127;
    lib->gcache = 1;
    lib->odig = 0;

    // SHIP HIT =========================
    lib = &fx_items[FX_HIT];
    lib->item = GLB_GetItemID("GUN_FX");
    lib->pri = 1;
    lib->pitch = 214;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 100;
    lib->gcache = 1;
    lib->odig = 0;

    // NO_SHOOT ==========================
    lib = &fx_items[FX_NOSHOOT];
    lib->item = GLB_GetItemID("ESHOT_FX");
    lib->pri = 2;
    lib->pitch = 254;
    lib->rpflag = 0;
    lib->sid = -1;
    lib->vol = 40;
    lib->gcache = 1;
    lib->odig = 0;

    // PULSE CANNON ======================
    lib = &fx_items[FX_PULSE];
    lib->item = GLB_GetItemID("ESHOT_FX");
    lib->pri = 2;
    lib->pitch = 100;
    lib->rpflag = 1;
    lib->sid = -1;
    lib->vol = 50;
    lib->gcache = 1;
    lib->odig = 1;

    for (loop = 0; loop < FX_LAST_SND; loop++)
    {
        lib = &fx_items[loop];
        lib->sid = -1;
        
        if ((unsigned int)lib->item > 0)
        {
            lib->item += fx_device;
            GLB_CacheItem(lib->item);
        }
        else
            lib->item = -1;
    }
}

/***************************************************************************
SND_FreeFX () - Frees up Fx's
 ***************************************************************************/
void 
SND_FreeFX(
    void
)
{
    int loop;
    DFX *lib;
    
    SND_StopPatches();
    
    for (loop = 0; loop < FX_LAST_SND; loop++)
    {
        lib = &fx_items[loop];
        
        if ((unsigned int)lib->item > 0)
            GLB_FreeItem(lib->item);
    }
}

/***************************************************************************
SND_CacheFX () Caches all FX's
 ***************************************************************************/
void 
SND_CacheFX(
    void
)
{
    int loop;
    DFX *lib;
    
    for (loop = 0; loop < FX_LAST_SND; loop++)
    {
        lib = &fx_items[loop];
        
        if (lib->item != -1)
            GLB_CacheItem(lib->item);
    }
}

/***************************************************************************
SND_CacheGFX () Caches in Game FX's
 ***************************************************************************/
void 
SND_CacheGFX(
    void
)
{
    int loop;
    DFX *lib;
    
    SND_StopPatches();
    
    for (loop = 0; loop < FX_LAST_SND; loop++)
    {
        lib = &fx_items[loop];
        
        if (lib->item != -1 && lib->gcache != 0)
            GLB_CacheItem(lib->item);
    }
}

/***************************************************************************
SND_CacheIFX () Caches intro and menu FX
 ***************************************************************************/
void 
SND_CacheIFX(
    void
)
{
    int loop;
    DFX *lib;
    
    SND_StopPatches();
    
    for (loop = 0; loop < FX_LAST_SND; loop++)
    {
        lib = &fx_items[loop];
        
        if (lib->item != -1 && lib->gcache == 0)
            GLB_CacheItem(lib->item);
    }
}

/***************************************************************************
SFX_Playing () -
 ***************************************************************************/
int 
SFX_Playing(
    int handle
)
{
    switch (handle & FXHAND_TMASK)
    {
    case FXHAND_GSS1:
        return GSS_PatchIsPlaying(handle);
    case FXHAND_DSP:
        return DSP_PatchIsPlaying(handle);
    }
    
    return 0;
}

/***************************************************************************
SFX_PlayPatch () -
 ***************************************************************************/
int 
SFX_PlayPatch(
    char* patch, 
    int pitch, 
    int sep, 
    int vol, 
    int priority
)
{
    int type = *(int16_t*)patch;
    type = LE_LONG(type);
    
    switch (type)
    {
    case 0:
        break;
    
    case 1:
    case 2:
        return GSS_PlayPatch(patch, sep, pitch, vol, priority);
    case 3:
        return DSP_StartPatch((dsp_t*)patch, sep, pitch, vol, priority);
    }
    
    return -1;
}

/***************************************************************************
SFX_StopPatch () -
 ***************************************************************************/
void 
SFX_StopPatch(
    int handle
)
{
    switch (handle & FXHAND_TMASK)
    {
    case FXHAND_GSS1:
        GSS_StopPatch(handle);
        break;
    
    case FXHAND_DSP:
        DSP_StopPatch(handle);
        break;
    }
}

/***************************************************************************
SND_Patch () - Test patch to see if it will be played by SND_Play
 ***************************************************************************/
void 
SND_Patch(
    int type,              // INPUT : DFX type patch to play
    int xpos               // INPUT : 127=center
)
{
    char *patch;
    int rnd, numsnds, volume;
    DFX *curfld;
    int loop;
    
    if (fx_volume < 1)
        return;
    
    rnd = 0;
    numsnds = 0;
    curfld = fx_items;
    
    for (loop = 0; loop < FX_LAST_SND; loop++, curfld++)
    {
        if (curfld->sid != -1)
        {
            if (!SFX_Playing(curfld->sid) && loop != type)
                SND_StopPatch(loop);
            else
                numsnds++;
        }
    }
    
    if (numsnds <= fx_channels + 2)
    {
        curfld = &fx_items[type];
        
        if ((!curfld->odig || dig_flag) && curfld->item != -1)
        {
            if (curfld->rpflag)
            {
                rnd = wrand() % 40;
                rnd -= 20;
            }
            
            patch = GLB_LockItem(curfld->item);
            
            volume = (curfld->vol * fx_volume) / 127;
            
            curfld->sid = SFX_PlayPatch(patch, curfld->pitch + rnd, xpos, volume, curfld->pri);
        }
    }
}

/***************************************************************************
SND_3DPatch () - playes a patch in 3d for player during game play
 ***************************************************************************/
void 
SND_3DPatch(
    int type,              // INPUT : DFX type patch to play
    int x,                 // INPUT : x sprite center
    int y                  // INPUT : y sprite center
)
{
    int rnd;
    int numsnds, xpos;
    int loop;
    int dx, dy, dist, volume, vol, getdxdy;
    char *patch;
    DFX *curfld;
    
    if (fx_volume < 1)
        return;

    rnd = 0;
    numsnds = 0;
    curfld = fx_items;
    
    for (loop = 0; loop < FX_LAST_SND; loop++, curfld++)
    {
        if (curfld->sid != -1)
        {
            if (!SFX_Playing(curfld->sid))
                SND_StopPatch(loop);
            else
                numsnds++;
        }
    }
    
    if (numsnds <= fx_channels + 2)
    {
        dx = x - player_cx;
        dy = y - player_cy;
        
        xpos = dx + 127;
        
        if (xpos < 1)
            xpos = 1;
        else if (xpos > 255)
            xpos = 255;
        
        dx = abs(dx);
        dy = abs(dy);
        
        if (dx < dy)
            getdxdy = dx;
        else
            getdxdy = dy;
        
        dist = dx + dy - (getdxdy / 2);
        
        if (dist < SND_CLOSE)
            vol = 127;
        else if (dist > SND_FAR)
            vol = 1;
        else
            vol = 127 - ((dist - SND_CLOSE) * 127) / (SND_FAR - SND_CLOSE);
        
        curfld = &fx_items[type];
        
        if (!curfld->odig || dig_flag)
        {
            if (curfld->rpflag)
            {
                rnd = wrand() % 40;
                rnd -= 20;
            }
            
            volume = (vol * fx_volume) / 127;
            volume = (volume * curfld->vol) / 127;
            
            patch = GLB_LockItem(curfld->item);
            curfld->sid = SFX_PlayPatch(patch, curfld->pitch + rnd, xpos, volume, curfld->pri);
        }
    }
}

/***************************************************************************
SND_IsPatchPlaying() - Returns TRUE if patch is playing
 ***************************************************************************/
int 
SND_IsPatchPlaying(
    int type                 // INPUT : position in fxitems
)
{
    DFX *curfld;
    
    curfld = &fx_items[type];
    
    if (curfld->sid != -1 && SFX_Playing(curfld->sid))
        return 1;
    
    return 0;
}

/***************************************************************************
SND_StopPatch () - Stops Type patch
 ***************************************************************************/
void 
SND_StopPatch(
    int type               // INPUT : DFX type patch to play
)
{
    DFX *curfld;
    
    curfld = &fx_items[type];
    
    if (curfld->sid != -1)
    {
        SFX_StopPatch(curfld->sid);
        GLB_UnlockItem(curfld->item);
        curfld->sid = -1;
    }
}

/***************************************************************************
SND_StopPatches () - Stops all currently playing patches
 ***************************************************************************/
void 
SND_StopPatches(
    void
)
{
    int loop;
    DFX *curfld;
    
    curfld = fx_items;
    
    for (loop = 0; loop < FX_LAST_SND; loop++, curfld++)
    {
        if (curfld->sid != -1)
            SFX_StopPatch(curfld->sid);
    }
    
    curfld = fx_items;
    
    for (loop = 0; loop < FX_LAST_SND; loop++, curfld++)
    {
        if (curfld->sid != -1)
        {
            GLB_UnlockItem(curfld->item);
            curfld->sid = -1;
        }
    }
}

/***************************************************************************
SND_PlaySong() - Plays song associated with song id
 ***************************************************************************/
void 
SND_PlaySong(
    int item,             // INPUT : Song GLB item
    int chainflag,        // INPUT : Chain Song to ItSelf
    int fadeflag          // INPUT : Fade Song Out
)
{
    char *song;
    
    if (music_volume <= 1)
        return;
    
    if (music_song == item)
        return;
    
    if (music_song != -1)
    {
        MUS_StopSong(fadeflag);
        
        if (fadeflag)
        {
            while (MUS_SongPlaying())
            {
                I_GetEvent();
            }
        }
        
        GLB_UnlockItem(music_song);
        music_song = -1;
    }
    
    if (item != -1)
    {
        music_song = item;
        song = GLB_LockItem(item);
        MUS_PlaySong(song, chainflag, fadeflag);
    }
}

/***************************************************************************
SND_IsSongPlaying () - Is current song playing
 ***************************************************************************/
int 
SND_IsSongPlaying(
    void
) 
{
    return MUS_SongPlaying();
}

/***************************************************************************
SND_FadeOutSong () - Fades current song out and stops playing music
 ***************************************************************************/
void 
SND_FadeOutSong(
    void
)
{
    if (music_song != -1)
    {
        if (MUS_SongPlaying())
            MUS_StopSong(1);
        
        while (MUS_SongPlaying())
        {
            I_GetEvent();
        }
        
        GLB_UnlockItem(music_song);
    }
    
    music_song = -1;
}

/***************************************************************************
SND_Lock () -
 ***************************************************************************/
void 
SND_Lock(
    void
)
{
    if (!lockcount)
        SDL_LockAudioDevice(fx_dev);
    
    lockcount++;
}

/***************************************************************************
SND_Unlock () -
 ***************************************************************************/
void 
SND_Unlock(
    void
)
{
    lockcount--;
    
    if (!lockcount)
        SDL_UnlockAudioDevice(fx_dev);
}
