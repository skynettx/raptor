#include <string.h>
#include <stdint.h>
#include "SDL.h"
#include "common.h"
#include "gfxapi.h"
#include "i_video.h"

char *displaybuffer;
char *displayscreen;

int framecount;

int ylookup[200];

char *ltable, *dtable, *gtable;
char tpal1[768], tpal2[768];
int start_lookup = 0;
int end_lookup = 255;
char *gfx_inmem;
int gfx_xp, gfx_yp;
int gfx_lx, gfx_ly;
int gfx_imga;

int tablelen;
int stable[324];

int ud_x, ud_y, ud_lx, ud_ly;
int o_ud_x, o_ud_y, o_ud_lx, o_ud_ly;
int update_start;

void (*framehook)(void (*call)(void));

static int timer_init = 0;

int retraceflag = 1;
int fontspacing = 1;

int G3D_x;                        // input: x position
int G3D_y;                        // input: y position
int G3D_z;                        // input: z position
int G3D_screenx;                  // output: screen x pos
int G3D_screeny;                  // output: screen y pos
int G3D_viewx;                    // user view x pos
int G3D_viewy;                    // user view y pos
int G3D_viewz;                    // user view z pos

void GFX_InitTimer(void)
{
#if SDL_VERSION_ATLEAST(2, 0, 5)
    SDL_SetHint(SDL_HINT_WINDOWS_DISABLE_THREAD_NAMING, "1");
#endif
    SDL_Init(SDL_INIT_TIMER);
    timer_init = 1;
}

#define GFX_RATE 70

/***************************************************************************
GFX_UpdateTimer () - 
 ***************************************************************************/
void 
GFX_UpdateTimer(
    void
)
{
    if (!timer_init)
        return;
    
    static int last;
    static int accm;
    int now = SDL_GetTicks();
    
    if (last == 0)
        last = now;
    
    for (int i = last; i < now;)
    {
        int step = (1000 - accm + (GFX_RATE - 1)) / GFX_RATE;
        
        if (step < 1)
            step = 1;
        
        if (step > now - i)
            step = now - i;
        
        accm += GFX_RATE * step;
        
        while (accm >= 1000)
        {
            accm -= 1000;
            framecount++;
        }
        
        i += step;
    }
    
    last = now;
}

/***************************************************************************
GFX_GetFrameCount () -
 ***************************************************************************/
int 
GFX_GetFrameCount(
    void
)
{
    I_GetEvent();
    GFX_UpdateTimer();
    
    return framecount;
}

/*************************************************************************
   GFX_ClipLines ()
 *************************************************************************/
int                        // RETURN: 0 = Off, 1 == No Clip , 2 == Cliped
GFX_ClipLines(
    char **image,          // INOUT : pointer to image or NUL
    int *x,                // INOUT : pointer to x pos
    int *y,                // INOUT : pointer to y pos
    int *lx,               // INOUT : pointer to width
    int *ly                // INOUT : pointer to length
)
{
    int rval = 1;
    
    if (*x >= SCREENWIDTH)
        return 0;
    if (*y >= SCREENHEIGHT)
        return 0;
    if (*x + *lx <= 0)
        return 0;
    if (*y + *ly <= 0)
        return 0;
    
    if (*y < 0)
    {
        rval = 2;
        if (image)
            *image += -(*y) * *lx;
        *ly += *y;
        *y = 0;
    }
    
    if (*y + *ly > SCREENHEIGHT)
    {
        rval = 2;
        *ly = SCREENHEIGHT - *y;
    }
    
    if (*x < 0)
    {
        rval = 2;
        if (image)
            *image += -(*x);
        *lx += *x;
        *x = 0;
    }
    
    if (*x + *lx > SCREENWIDTH)
    {
        rval = 2;
        *lx = SCREENWIDTH - *x;
    }
    
    return rval;
}

/**************************************************************************
GFX_SetPalette() - Sets VGA palette
 **************************************************************************/
void 
GFX_SetPalette(
    char *palette, 
    int start_pal
)
{
#if 0
    if (retraceflag)
    {
        I_GetEvent();
        GFX_UpdateTimer();
        int now = framecount;
        while (now == framecount)
            GFX_UpdateTimer();
    }
#endif
    
    I_SetPalette((uint8_t*)palette, start_pal);
}

/**************************************************************************
  GFX_InitSystem() - allocates buffers, makes tables, does not set vmode
 **************************************************************************/
void 
GFX_InitSystem(
    void
)
{
    displaybuffer = (char*)malloc(64000);
    
    if (!displaybuffer)
    {
        EXIT_Error("GFX_Init() - malloc");
        return;
    }
    
    memset(displaybuffer, 0, 64000);
    
    GFX_InitTimer();
    
    for (int loop = 0; loop < SCREENHEIGHT; loop++)
        ylookup[loop] = loop * SCREENWIDTH;
    
    ltable = (char*)malloc(256);
    dtable = (char*)malloc(256);
    gtable = (char*)malloc(256);
    
    if (!ltable || !dtable || !gtable)
    {
        EXIT_Error("GFX_Init() - malloc");
        return;
    }
}

