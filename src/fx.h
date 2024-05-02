#pragma once

#define SND_CLOSE    40
#define SND_FAR      500

enum 
{
    FXHAND_PCS = 0x0000,
    FXHAND_GSS1 = 0x4000,
    // FXHAND_GSS2 = 0x8000,
    FXHAND_DSP = 0xc000,
    FXHAND_MASK = 0x3fff,
    FXHAND_TMASK = 0xc000,
};

typedef enum
{
    SND_NONE,
    SND_PC,
    SND_MIDI,
    SND_CANVAS,
    SND_DIGITAL
}SND_TYPE;

typedef enum
{
    FX_THEME,
    FX_MON1,
    FX_MON2,
    FX_MON3,
    FX_MON4,
    FX_MON5,
    FX_MON6,
    FX_DAVE,
    FX_AIREXPLO,
    FX_AIREXPLO2,
    FX_BONUS,
    FX_CRASH,
    FX_DOOR,
    FX_FLYBY,
    FX_EGRAB,
    FX_GEXPLO,
    FX_GUN,
    FX_JETSND,
    FX_LASER,
    FX_MISSLE,
    FX_SWEP,
    FX_TURRET,
    FX_WARNING,
    FX_BOSS1,
    FX_IJETSND,
    FX_EJETSND,
    FX_INTROHIT,
    FX_INTROGUN,
    FX_ENEMYSHOT,
    FX_ENEMYLASER,
    FX_ENEMYMISSLE,
    FX_ENEMYPLASMA,
    FX_SHIT,
    FX_HIT,
    FX_NOSHOOT,
    FX_PULSE,
    FX_LAST_SND
}DEFX;

extern int music_volume;
extern int fx_volume;
extern int dig_flag;
extern int fx_freq;
extern int fx_gus;
extern int sys_midi, winmm_mpu_device, core_dls_synth, core_midi_port, alsaclient, alsaport;

int SND_InitSound(void);
void SND_DeInit(void);
void SND_Setup(void);
void SND_PlaySong(int item, int chainflag, int fadeflag);
int SND_IsSongPlaying(void);
void SND_Lock(void);
void SND_Unlock(void);
void SND_FadeOutSong(void);
void SND_FreeFX(void);
void SND_CacheFX(void);
void SND_CacheGFX(void);
void SND_CacheIFX(void);
void SND_3DPatch(int type, int x, int y);
void SND_Patch(int type, int xpos);
int SND_IsPatchPlaying(int type);
void SND_StopPatch(int type);
void SND_StopPatches(void);
