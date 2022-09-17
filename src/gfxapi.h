#pragma once

struct texture_t {

    int f_0; // x
    int f_4; // y
    int f_8; // marker
    int width; // width
    int height; // height
    char f_14[1];
};

struct font_t {

    int f_0; // height
    short f_4[256]; // offset
    char f_204[256]; // width
    char f_304[1]; // data
};

extern char *displaybuffer;
extern char *displayscreen;
extern int ud_x, ud_y, ud_lx, ud_ly;
extern int o_ud_x, o_ud_y, o_ud_lx, o_ud_ly;
extern int update_start;
extern char *gfx_inmem;
extern int gfx_xp, gfx_yp;
extern int gfx_lx, gfx_ly;
extern int gfx_imga;
extern int framecount;
extern int tablelen;
extern int stable[324];
extern char *ltable, *dtable, *gtable;
extern int ylookup[200];
extern int retraceflag;
extern int G3D_viewx, G3D_viewy, G3D_viewz;
extern int G3D_x, G3D_y, G3D_z, G3D_screenx, G3D_screeny;

void GFX_UpdateTimer(void);

void GFX_InitSystem(void);
void GFX_InitVideo(char *pal);
int GFX_GetFrameCount(void);
int GFX_ClipLines(char **a1, int *a2, int *a3, int *a4, int *a5);
void GFX_SetPalette(char *a1, int a2);
void GFX_FadeOut(int a1, int a2, int a3, int a4);
void GFX_FadeIn(char *a1, int a2);
void GFX_FadeStart(void);
void GFX_FadeFrame(char *a1, int a2, int a3);
void GFX_SetPalRange(int a1, int a2);
void GFX_MakeLightTable(char *a1, char *a2, int a3);
void GFX_MakeGreyTable(char *a1, char *a2);
void GFX_GetScreen(char *a1, int a2, int a3, int a4, int a5);
void GFX_PutTexture(texture_t *a1, int a2, int a3, int a4, int a5);
void GFX_ShadeArea(int a1, int a2, int a3, int a4, int a5);
void GFX_ShadeShape(int a1, texture_t* a2, int a3, int a4);
void GFX_VShadeLine(int a1, int a2, int a3, int a4);
void GFX_HShadeLine(int a1, int a2, int a3, int a4);
void GFX_LightBox(int a1, int a2, int a3, int a4, int a5);
void GFX_ColorBox(int a1, int a2, int a3, int a4, int a5);
void GFX_HLine(int a1, int a2, int a3, int a4);
void GFX_VLine(int a1, int a2, int a3, int a4);
void GFX_Line(int a1, int a2, int a3, int a4, int a5);
void GFX_ScalePic(texture_t *a1, int a2, int a3, int a4, int a5, int a6);
void GFX_DisplayUpdate(void);
void GFX_PutImage(texture_t *a1, int a2, int a3, int a4);
void GFX_PutSprite(texture_t *a1, int a2, int a3);
void GFX_MarkUpdate(int a1, int a2, int a3, int a4);
void GFX_ForceUpdate(int a1, int a2, int a3, int a4);
void GFX_SetFrameHook(void (*a1)(void (*)(void)));
void GFX_WaitUpdate(int a1);
void GFX_DisplayScreen(void);
void GFX_DrawSprite(char *a1, texture_t *a2);
void GFX_DrawChar(char *a1, char *a2, int a3, int a4, int a5, int a6);
void GFX_Shade(char *a1, int a2, char *a3);
void GFX_PutPic(void);
void GFX_PutMaskPic(void);
int GFX_StrPixelLen(font_t *a1, char *a2, int a3);
int GFX_Print(int a1, int a2, char *a3, font_t* a4, int a5);
void GFX_ShadeSprite(char *p, texture_t *t, char *s);
void GFX_ScaleLine(char *a1, char *a2);
void GFX_CScaleLine(char *a1, char *a2);
void GFX_3D_SetView(int a1, int a2, int a3);
void GFX_3DPoint(void);