/**************************************************************************
GFX_InitVideo() - Inits things related to Video, and sets up fade tables
 **************************************************************************/
void 
GFX_InitVideo(
    char *curpal
)
{
    I_InitGraphics((uint8_t*)curpal);
    displayscreen = (char*)I_VideoBuffer;
    
    GFX_MakeLightTable(curpal, ltable, 9);
    GFX_MakeLightTable(curpal, dtable, -9);
    GFX_MakeGreyTable(curpal, gtable);
}

/**************************************************************************
  GFX_EndSystem() - Frees up all resources used by GFX system
 **************************************************************************/
void 
GFX_EndSystem(
    void
)
{
    I_ShutdownGraphics();
}

/**************************************************************************
  GFX_GetPalette() - Sets 256 color palette
 **************************************************************************/
void 
GFX_GetPalette(
    char* curpal           // OUTPUT : pointer to palette data
)
{
    I_GetPalette((uint8_t*)curpal);
}

/**************************************************************************
 GFX_FadeOut () - Fade Palette out to ( Red, Green , and Blue Value
 **************************************************************************/
void 
GFX_FadeOut(
    int red,               // INPUT : red ( 0 - 63 )
    int green,             // INPUT : green ( 0 - 63 )
    int blue,              // INPUT : blue ( 0 - 63 )
    int steps              // INPUT : steps of fade ( 0 - 255 )
)
{
    char pal1[768];
    char pal2[768];
    int loop, i;
    
    GFX_GetPalette(pal1);
    memcpy(pal2, pal1, 768);
    
    int now = GFX_GetFrameCount();
    loop = 0;
    
    while (GFX_GetFrameCount() - now < steps)
    {
        if (GFX_GetFrameCount() - now != loop)
        {
            loop = GFX_GetFrameCount() - now;
            
            for (i = 0; i < 256; i++)
            {
                pal2[i * 3 + 0] = loop * (red - pal1[i * 3 + 0]) / steps + pal1[i * 3 + 0];
                pal2[i * 3 + 1] = loop * (green - pal1[i * 3 + 1]) / steps + pal1[i * 3 + 1];
                pal2[i * 3 + 2] = loop * (blue - pal1[i * 3 + 2]) / steps + pal1[i * 3 + 2];
            }
            
            GFX_SetPalette(pal2, 0);
            I_FinishUpdate();
        }
    }
    
    for (loop = 0; loop < 256; loop++)
    {
        pal2[loop * 3 + 0] = red;
        pal2[loop * 3 + 1] = green;
        pal2[loop * 3 + 2] = blue;
    }

    GFX_SetPalette(pal2, 0);
    I_FinishUpdate();
}

/**************************************************************************
 GFX_FadeIn () - Fades From current palette to new palette
 **************************************************************************/
void 
GFX_FadeIn(
    char *palette,          // INPUT : palette to fade into
    int steps               // INPUT : steps of fade ( 0 - 255 )
)
{
    char pal1[768];
    char pal2[768];
    int loop, i;
    
    GFX_GetPalette(pal1);
    memcpy(pal2, pal1, 768);
    
    int now = GFX_GetFrameCount();
    loop = 0;
    
    while (GFX_GetFrameCount() - now < steps)
    {
        if (GFX_GetFrameCount() - now != loop)
        {
            loop = GFX_GetFrameCount() - now;
            
            for (i = 0; i < 768; i++)
            {
                pal2[i] = loop * (palette[i] - pal1[i]) / steps + pal1[i];
            }
            
            GFX_SetPalette(pal2, 0);
            I_FinishUpdate();
        }
    }

    GFX_SetPalette(palette, 0);
    I_FinishUpdate();
}

/**************************************************************************
GFX_FadeStart () - Sets up fade for GFX_FadeFrame()
 **************************************************************************/
void 
GFX_FadeStart(
    void
)
{
    GFX_GetPalette(tpal1);
    memcpy(tpal2, tpal1, 768);
}

/**************************************************************************
GFX_FadeFrame () - Fades Individual Frames
 **************************************************************************/
void 
GFX_FadeFrame(
    char* palette,      // INPUT : palette to fade into    
    int cur_step,       // INPUT : cur step position
    int steps           // INPUT : total steps of fade ( 0 - 255 )
)
{
    int i;
    
    for (i = 0; i < 768; i++)
    {
        tpal2[i] = ((palette[i] - tpal1[i]) * cur_step) / steps + tpal1[i];
    }
    
    GFX_SetPalette(tpal2, 0);
}

/**************************************************************************
GFX_SetPalRange() - Sets start and end range for remaping stuff
 **************************************************************************/
void 
GFX_SetPalRange(
    int start, 
    int end
)
{
    if (start < end && end < 256 && start >= 0)
    {
        start_lookup = start;
        end_lookup = end;
    }
}

/**************************************************************************
  GFX_GetRGB() - gets R,G and B values from pallete data
 **************************************************************************/
void 
GFX_GetRGB(
    char *pal,             // INPUT : pointer to palette data
    int num,               // INPUT : palette entry
    int *red,              // OUTPUT: red value
    int *green,            // OUTPUT: green value
    int *blue              // OUTPUT: blue value
)
{
    *red = pal[num * 3 + 0];
    *green = pal[num * 3 + 1];
    *blue = pal[num * 3 + 2];
}

