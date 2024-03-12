#pragma once

typedef enum
{
    M_ANIM,
    M_PIC,
    M_SEE_THRU
}FRAMETYPE;

typedef enum
{
    M_NORM,
    M_FADEIN,
    M_FADEOUT,
    M_PALETTE,
    M_ERASE
}FRAMEOPT;

typedef enum
{
    S_PLAY,
    S_FADEIN,
    S_FADEOUT,
    S_STOP
}SONGOPTS;

typedef struct 
{
    unsigned short opt;
    unsigned short fill;
    unsigned short offset;
    unsigned short length;
}ANIMLINE;

typedef struct
{
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
}FRAME;

void ANIM_Render(ANIMLINE *inmem);
void MOVIE_BPatch(int soundfx);
int MOVIE_Play(FRAME *frame, int numplay, char *palette);
