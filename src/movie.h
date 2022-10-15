#pragma once

enum FRAMETYPE
{
    M_ANIM,
    M_PIC,
    M_SEE_THRU
};

enum FRAMEOPT
{
    M_NORM,
    M_FADEIN,
    M_FADEOUT,
    M_PALETTE,
    M_ERASE
};

enum SONGOPTS
{
    S_PLAY,
    S_FADEIN,
    S_FADEOUT,
    S_STOP
};

struct movanim_t {
    unsigned short f_0;
    unsigned short f_2;
    unsigned short f_4;
    unsigned short f_6;
};

struct movie_t {
    int opt;                   // TYPE OF DRAWING REQUIRED
    int framerate;             // FRAME RATE OF UPDATE
    int numframes;             // NUMBER OF FRAMES LEFT
    int item;                  // ITEM # OF PICTURE
    int startf;                // START FRAME OPTS
    int startsteps;            // # OF STEPS IF FADEIN
    int endf;                  // END FRAME OPTS
    int endsteps;              // # OF STEPS IN FADEOUT
    int red;                   // RED VAL FOR FADEOUT
    int green;                 // GREEN VAL FOR FADEOUT
    int blue;                  // BLUE VAL FOR FADEOUT
    int holdframe;             // NUMBER OF TICS TO HOLD FRAME
    int songid;                // SONG ID TO PLAY
    int songopt;               // SONG OPTS
    int songstep;              // SONG STEPS FOR FADES
    int soundfx;               // SOUND FX START
    int fx_vol;                // SOUND FX VOLUME
    int fx_xpos;               // SOUND FX XPOS
};

void ANIM_Render(movanim_t *inmem);
void MOVIE_BPatch(int soundfx);
int MOVIE_Play(movie_t *frame, int numplay, char *palette);