/**************************************************************************
  GFX_Remap() - Finds the closest color avalable
 **************************************************************************/
int                        // RETURN: new color number
GFX_Remap(
    char *pal,             // INPUT : pointer to palette data
    int red,               // INPUT : red  ( 0 - 63 )
    int green,             // INPUT : green( 0 - 63 )
    int blue               // INPUT : blue ( 0 - 63 )          
)
{
    int loop, r, g, b, num;
    int low = 769;
    int pos = 0;
    
    for (loop = start_lookup; loop < end_lookup + 1; loop++)
    {
        GFX_GetRGB(pal, loop, &r, &g, &b);
        num = abs(r - red) + abs(g - green) + abs(b - blue);
        
        if (num <= low)
        {
            low = num;
            pos = loop;
        }
    }
    
    return pos;
}

/**************************************************************************
  GFX_MakeLightTable() - make a light/dark palette lookup table
 **************************************************************************/
void 
GFX_MakeLightTable(
    char *palette,         // INPUT : pointer to palette data
    char *ltable,          // OUTPUT: pointer to lookup table  
    int level              // INPUT : - 63 to + 63 
)
{
    int loop, red, green, blue;
    
    for (loop = 0; loop < 256; loop++)
    {
        GFX_GetRGB(palette, loop, &red, &green, &blue);
        
        if (red >= 0)
            red += level;
        else
            red = 0;
        
        if (green >= 0)
            green += level;
        else
            green = 0;
        
        if (blue >= 0)
            blue += level;
        else
            blue = 0;
        
        if (level >= 0)
        {
            if (red > 63)
                red = 63;
            if (green > 63)
                green = 63;
            if (blue > 63)
                blue = 63;
        }
        else
        {
            if (red < 0)
                red = 0;
            if (green < 0)
                green = 0;
            if (blue < 0)
                blue = 0;
        }
        
        ltable[loop] = GFX_Remap(palette, red, green, blue);
    }
}

/**************************************************************************
  GFX_MakeGreyTable() - make a grey palette lookup table
 **************************************************************************/
void 
GFX_MakeGreyTable(
    char *palette,         // INPUT : pointer to palette data
    char *ltable)          // OUTPUT: pointer to lookup table        
{
    int loop, color, red, green, blue;
    
    for (loop = 0; loop < 256; loop++)
    {
        GFX_GetRGB(palette, loop, &red, &green, &blue);
        
        color = (red + green + blue) / 3;
        
        ltable[loop] = GFX_Remap(palette, color, color, color);
    }
}

/*************************************************************************
   GFX_GetScreen() -     Gets A block of screen memory to CPU memory
 *************************************************************************/
void 
GFX_GetScreen(
    char *outmem,          // OUTPUT: pointer to CPU mem 
    int x,                 // INPUT : x pos
    int y,                 // INPUT : y pos
    int lx,                // INPUT : x length     
    int ly                 // INPUT : y length
)
{
    char *source;
    int loop;
    
    if (!GFX_ClipLines(&outmem, &x, &y, &lx, &ly))
        return;
    
    source = &displaybuffer[ylookup[y] + x];
    
    for (loop = 0; loop < ly; loop++)
    {
        memcpy(outmem, source, lx);
        outmem += lx;
        source += SCREENWIDTH;
    }
}

/*************************************************************************
   GFX_PutTexture() - Repeats a Picture though the area specified
 *************************************************************************/
void 
GFX_PutTexture(
    texture_t *intxt,      // INPUT : color texture
    int x,                 // INPUT : x pos
    int y,                 // INPUT : y pos
    int lx,                // INPUT : x length
    int ly                 // INPUT : y length
)
{
    int loopy, loopx;
    int maxxloop = lx + abs(x);
    int maxyloop = ly + abs(y);
    int y2 = ly + y - 1;
    int x2 = lx + x - 1;
    int new_ly, ypos, new_lx, xpos;
    char *buf;
    
    if (x2 >= SCREENWIDTH)
        x2 = SCREENWIDTH - 1;
    if (y2 >= SCREENHEIGHT)
        y2 = SCREENHEIGHT - 1;
    
    for (loopy = y; loopy < maxyloop; loopy += intxt->height)
    {
        if (loopy <= y2 && loopy + intxt->height > 0)
        {
            if (loopy < 0)
            {
                new_ly += loopy;
                buf += (-loopy) * intxt->width;
                ypos = 0;
            }
            else
                ypos = loopy;
            
            for (loopx = x; loopx < maxxloop; loopx += intxt->width)
            {
                if (loopx <= x2 && loopx + intxt->width > 0)
                {
                    buf = intxt->charofs;
                    new_lx = intxt->width;
                    new_ly = intxt->height;
                    
                    if (loopx < 0)
                    {
                        new_lx += loopx;
                        buf += -loopx;
                        xpos = 0;
                    }
                    else
                        xpos = loopx;
                    
                    if (xpos + new_lx - 1 >= x2)
                        new_lx = (x2 + 1) - xpos;
                    
                    if (ypos + new_ly - 1 >= y2)
                        new_ly = (y2 + 1) - ypos;
                    
                    gfx_inmem = buf;
                    gfx_xp = xpos;
                    gfx_yp = ypos;
                    gfx_lx = new_lx;
                    gfx_ly = new_ly;
                    gfx_imga = intxt->width - new_lx;
                    
                    GFX_PutPic();
                    
                    GFX_MarkUpdate(xpos, ypos, new_lx, new_ly);
                }
            }
        }
    }
}

