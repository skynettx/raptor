#pragma once

enum {
    FXHAND_PCS = 0x0000,
    FXHAND_GSS1 = 0x4000,
    // FXHAND_GSS2 = 0x8000,
    FXHAND_DSP = 0xc000,
    FXHAND_MASK = 0x3fff,
    FXHAND_TMASK = 0xc000,
};

extern int music_volume;
extern int fx_volume;
extern int dig_flag;
extern int fx_freq;
extern int fx_gus;
extern int sys_midi, alsaclient, alsaport;

int SND_InitSound(void);
void SND_DeInit(void);
void SND_Setup(void);
void SND_PlaySong(int a1, int a2, int a3);
int SND_SongPlaying(void);
void SND_Lock(void);
void SND_Unlock(void);
void SND_FadeOutSong(void);
void SND_FreeFX(void);
void SND_CacheFX(void);
void SND_CacheGFX(void);
void SND_CacheIFX(void);
void SND_3DPatch(int, int, int);
void SND_Patch(int a1, int a2);
int SND_IsPatchPlaying(int a1);
void SND_StopPatch(int a1);
void SND_StopPatches(void);
