#pragma once

#define G3D_DIST 200

#define GFX_DARK  0
#define GFX_LITE  1

#define SCREENWIDTH  320
#define SCREENHEIGHT 200

enum CORNER
{
    UPPER_LEFT,
    UPPER_RIGHT,
    LOWER_LEFT,
    LOWER_RIGHT
};

enum SHADE
{
    DARK,
    LIGHT,
    GREY
};

enum GFX_TYPE
{
    GSPRITE,
    GPIC
};

#define CLIP_XRIGHT    2
#define CLIP_XLEFT     4
#define CLIP_YTOP      8
#define CLIP_YBOTTOM   16

struct texture_t {

    int x;                   // x
    int y;                   // y
    int offset;              // marker
    int width;               // width
    int height;              // height
    char charofs[1];         // data
};

struct font_t {

    int height;             // height
    short offset[256];      // offset
    char width[256];        // width
    char charofs[1];        // data
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
void GFX_InitVideo(char *curpal);
int GFX_GetFrameCount(void);
int GFX_ClipLines(char **image, int *x, int *y, int *lx, int *ly);
void GFX_SetPalette(char *curpal, int startpal);
void GFX_FadeOut(int red, int green, int blue, int speed);
void GFX_FadeIn(char *palette, int steps);
void GFX_FadeStart(void);
void GFX_FadeFrame(char *palette, int cur_step, int steps);
void GFX_SetPalRange(int start, int end);
void GFX_MakeLightTable(char *palette, char *ltable, int level);
void GFX_MakeGreyTable(char *palette, char *ltable);
void GFX_GetScreen(char *outmem, int x, int y, int lx, int ly);
void GFX_PutTexture(texture_t *intxt, int x, int y, int lx, int ly);
void GFX_ShadeArea(int opt, int x, int y, int lx, int ly);
void GFX_ShadeShape(int opt, texture_t* mask, int x, int y);
void GFX_VShadeLine(int opt, int x, int y, int ly);
void GFX_HShadeLine(int opt, int x, int y, int lx);
void GFX_LightBox(int opt, int x, int y, int lx, int ly);
void GFX_ColorBox(int x, int y, int lx, int ly, int color);
void GFX_HLine(int x, int y, int lx, int color);
void GFX_VLine(int x, int y, int ly, int color);
void GFX_Line(int x, int y, int x2, int y2, int color);
void GFX_ScalePic(texture_t *buffin, int x, int y, int new_lx, int new_ly, int see_thru);
void GFX_DisplayUpdate(void);
void GFX_PutImage(texture_t *image, int x, int y, int see_thru);
void GFX_PutSprite(texture_t *inmem, int x, int y);
void GFX_MarkUpdate(int x, int y, int lx, int ly);
void GFX_ForceUpdate(int x, int y, int lx, int ly);
void GFX_SetFrameHook(void (*func)(void (*)(void)));
void GFX_WaitUpdate(int count);
void GFX_DisplayScreen(void);
void GFX_DrawSprite(char *dest, texture_t *inmem);
void GFX_DrawChar(char *dest, char *inmem, int width, int height, int addx, int color);
void GFX_Shade(char *outmem, int maxlen, char *dtable);
void GFX_PutPic(void);
void GFX_PutMaskPic(void);
int GFX_StrPixelLen(font_t *infont, char *instr, int maxloop);
int GFX_Print(int printx, int printy, char *str, font_t* infont, int basecolor);
void GFX_ShadeSprite(char *dest, texture_t *inmem, char *dtable);
void GFX_ScaleLine(char *outmem, char *inmem);
void GFX_CScaleLine(char *outmem, char *inmem);
void GFX_3D_SetView(int x, int y, int z);
void GFX_3DPoint(void);