/*************************************************************************
   GFX_ShadeArea()- lightens or darkens and area of the screen
 *************************************************************************/
void 
GFX_ShadeArea(
    int opt,               // INPUT : DARK/LIGHT or GREY 
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx,                // INPUT : x length     
    int ly                 // INPUT : y length
)
{
    int loop;
    char *buf, *cur_table;
    
    if (!GFX_ClipLines(NULL, &x, &y, &lx, &ly))
        return;
    
    buf = &displaybuffer[x + ylookup[y]];
    
    switch (opt)
    {
    case DARK:
        cur_table = dtable;
        break;
    case LIGHT:
        cur_table = ltable;
        break;
    case GREY:
        cur_table = gtable;
        break;
    }
    
    GFX_MarkUpdate(x, y, lx, ly);
    
    for (loop = 0; loop < ly; loop++)
    {
        GFX_Shade(buf, lx, cur_table);
        buf += SCREENWIDTH;
    }
}

/*************************************************************************
   GFX_ShadeShape()- lightens or darkens and area of the screen
 *************************************************************************/
void 
GFX_ShadeShape(
    int opt,               // INPUT : DARK/LIGHT or GREY        
    texture_t* inmem,      // INPUT : mask 0 = no shade ( GFX format pic )
    int x,                 // INPUT : x position
    int y                  // INPUT : y position
)
{
    char rval;
    char *cur_table;
    int ox = x;
    int oy = y;
    int lx = inmem->width;
    int ly = inmem->height;
    
    texture_t *ah = (texture_t*)inmem->charofs;

    rval = GFX_ClipLines(NULL, &ox, &oy, &lx, &ly);
    if (!rval)
        return;
    
    switch (opt)
    {
    case DARK:
        cur_table = dtable;
        break;
    
    case LIGHT:
        cur_table = ltable;
        break;
    
    case GREY:
        cur_table = gtable;
        break;
    }
    
    switch (rval)
    {
    case 1:
        GFX_ShadeSprite(&displaybuffer[x + ylookup[y]], ah, cur_table);
        break;
    
    case 2:
        while (ah->offset != -1)
        {
            ox = ah->x + x;
            oy = ah->y + y;
            
            if (oy > SCREENHEIGHT)
                return;
            
            lx = ah->width;
            ly = 1;
            
            if (GFX_ClipLines(NULL, &ox, &oy, &lx, &ly))
                GFX_Shade(&displaybuffer[ox + ylookup[oy]], lx, cur_table);
            
            ah = (texture_t*)((char*)ah + 16 + ah->width);
        }
        break;
    }
}

/*************************************************************************
   GFX_VShadeLine () - Shades a vertical line
 *************************************************************************/
void 
GFX_VShadeLine(
    int opt,               // INPUT : DARK/LIGHT or GREY    
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int ly                 // INPUT : length of line
)
{
    char *cur_table;
    char *outbuf;
    int lx = 1;
    
    if (ly < 1)
        return;
    
    if (!GFX_ClipLines(NULL, &x, &y, &lx, &ly))
        return;
    
    switch (opt)
    {
    case DARK:
        cur_table = dtable;
        break;
    case LIGHT:
        cur_table = ltable;
        break;
    case GREY:
        cur_table = gtable;
        break;
    }
    
    GFX_MarkUpdate(x, y, lx, ly);
    
    outbuf = &displaybuffer[x + ylookup[y]];
    
    while (ly--)
    {
        *outbuf = cur_table[(uint8_t)*outbuf];
        outbuf += SCREENWIDTH;
    }
}

/*************************************************************************
   GFX_HShadeLine () Shades a Horizontal Line
 *************************************************************************/
void 
GFX_HShadeLine(
    int opt,               // INPUT : DARK/LIGHT or GREY          
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx                 // INPUT : length of line
)
{
    char *cur_table;
    char *outbuf;
    int ly = 1;
    
    if (lx < 1)
        return;
    
    if (!GFX_ClipLines(NULL, &x, &y, &lx, &ly))
        return;
    
    switch (opt)
    {
    case DARK:
        cur_table = dtable;
        break;
    case LIGHT:
        cur_table = ltable;
        break;
    case GREY:
        cur_table = gtable;
        break;
    }
    
    GFX_MarkUpdate(x, y, lx, ly);
    
    outbuf = &displaybuffer[x + ylookup[y]];
    
    GFX_Shade(outbuf, lx, cur_table);
}

/*************************************************************************
   GFX_LightBox()- Draws a rectangle border with light source
 *************************************************************************/
