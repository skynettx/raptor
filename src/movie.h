#pragma once

struct movanim_t {
    unsigned short f_0;
    unsigned short f_2;
    unsigned short f_4;
    unsigned short f_6;
};

struct movie_t {
    int f_0;
    int f_4;
    int f_8;
    int f_c;
    int f_10;
    int f_14;
    int f_18;
    int f_1c;
    int f_20;
    int f_24;
    int f_28;
    int f_2c;
    int f_30;
    int f_34;
    int f_38;
    int f_3c;
    int f_40;
    int f_44;
};

void ANIM_Render(movanim_t *a1);
void MOVIE_BPatch(int a1);
int MOVIE_Play(movie_t *a1, int a2, char *a3);