void 
GFX_LightBox(
    int opt,               // INPUT : light source
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx,                // INPUT : x length
    int ly                 // INPUT : y length
)
{
    if (lx < 1 || ly < 1)
        return;
    
    switch (opt)
    {
    case UPPER_LEFT:
        GFX_HShadeLine(LIGHT, x, y, lx - 1);
        GFX_VShadeLine(LIGHT, x, y + 1, ly - 2);
        GFX_HShadeLine(DARK, x, y + ly - 1, lx);
        GFX_VShadeLine(DARK, x + lx - 1, y + 1, ly - 2);
        break;
    
    case UPPER_RIGHT:
    default:
        GFX_HShadeLine(LIGHT, x + 1, y, lx - 1);
        GFX_VShadeLine(LIGHT, x + lx - 1, y + 1, ly - 2);
        GFX_HShadeLine(DARK, x, y + ly - 1, lx);
        GFX_VShadeLine(DARK, x, y, ly - 1);
        break;
    
    case LOWER_LEFT:
        GFX_HShadeLine(LIGHT, x, y + ly - 1, lx - 1);
        GFX_VShadeLine(LIGHT, x, y + 1, ly - 2);
        GFX_HShadeLine(DARK, x, y, lx);
        GFX_VShadeLine(DARK, x + lx - 1, y + 1, ly - 1);
        break;
    
    case LOWER_RIGHT:
        GFX_HShadeLine(LIGHT, x + 1, y, lx - 1);
        GFX_VShadeLine(LIGHT, x + lx - 1, y + 1, ly - 2);
        GFX_HShadeLine(DARK, x, y, lx);
        GFX_VShadeLine(DARK, x, y + 1, ly - 2);
        break;
    }
}

/*************************************************************************
   GFX_ColorBox () - sets a rectangular area to color
 *************************************************************************/
void 
GFX_ColorBox(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position         
    int lx,                // INPUT : width
    int ly,                // INPUT : length
    int color              // INPUT : fill color ( 0 - 255 )
)
{
    
    char *outbuf;
    
    if (lx < 1 || ly < 1)
        return;
    
    if (!GFX_ClipLines(NULL, &x, &y, &lx, &ly))
        return;
    
    outbuf = &displaybuffer[x + ylookup[y]];
    
    GFX_MarkUpdate(x, y, lx, ly);
    
    if (color < 0)
    {
        while (ly--)
        {
            GFX_HLine(x, y, lx, color);
            y++;
        }
    }
    else
    {
        while (ly--)
        {
            memset(outbuf, color, lx);
            outbuf += SCREENWIDTH;
        }
    }
}

/*************************************************************************
   GFX_HLine () - plots a horizontal line in color
 *************************************************************************/
void 
GFX_HLine(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx,                // INPUT : width
    int color              // INPUT : fill color ( 0 - 255 )
)
{
    char *outbuf;
    int loop;
    int ly = 1;
    
    if (lx < 1)
        return;
    
    if (!GFX_ClipLines(0, &x, &y, &lx, &ly))
        return;
    
    outbuf = &displaybuffer[x + ylookup[y]];
    
    GFX_MarkUpdate(x, y, lx, 1);
    
    if (color < 0)
    {
        for (loop = 0; loop < lx; loop++)
        {
            *outbuf ^= (color + 255);
            outbuf++;
        }
    }
    else
        memset(outbuf, color, lx);
}

/*************************************************************************
   GFX_VLine () plots a vertical line in color
 *************************************************************************/
void 
GFX_VLine(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int ly,                // INPUT : length
    int color              // INPUT : fill color ( 0 - 255 )
)
{
    char *outbuf;
    int lx = 1;
    
    if (ly < 1)
        return;
    
    if (!GFX_ClipLines(0, &x, &y, &lx, &ly))
        return;
    
    outbuf = &displaybuffer[x + ylookup[y]];
    
    GFX_MarkUpdate(x, y, 1, ly);
    
    if (color < 0)
    {
        while (ly--)
        {
            *outbuf ^= (color + 255);
            outbuf += SCREENWIDTH;
        }
    }
    else
    {
        while (ly--)
        {
            *outbuf = color;
            outbuf += SCREENWIDTH;
        }
    }
}

/*************************************************************************
   GFX_Line () plots a line in color ( Does no Clipping )
 *************************************************************************/
void 
GFX_Line(
    int x,                 // INPUT : x start point 
    int y,                 // INPUT : y start point
    int x2,                // INPUT : x2 end point
    int y2,                // INPUT : y2 end point 
    int color              // INPUT : color ( 0 - 255 )
)
{
    int addx = 1;
    int addy = 1;
    int dely = y2 - y;
    int delx = x2 - x;
    int err, maxloop;
    
    if (delx < 0)
    {
        delx = -delx;
        addx = -addx;
    }
    if (dely < 0)
    {
        dely = -dely;
        addy = -addy;
    }
    
    if (delx >= dely)
    {
        err = -(dely >> 1);
        maxloop = delx + 1;
    }
    else
    {
        err = (delx >> 1);
        maxloop = dely + 1;
    }
    
    if (delx >= dely)
    {
        while (maxloop)
        {
            if (x >= 0 && x < 320 && y >= 0 && y < 200)
            {
                displaybuffer[x + ylookup[y]] = color;
            }
            maxloop--;
            err += dely;
            x += addx;
            
            if (err > 0)
            {
                err -= delx;
                y += addy;
            }
        }
    }
    else
    {
        while (maxloop)
        {
            if (x >= 0 && x <= 320 && y >= 0 && y < 200)
            {
                displaybuffer[x + ylookup[y]] = color;
            }
            maxloop--;
            err += delx;
            y += addy;
            
            if (err > 0)
            {
                err -= dely;
                x += addx;
            }
        }
    }
}

/*************************************************************************
   GFX_Rectangle () - sets a rectangular border to color
 *************************************************************************/
void 
GFX_Rectangle(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx,                // INPUT : width
    int ly,                // INPUT : length
    int color              // INPUT : fill color ( 0 - 255 )
)
{
    if (ly < 1 || lx < 1)
        return;
    
    GFX_HLine(x, y, lx, color);
    GFX_HLine(x, y + ly - 1, lx, color);
    GFX_VLine(x, y + 1, ly - 2, color);
    GFX_VLine(x + lx - 1, y + 1, ly - 2, color);
}

/*************************************************************************
   GFX_ScalePic () - Scales picture optionaly make color 0 see thru
 *************************************************************************/
void 
GFX_ScalePic(
    texture_t *buffin,     // INPUT : pointer to pic data
    int x,                 // INPUT : x display position
    int y,                 // INPUT : y display position
    int new_lx,            // INPUT : new x length 
    int new_ly,            // INPUT : new y length       
    int see_thru           // INPUT : TRUE = see thru
)
{
    char *dest;
    int accum_x, accum_y, i;
    char *pic = buffin->charofs;
    int addx = (buffin->width<<16) / new_lx;
    int addy = (buffin->height<<16) / new_ly;
    accum_x = 0;
    accum_y = 0;
    
    if (!buffin->x)
    {
        GFX_PutSprite(buffin, x, y);
        return;
    }
    
    if (x < 0)
    {
        accum_x = addx * (-x);
        pic += accum_x >> 16;
        accum_x &= 0xffff;
        new_lx += x;
        x = 0;
    }
    
    if (y < 0)
    {
        accum_y = addy * (-y);
        pic += (buffin->width) * (accum_y >> 16);
        accum_y &= 0xffff;
        new_ly += y;
        y = 0;
    }
    
    if (x + new_lx > SCREENWIDTH)
    {
        new_lx = SCREENWIDTH - x;
    }
    if (y + new_ly > SCREENHEIGHT)
    {
        new_ly = SCREENHEIGHT - y;
    }
    
    dest = &displaybuffer[x + ylookup[y]];
    
    GFX_MarkUpdate(x, y, new_lx, new_ly);
    
    tablelen = new_lx;
    
    for (i = new_lx - 1; i >= 0; i--)
    {
        stable[i] = (accum_x >> 16);
        accum_x += addx;
    }
    
    if (see_thru)
    {
        while (new_ly--)
        {
            GFX_CScaleLine(dest, pic + buffin->width * (accum_y>>16));
            accum_y += addy;
            dest += SCREENWIDTH;
        }
    }
    else
    {
        while (new_ly--)
        {
            GFX_ScaleLine(dest, pic + buffin->width * (accum_y>>16));
            accum_y += addy;
            dest += SCREENWIDTH;
        }
    }
}

/*************************************************************************
   GFX_MarkUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void 
GFX_MarkUpdate(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx,                // INPUT : x length
    int ly                 // INPUT : y length
)
{
    int x2 = x + lx - 1;
    int y2 = y + ly - 1;
    int ud_x2 = ud_x + ud_lx - 1;
    int ud_y2 = ud_y + ud_ly - 1;
    
    if (update_start)
    {
        if (x < ud_x)
            ud_x = x;
        
        if (y < ud_y)
            ud_y = y;
        
        if (x2 > ud_x2)
            ud_x2 = x2;
        if (y2 > ud_y2)
            ud_y2 = y2;
    }
    else
    {
        ud_x = x;
        ud_y = y;
        ud_x2 = x2;
        ud_y2 = y2;
        if (lx + ly > 0)
            update_start = 1;
    }
    if ((ud_x) & 3)
        ud_x = ud_x - (ud_x & 3);
    
    if (ud_x < 0)
        ud_x = 0;
    
    if (ud_y < 0)
        ud_y = 0;
    
    ud_lx = (ud_x2 - ud_x + 1);
    ud_ly = (ud_y2 - ud_y + 1);
    
    if ((ud_lx) & 3)
        ud_lx += 4 - (ud_lx & 3);
    
    if (ud_x + ud_lx > SCREENWIDTH)
        ud_lx = SCREENWIDTH - ud_x;
    if (ud_y + ud_ly > SCREENHEIGHT)
        ud_ly = SCREENHEIGHT - ud_y;
}

/*************************************************************************
   GFX_ForceUpdate () Marks an area to be draw with GFX_DrawScreen()
 *************************************************************************/
void 
GFX_ForceUpdate(
    int x,                 // INPUT : x position    
    int y,                 // INPUT : y position
    int lx,                // INPUT : x length
    int ly                 // INPUT : y length
)
{
    ud_x = x;
    ud_y = y;
    ud_lx = lx;
    ud_ly = ly;
    o_ud_x = x;
    o_ud_y = y;
    o_ud_lx = lx;
    o_ud_ly = ly;
}

/***************************************************************************
   GFX_SetFrameHook () sets function to call before every screen update
 ***************************************************************************/
void 
GFX_SetFrameHook(
    void (*func)(void (*)(void))     // INPUT : pointer to function
)
{
    framehook = func;
}

/***************************************************************************
   GFX_WaitUpdate () - Updates screen at specified frame rate
 ***************************************************************************/
void 
GFX_WaitUpdate(
    int count           // INPUT : frame rate ( MAX = 70 )     
)
{
    static int now;
    int get_count, loop;
    
    if (count > 70)
        count = 70;
    else if (count < 1)
        count = 70;
    
    get_count = 70 / count;
    
    GFX_MarkUpdate(o_ud_x, o_ud_y, o_ud_lx, o_ud_ly);
    
    for (loop = 0; loop < get_count; loop++)
    {
        while (now == GFX_GetFrameCount())
        {
        }
        now = GFX_GetFrameCount();
    }
    
    if (update_start)
    {
        if (framehook)
            framehook(GFX_DisplayScreen);
        else
            GFX_DisplayScreen();
    }
    
    o_ud_x = ud_x;
    o_ud_y = ud_y;
    o_ud_lx = ud_lx;
    o_ud_ly = ud_ly;
    
    I_FinishUpdate();
}

/***************************************************************************
   GFX_DisplayUpdate () - Copys Marked areas to display
 ***************************************************************************/
void 
GFX_DisplayUpdate(
    void
)
{
    static int hold;
    
    I_GetEvent();
    GFX_UpdateTimer();
    
    while (hold == framecount)
    {
        GFX_UpdateTimer();
    }
    
    GFX_MarkUpdate(o_ud_x, o_ud_y, o_ud_lx, o_ud_ly);
    
    if (update_start)
    {
        if (framehook)
            framehook(GFX_DisplayScreen);
        else
            GFX_DisplayScreen();
    }
    
    o_ud_x = ud_x;
    o_ud_y = ud_y;
    o_ud_lx = ud_lx;
    o_ud_ly = ud_ly;
    
    hold = framecount;
    
    I_FinishUpdate();
}

/***************************************************************************
   GFX_PutImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void 
GFX_PutImage(
    texture_t *image,      // INPUT : image data 
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int see_thru           // INPUT : true = masked, false = put block
)
{
    char *get_image;
    
    gfx_lx = image->width;
    gfx_ly = image->height;
    
    if (image->x == GSPRITE)
    {
        GFX_PutSprite(image, x, y);
        return;
    }
    
    get_image = image->charofs;
    
    if (!GFX_ClipLines(&get_image, &x, &y, &gfx_lx, &gfx_ly))
        return;
    
    GFX_MarkUpdate(x, y, gfx_lx, gfx_ly);
    
    gfx_xp = x;
    gfx_yp = y;
    
    gfx_inmem = get_image;
    gfx_imga = image->width;
    
    if (see_thru == 0)
    {
        gfx_imga -= gfx_lx;
        GFX_PutPic();
    }
    else
        GFX_PutMaskPic();
}

/***************************************************************************
   GFX_PutSprite () -Puts a Sprite into display buffer
 ***************************************************************************/
void 
GFX_PutSprite(
    texture_t *inmem,      // INPUT : inmem
    int x,                 // INPUT : x pos
    int y                  // INPUT : y pos
)
{
    char rval;
    char *get_inmem;
    texture_t *ah;
    char *outline;
    int ox = x;
    int oy = y;
    int lx = inmem->width;
    int ly = inmem->height;
    
    rval = GFX_ClipLines(NULL, &ox, &oy, &lx, &ly);
    
    if (!rval)
        return;
    
    switch (rval)
    {
    case 1:
        GFX_DrawSprite(&displaybuffer[ox + ylookup[oy]], (texture_t*)inmem->charofs);
        break;
    
    case 2:
        ah = (texture_t*)inmem->charofs;
        while (ah->offset != -1)
        {
            ox = x + ah->x;
            oy = y + ah->y;
            
            get_inmem = (char*)ah + 16;
            
            if (oy > SCREENHEIGHT)
                break;
            
            lx = ah->width;
            ly = 1;
            
            outline = get_inmem;
            
            if (GFX_ClipLines(&outline, &ox, &oy, &lx, &ly))
                memcpy(&displaybuffer[ox + ylookup[oy]], outline, lx);
            
            ah = (texture_t*)(get_inmem + ah->width);
        }
        break;
    }
}

/***************************************************************************
   GFX_OverlayImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void 
GFX_OverlayImage(
    texture_t *baseimage,   // INPUT : base image data         
    texture_t *overimage,   // INPUT : overlay image data
    int x,                  // INPUT : x position
    int y                   // INPUT : y position
)
{
    char *get_baseimage, *get_overimage;
    int addnum, loop, i;
    int x2 = x + overimage->width - 1;
    int y2 = y + overimage->height - 1;
    
    if (x < 0 || y < 0)
        return;
    
    if (x2 >= baseimage->width || y2 >= baseimage->height)
        return;
    
    get_baseimage = &baseimage->charofs[x + baseimage->width * y];
    addnum = overimage->width - baseimage->width;
    get_overimage = overimage->charofs;
    
    for (loop = 0; loop < overimage->height; loop++)
    {
        for (i = 0; i < overimage->width; i++)
        {
            if (i != 255)
                *get_baseimage = *get_overimage;
            
            get_baseimage++;
            get_overimage++;
        }
        get_baseimage += addnum;
    }
}

/***************************************************************************
   GFX_StrPixelLen() - Calculates the length of a GFX string
 ***************************************************************************/
int                        // RETURNS : pixel length
GFX_StrPixelLen(
    font_t *infont,        // INPUT : pointer to current font
    char *instr,           // INPUT : pointer to string
    int maxloop            // INPUT : length of string
)
{
    int loop;
    int outlen = 0;
    
    for (loop = 0; loop < maxloop; loop++)
    {
        outlen += infont->width[instr[loop]] + fontspacing;
    }
    
    return outlen;
}

/*--------------------------------------------------------------------------
   GFX_PutChar () - Draws charater to displaybuffer and clips
 --------------------------------------------------------------------------*/
int 
GFX_PutChar(
    int x,                  // INPUT : x position     
    int y,                  // INPUT : y position
    unsigned char inchar,   // INPUT : char to print
    font_t* font,           // INPUT : pointer to font
    int basecolor           // INPUT : font base color
)
{
    char *dest;
    int lx = font->width[inchar];
    int addx;
    int ly = font->height;
    char* cdata = &font->charofs[font->offset[inchar]];
    addx = lx;
    
    if (!GFX_ClipLines(&cdata, &x, &y, &lx, &ly))
        return 0;
    
    dest = &displaybuffer[x + ylookup[y]];
    
    GFX_MarkUpdate(x, y, lx, ly);
    
    GFX_DrawChar(dest, cdata, lx, ly, addx - lx, basecolor);
    
    return lx;
}

/***************************************************************************
   GFX_Print () - prints a string using specified font with basecolor
 ***************************************************************************/
int                           // RETURN: length of print
GFX_Print(
    int x,                    // INPUT : x position
    int y,                    // INPUT : y position
    char *str,                // INPUT : string to print
    font_t* infont,           // INPUT : pointer to font
    int basecolor             // INPUT : basecolor of font
)
{
    unsigned char ch;
    int length, lx, cwidth;
    
    basecolor--;
    length = strlen(str);
    lx = 0;
    
    if (!length)
        return 0;
    
    while ((ch = *str++) != 0)
    {
        if (infont->offset[ch] == -1)
            continue;
        
        cwidth = GFX_PutChar(x, y, ch, infont, basecolor);
        lx += cwidth + fontspacing;
        x += infont->width[ch] + fontspacing;
    }
    return lx;
}

/***************************************************************************
   GFX_3D_SetView() Sets user view in 3d space
 ***************************************************************************/
void 
GFX_3D_SetView(
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int z                  // INPUT : z position
)
{
    G3D_viewx = x;
    G3D_viewy = y;
    G3D_viewz = z;
}

/*--------------------------------------------------------------------------
   GFX_3DPoint () plots a points in 3D space
 --------------------------------------------------------------------------*/
void 
GFX_3DPoint(
    void
)
{
    G3D_x -= G3D_viewx;
    G3D_y -= G3D_viewy;
    G3D_z -= G3D_viewz;
    
    G3D_screenx = ((G3D_x * (G3D_DIST<<11)) / G3D_z) >> 11;
    G3D_screeny = ((G3D_y * (G3D_DIST<<11)) / G3D_z) >> 11;
    
    G3D_screenx += G3D_viewx;
    G3D_screeny += G3D_viewy;
}

/***************************************************************************
   GFX_3D_PutImage() - places image in displaybuffer and performs cliping
 ***************************************************************************/
void 
GFX_3D_PutImage(
    texture_t *image,      // INPUT : image data
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int z,                 // INPUT : z position ( distance )
    int see_thru           // INPUT : true = masked, false = put block
)
{
    int x1, y1, new_lx, new_ly;
    
    if (z == G3D_DIST)
    {
        GFX_MarkUpdate(x, y, image->width, image->height);
        GFX_PutImage(image, x, y, see_thru);
        return;
    }
    
    G3D_x = x;
    G3D_y = y;
    G3D_z = z;
    GFX_3DPoint();
    x1 = G3D_screenx;
    y1 = G3D_screeny;
    
    G3D_x = x + image->width - 1;
    G3D_y = y + image->height - 1;
    G3D_z = z;
    GFX_3DPoint();
    
    new_lx = G3D_screenx - x1;
    new_ly = G3D_screeny - y1;
    
    GFX_MarkUpdate(x1, y1, new_lx, new_ly);
    GFX_ScalePic(image, x1, y1, new_lx, new_ly, see_thru);
}

