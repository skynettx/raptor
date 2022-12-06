#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "common.h"
#include "i_video.h"
#include "gfxapi.h"
#include "glbapi.h"
#include "swdapi.h"
#include "imsapi.h"
#include "ptrapi.h"
#include "kbdapi.h"
#include "joyapi.h"
#include "input.h"

int g_joy_ascii;
unsigned int fi_joy_count;
bool fi_sec_field;

int kbactive;
int g_button_flag = 1;
int g_key;
int g_ascii;
int cur_act;
int cur_cmd;
int obj_x;
int obj_y;
int obj_height;
int obj_width;
int old_win = -99;
int old_field = -99;
int highlight_flag;

int usekb_flag;
int prev_window = -1;

int active_window = -1;
int active_field = -1;
int master_window = -1;

int clearscreenflag = 1;

void (*viewdraw)(void);

window_t g_wins[MAX_WINDOWS];

swdfield_t *lastfld;

char *movebuffer;

void (*winfuncs[MAX_WINDOWS])(wdlg_t*);
void (*fldfuncs[15])(swd_t*, swdfield_t*);

// == STUFF FOR TEXT SCRIPT ==============================

#define MAX_TEXT_LEN 81
#define MAX_ARGS     5

enum TEXTCMD
{
    T_NONE,
    T_IMAGE,
    T_COLOR,
    T_TEXT_POS,
    T_RIGHT,
    T_DOWN,
    T_LASTCMD
};

char tcmds[][14] = {
    "TEXT_IMAGE",   // GLBNAME, X, Y
    "TEXT_COLOR",   // COLOR #
    "TEXT_POS",     // X, Y
    "TEXT_RIGHT",   // X add
    "TEXT_DOWN"     // Y add
};

int textcmd_flag;
int textdraw_x, textdraw_y;
int textcmd_x, textcmd_y;
int textcmd_x2, textcmd_y2;
int textcolor;
int textcmd_line;
char textfill[MAX_TEXT_LEN];

/*------------------------------------------------------------------------
   SWD_ShadeButton () -
  ------------------------------------------------------------------------*/
void 
SWD_ShadeButton(
    int opt,               // INPUT : NORMAL/UP/DOWN
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx,                // INPUT : x width
    int ly                 // INPUT : y width
)
{
    switch (opt)
    {
    case DOWN:
        GFX_HShadeLine(DARK, x, y, lx);
        GFX_VShadeLine(DARK, x + lx - 1, y + 1, ly - 1);
        break;
    
    case UP:
        GFX_HShadeLine(LIGHT, x + 2, y, lx - 2);
        GFX_VShadeLine(LIGHT, x + lx - 1, y + 1, ly - 3);
    
    default:
        GFX_HShadeLine(LIGHT, x + 1, y, lx - 1);
        GFX_VShadeLine(LIGHT, x + lx - 1, y + 1, ly - 2);
        GFX_HShadeLine(DARK, x, y + ly - 1, lx);
        GFX_VShadeLine(DARK, x, y, ly - 1);
        break;
    }
}

/*------------------------------------------------------------------------
   SWD_GetLine () -
  ------------------------------------------------------------------------*/
int 
SWD_GetLine(
    const char *inmem
)
{
    char temp[MAX_TEXT_LEN];
    static const char *text;
    const char *cbrks = "\n\v\r \t,;\b";
    const char *cmd;
    int curpos, loop, item, x, y, col;
    texture_t *pic;
    
    if (inmem)
        text = inmem;
    
    textcmd_flag = 0;
    curpos = 0;
    
    memcpy(temp, text, MAX_TEXT_LEN);
    
    cmd = strtok(temp, cbrks);
    
    for (loop = 0; loop < T_LASTCMD - 1; loop++)
    {
        if (!strcmp(cmd, tcmds[loop]))
        {
            textcmd_flag = 1;
            
            while (text[curpos] > 31)
                curpos++;
            
            while (text[curpos] <= 31)
                curpos++;
            
            text += curpos;
            
            cmd = strtok(NULL, cbrks);
            
            switch (loop + 1)
            {
            case T_IMAGE:
                item = GLB_GetItemID(cmd);
                if (item == -1)
                    break;
                pic = (texture_t*)GLB_GetItem(item);
                cmd = strtok(NULL, cbrks);
                
                if (!cmd)
                {
                    x = textdraw_x;
                    y = textdraw_y;
                }
                else
                {
                    x = atoi(cmd);
                    cmd = strtok(NULL, cbrks);
                    y = atoi(cmd);
                    
                    x += textcmd_x;
                    y += textcmd_y;
                }
                
                if (x > textcmd_x2 || y > textcmd_y2)
                    break;
                
                textdraw_x += pic->width + 1;
                textdraw_y = y;
                
                GFX_PutImage(pic, x, y, 0);
                GLB_FreeItem(item);
                break;
            
            case T_COLOR:
                col = atoi(cmd);
                if (col >= 0 && col < 256)
                    textcolor = col;
                break;
            
            case T_TEXT_POS:
                x = atoi(cmd);
                cmd = strtok(NULL, cbrks);
                y = atoi(cmd);
                
                if (x > textcmd_x2 || y > textcmd_y2)
                    break;
                
                textdraw_x = textcmd_x + x;
                textdraw_y = textcmd_y + y;
                textcmd_line = 0;
                break;
            
            case T_RIGHT:
                x = atoi(cmd);
                if (x > textcmd_x2)
                    break;
                if (cmd)
                    textdraw_x += x;
                break;
            
            case T_DOWN:
                y = atoi(cmd);
                if (y > textcmd_y2)
                    break;
                if (cmd)
                    textdraw_y += y;
                break;
            }
            return curpos;
        }
    }
    
    while (text[curpos] > 31)
        curpos++;
    
    memcpy(textfill, text, curpos);
    textfill[curpos] = 0;
    
    while (text[curpos] <= 31)
        curpos++;
    
    text += curpos;
    
    return curpos;
}

/***************************************************************************
SWD_FillText () - Fills Text from GLB intro an AREA
 ***************************************************************************/
void 
SWD_FillText(
    font_t *font,          // INPUT : pointer to FONT
    int item,              // INPUT : GLB text Item
    int color,             // INPUT : field color
    int x,                 // INPUT : x position
    int y,                 // INPUT : y position
    int lx,                // INPUT : width of field
    int ly                 // INPUT : height of field
)
{
    char *text;
    int sizerec, len;
    
    if (item == -1)
        return;
    
    text = GLB_LockItem(item);
    
    if (!text)
        return;
    
    textcmd_x = x;
    textdraw_x = x;
    textcmd_y = y;
    textdraw_y = y;
    textcolor = color;
    textcmd_x2 = x + lx - 1;
    textcmd_y2 = y + ly - 1;
    textcmd_line = x;
    
    sizerec = GLB_GetItemSize(item);
    
    len = SWD_GetLine(text);
    
    while (1)
    {
        if (!textcmd_flag)
        {
            GFX_Print(textdraw_x, textdraw_y, textfill, font, textcolor);
            textdraw_y += font->height + 3;
        }
        
        if (len < sizerec)
            len += SWD_GetLine(NULL);
        else
            break;
    }
    
    GLB_FreeItem(item);
}

/*------------------------------------------------------------------------
  SWD_PutField() - puts a field in displaybuffer
  ------------------------------------------------------------------------*/
void 
SWD_PutField(
    swd_t *curwin,         // INPUT : pointer to window data
    swdfield_t *curfld     // INPUT : pointer to field data
)
{
    font_t *fld_font;
    char *fld_text;
    int fontheight;
    int draw_style;
    int fld_x;
    int draw_text;
    int fld_y;
    int curpos;
    int text_x;
    int text_y;
    int rval;
    int loop;
    texture_t *pic;
    
    fld_font = (font_t*)GLB_GetItem(curfld->fontid);
    fld_text = (char*)curfld + curfld->txtoff;
    fontheight = fld_font->height;
    draw_style = 0;
    fld_x = curfld->x + curwin->x;
    draw_text = 0;
    fld_y = curfld->y + curwin->y;
    curpos = strlen(fld_text);
    
    rval = GFX_StrPixelLen(fld_font, fld_text, curpos);
    text_x = fld_x + ((curfld->lx - rval) >> 1);
    text_y = fld_y + ((curfld->ly - fld_font->height) >> 1);
    
    if (curfld->bstatus == DOWN && curfld->opt != FLD_DRAGBAR)
    {
        if (text_x > 0)
            text_x--;
        
        text_y++;
    }
    
    if (curfld->saveflag && curfld->sptr)
        GFX_PutImage(curfld->sptr, fld_x, fld_y, 0);
    
    if (curfld->picflag && curfld->picflag != INVISABLE)
    {
        if (curfld->item == -1) 
            goto PutField_Exit;
        
        if (curwin->numflds == SEE_THRU)
            draw_style = 1;
        
        switch (curfld->opt)
        {
        case FLD_BUTTON:
            pic = (texture_t*)GLB_GetItem(curfld->item);
            
            if (curfld->picflag == TEXTURE)
            {
                GFX_PutTexture(pic, fld_x, fld_y, curfld->lx, curfld->ly);
                SWD_ShadeButton(curfld->bstatus, fld_x, fld_y, curfld->lx, curfld->ly);
            }
            else
            {
                GFX_PutImage(pic, fld_x, fld_y, draw_style);
            }
            draw_text = 1;
            break;
        
        case FLD_DRAGBAR:
            pic = (texture_t*)GLB_GetItem(curfld->item);
            
            if (curfld->picflag == TEXTURE)
            {
                GFX_PutTexture(pic, fld_x, fld_y, curfld->lx, curfld->ly);
                GFX_LightBox(UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly);
            }
            else
            {
                GFX_PutImage(pic, fld_x, fld_y, draw_style);
            }
            if (curwin != g_wins[active_window].win)
                GFX_ShadeArea(GREY, fld_x, fld_y, curfld->lx, curfld->ly);
            
            draw_text = 1;
            break;
        
        case FLD_ICON:
            pic = (texture_t*)GLB_GetItem(curfld->item);
            
            if (!pic)
                break;
            
            if (curfld->picflag == TEXTURE)
            {
                GFX_PutTexture(pic, fld_x, fld_y, curfld->lx, curfld->ly);
                goto PutField_Exit;
            }
            if (curfld->lx < pic->width || curfld->ly < pic->height)
            {
                GFX_ScalePic(pic, fld_x, fld_y, curfld->lx, curfld->ly, 0);
            }
            else
            {
                GFX_PutImage(pic, fld_x, fld_y, draw_style);
            }
            break;
        
        case FLD_MARK:
        case FLD_CLOSE:
            pic = (texture_t*)GLB_GetItem(curfld->item);
            GFX_PutImage(pic, fld_x, fld_y, draw_style);
            break;
        
        case FLD_TEXT:
            SWD_FillText(fld_font, curfld->item, curfld->fontbasecolor, fld_x, fld_y, curfld->lx, curfld->ly);
            break;
        
        case 3:
        case 8:
        case 7:
            break;
        }
        
        if (!curfld->bstatus)
            goto PutField_Exit;
    }
    else
    {
        switch (curfld->opt)
        {
        case FLD_TEXT:
            if (curfld->maxchars)
                GFX_Print(fld_x, fld_y, fld_text, fld_font, curfld->fontbasecolor);
            break;
        
        case FLD_BUTTON:
            if (curfld->picflag != INVISABLE)
            {
                GFX_ColorBox(fld_x, fld_y, curfld->lx, curfld->ly, curfld->color);
                SWD_ShadeButton(curfld->bstatus, fld_x, fld_y, curfld->lx, curfld->ly);
                draw_text = 1;
            }
            else
            {
                GFX_Print(text_x, text_y, fld_text, fld_font, curfld->fontbasecolor);
            }
            break;
        
        case FLD_INPUT:
            if (curfld->bstatus == NORMAL)
                GFX_ColorBox(fld_x, fld_y, curfld->lx, curfld->ly, curfld->color);
            else
                GFX_ColorBox(fld_x, fld_y, curfld->lx, curfld->ly, curfld->lite);
            
            if (curfld->maxchars)
                GFX_Print(fld_x + 1, text_y, fld_text, fld_font, curfld->fontbasecolor);
            
            if (curfld->bstatus)
            {
                curpos = strlen(fld_text);
                rval = GFX_StrPixelLen(fld_font, fld_text, curpos);
                
                text_x = fld_x + 1 + rval;
                
                if (rval + 2 < curfld->lx)
                    GFX_VLine(text_x, fld_y + 1, fontheight - 1, curfld->fontbasecolor);
            }
            break;
        
        case FLD_MARK:
            GFX_ColorBox(fld_x, fld_y, curfld->lx, curfld->ly, curfld->color);
            GFX_LightBox(UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly);
            GFX_ColorBox(fld_x + 2, fld_y + 2, curfld->lx - 4, curfld->ly - 4, 0);
            text_x = fld_x + 3;
            text_y = fld_y + 3;
            if (curfld->mark)
            {
                GFX_ColorBox(fld_x + 3, fld_y + 3, curfld->lx - 6, curfld->ly - 6, curfld->lite);
                SWD_ShadeButton(curfld->bstatus, fld_x + 3, fld_y + 3, curfld->lx - 6, curfld->ly - 6);
            }
            else
            {
                GFX_ColorBox(fld_x + 3, fld_y + 3, curfld->lx - 6, curfld->ly - 6, 0);
            }
            break;
        
        case FLD_CLOSE:
            if (curfld->picflag == INVISABLE) 
                goto PutField_Exit;

            GFX_ColorBox(fld_x, fld_y, curfld->lx, curfld->ly, curfld->lite);
            GFX_LightBox(UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly);
            GFX_ColorBox(fld_x + 2, fld_y + 2, curfld->lx - 4, curfld->ly - 4, curfld->lite);
            GFX_ColorBox(fld_x + 3, fld_y + 3, curfld->lx - 6, curfld->ly - 6, curfld->lite);
            SWD_ShadeButton(curfld->bstatus, fld_x + 3, fld_y + 3, curfld->lx - 6, curfld->ly - 6);
            text_x = fld_x + 3;
            text_y = fld_y + 3;
            break;
        
        case FLD_DRAGBAR:
            if (curfld->picflag != INVISABLE)
                GFX_ColorBox(fld_x, fld_y, curfld->lx, curfld->ly, curfld->color);
            
            if (curfld->maxchars > 1)
                GFX_Print(text_x, text_y, fld_text, fld_font, curfld->fontbasecolor);
            
            if (curfld->picflag != INVISABLE && curwin != g_wins[active_window].win)
            {
                GFX_ShadeArea(DARK, fld_x, fld_y, curfld->lx, curfld->ly);
                
                if (curfld->color)
                {
                    for (loop = 0; loop < curfld->ly; loop += 2)
                    {
                        GFX_HShadeLine(DARK, fld_x, fld_y + loop, curfld->lx);
                    }
                }
            }
            break;
        
        case FLD_BUMPIN:
            if (curfld->color)
                GFX_ShadeArea(DARK, fld_x + 1, fld_y, curfld->lx - 1, curfld->ly - 1);
            GFX_LightBox(LOWER_LEFT, fld_x, fld_y, curfld->lx, curfld->ly);
            if (!curfld->color)
                GFX_ColorBox(fld_x + 1, fld_y + 1, curfld->lx - 2, curfld->ly - 2, 0);
            break;
        
        case FLD_BUMPOUT:
            GFX_ShadeArea(LIGHT, fld_x + 1, fld_y, curfld->lx - 1, curfld->ly - 1);
            GFX_LightBox(UPPER_RIGHT, fld_x, fld_y, curfld->lx, curfld->ly);
            if (!curfld->color)
                GFX_ColorBox(fld_x + 1, fld_y + 1, curfld->lx - 2, curfld->ly - 2, 0);
            break;
        }
    }
    
    if (curfld->bstatus && curfld->opt != FLD_INPUT)
    {
        if (curfld->picflag == PICTURE)
            pic = (texture_t*)GLB_GetItem(curfld->item);
        else
            pic = NULL;
        
        if (curfld->bstatus == DOWN)
        {
            if (pic && pic->x == 0)
                GFX_ShadeShape(DARK, pic, fld_x, fld_y);
            else
                GFX_ShadeArea(DARK, fld_x, fld_y, curfld->lx, curfld->ly);
        }
        else if (curfld->bstatus == UP)
        {
            if (pic && pic->x == 0)
                GFX_ShadeShape(LIGHT, pic, fld_x, fld_y);
            else
                GFX_ShadeArea(LIGHT, fld_x, fld_y, curfld->lx, curfld->ly);
        }
    }

PutField_Exit:
    
    if (draw_text && curfld->maxchars > 1)
        GFX_Print(text_x, text_y, fld_text, fld_font, curfld->fontbasecolor);
}

/*------------------------------------------------------------------------
  SWD_DoButton() - processes all buttons from SWD_Dialog
  ------------------------------------------------------------------------*/
void 
SWD_DoButton(
    swd_t *curwin,         // INPUT : pointer to current window
    swdfield_t *curfld     // INPUT : pointer to current field
)
{
    // == CONTROLLER INPUT ==============================
    
    if (joy_ipt_MenuNew)                                                 
    {
        if (StickY > 0 || Down)                                                   
        {
            if (JOY_IsScroll(0) == 1)
                g_key = SC_DOWN;
        }
        
        if (StickY < 0 || Up)
        {
            if (JOY_IsScroll(0) == 1)
                g_key = SC_UP;
        }
        
        if (StickX > 0 || Right)
        {
            if (JOY_IsScroll(0) == 1)
                g_key = SC_RIGHT;
        }
        
        if (StickX < 0 || Left)
        {
            if (JOY_IsScroll(0) == 1)
                g_key = SC_LEFT;
        }
        
        if (AButton)
        {
            JOY_IsKey(AButton);
            g_key = SC_ENTER;
        }
    }
    
    if (!g_button_flag)
        return;
    
    switch (g_key)
    {
    case SC_TAB:
        if (keyboard[SC_ALT])
        {
            cur_act = S_WIN_COMMAND;
            cur_cmd = W_NEXT;
            while (keyboard[SC_TAB]) {
            }
        }
        else
        {
            cur_act = S_FLD_COMMAND;
            
            if (KBD_ISCAPS)
                cur_cmd = F_PREV;
            else
                cur_cmd = F_NEXT;
        }
        break;
    
    case SC_ENTER:
        cur_act = S_FLD_COMMAND;                                                         
        cur_cmd = F_SELECT;
        break;
    
    case SC_DOWN:
        if (curwin->arrowflag)                                                         
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_DOWN;
        }
        break;
    
    case SC_UP:                                                                
        if (curwin->arrowflag)
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_UP;
        }
        break;
    
    case SC_RIGHT:
        if (curwin->arrowflag)                                                        
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_RIGHT;
        }
        break;
    
    case SC_LEFT:
        if (curwin->arrowflag)                                                        
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_LEFT;
        }
        break;
    }
}

/*------------------------------------------------------------------------
  SWD_FieldInput() - Field Input function for SWD_Dialog
  ------------------------------------------------------------------------*/
void 
SWD_FieldInput(
    swd_t *curwin,          // INPUT : pointer to current window
    swdfield_t *curfld      // INPUT : pointer to current field
)
{
    static int curpos;
    font_t *fld_font;
    char *wrkbuf;
    int flag;
    flag = 0;
    fld_font = (font_t*)GLB_GetItem(curfld->fontid);
    wrkbuf = (char*)curfld + curfld->txtoff;
    
    curpos = strlen(wrkbuf);
    
    // == CONTROLLER FIELDINPUT ==============================   
    
    if (joy_ipt_MenuNew)
    {
        
        // == INPUT CONTROLLER MAX FIELDINPUT ==============================
        
        if (StickY || Down || Up || AButton || YButton || Start)            
        {
            if (curpos > 17)
            {
                curpos--;
                wrkbuf[curpos] = 0;
            }
            
            if (fi_sec_field)
            {
                if (curpos > 10)
                {
                    curpos--;
                    wrkbuf[curpos] = 0;
                }
            }
        }
        
        // == INPUT CONTROLLER ASCII TABLE DOWN ==============================

        if (StickY > 0 || Down)                                                    
        {
            if (JOY_IsScroll(0) == 1)
            {
                if (fi_joy_count > 0)
                {
                    curpos--;
                    g_joy_ascii--;
                    wrkbuf[curpos] = g_joy_ascii;
                    
                    if (g_joy_ascii < 0x30)
                    {
                        g_joy_ascii = 0x5a;
                        wrkbuf[curpos] = g_joy_ascii;
                    }
                }
                
                if (fi_joy_count == 0)
                {
                    fi_joy_count++;
                    g_joy_ascii = 0x41;
                    wrkbuf[curpos] = g_joy_ascii;
                    wrkbuf[curpos + 1] = 0;
                }
            }
        }
        
        // == INPUT CONTROLLER ASCII TABLE UP ==============================

        if (StickY < 0 || Up)                                                    
        {
            if (JOY_IsScroll(0) == 1)
            {
                if (fi_joy_count > 0)
                {
                    curpos--;
                    g_joy_ascii++;
                    wrkbuf[curpos] = g_joy_ascii;
                    
                    if (g_joy_ascii > 0x5a)
                    {
                        g_joy_ascii = 0x30;
                        wrkbuf[curpos] = g_joy_ascii;
                    }
                }
                
                if (fi_joy_count == 0)
                {
                    fi_joy_count++;
                    g_joy_ascii = 0x41;
                    wrkbuf[curpos] = g_joy_ascii;
                    wrkbuf[curpos + 1] = 0;
                }
            }
        }
        
        if (StickX > 0 || Right)
        {
            if (JOY_IsScroll(0) == 1)
                g_key = SC_RIGHT;
        }
        
        if (StickX < 0 || Left)
        {
            if (JOY_IsScroll(0) == 1)
                g_key = SC_LEFT;
        }
        
        // == INPUT CONTROLLER NEXT INPUT ==============================

        if (AButton)                                                  
        {
            JOY_IsKey(AButton);
            curpos++;
            fi_joy_count = 0;
        }
        
        // == INPUT CONTROLLER DELETE ==============================

        if (XButton)                                                  
        {
            JOY_IsKey(XButton);
            flag = 1;
            
            if (curpos > 0)
                curpos--;
            
            wrkbuf[curpos] = 0;
            fi_joy_count = 0;
        }
        
        // == INPUT CONTROLLER SPACE ==============================
        
        if (YButton)                                                  
        {
            JOY_IsKey(YButton);
            wrkbuf[curpos + 1] = 0;
            g_joy_ascii = 0x20;
            wrkbuf[curpos] = g_joy_ascii;
            fi_joy_count = 0;
        }
        
        // == INPUT CONTROLLER CONFIRM ==============================

        if (Start)                                                   
        {
            JOY_IsKey(Start);
            g_key = SC_ENTER;
            fi_joy_count = 0;
        }
    }
    
    switch (g_key)
    {
    case SC_TAB:
        if (keyboard[SC_ALT])
        {
            while (keyboard[SC_TAB]) {
            }
            cur_act = S_WIN_COMMAND;
            cur_cmd = W_NEXT;
        }
        else
        {
            cur_act = S_FLD_COMMAND;
            
            if (KBD_ISCAPS)
                cur_cmd = F_PREV;
            else
                cur_cmd = F_NEXT;
        }
        
        if (fi_sec_field == false)
            fi_sec_field = true;
        else
            fi_sec_field = false;
        break;
    
    case SC_ENTER:
        cur_act = S_FLD_COMMAND;
        cur_cmd = F_SELECT;
        break;
    
    case SC_DOWN:
        if (curwin->arrowflag)
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_DOWN;
        }
        break;
    
    case SC_UP:
        if (curwin->arrowflag)
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_UP;
        }
        break;
    
    case SC_RIGHT:
        if (curwin->arrowflag)
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_RIGHT;
        }
        break;
    
    case SC_LEFT:
        if (curwin->arrowflag)
        {
            cur_act = S_FLD_COMMAND;
            cur_cmd = F_LEFT;
        }
        break;
    
    case SC_BACKSPACE:
        flag = 1;
        if (curpos > 0)
            curpos--;
        wrkbuf[curpos] = 0;
        break;
    
    default:
        if (keyboard[SC_Y] && keyboard[SC_CTRL])
        {
            curpos = 0;
            flag = 1;
            *wrkbuf = 0;
        }
        else if (!keyboard[SC_ALT] && !keyboard[SC_CTRL] && g_key > 0 && curfld->maxchars-1 > curpos)
        {
            if (g_ascii > 31 && g_ascii < 127)
            {
                switch (curfld->input_opt)
                {
                case I_NORM:
                    wrkbuf[curpos] = g_ascii;
                    break;
                
                case I_TOUPPER:
                    wrkbuf[curpos] = toupper(g_ascii);
                    break;
                
                case I_NUMERIC:
                    if (isdigit(g_ascii) || g_ascii == '-')
                        wrkbuf[curpos] = g_ascii;
                    else
                        wrkbuf[curpos] = 0;
                    break;
                }
            }
            else
                wrkbuf[curpos] = 0;
            
            if (GFX_StrPixelLen(fld_font, wrkbuf, curpos + 1) >= curfld->lx)
                curpos--;
            
            flag = 1;
            wrkbuf[curpos + 1] = 0;
        }
        break;
    }
    
    if (flag)
    {
        SWD_PutField(curwin, curfld);
        cur_act = S_UPDATE;
        cur_cmd = C_IDLE;
    }
}

/*------------------------------------------------------------------------
   SWD_GetObjAreaInfo () - looks for a objarea then sets obj_xx variables
  ------------------------------------------------------------------------*/
void 
SWD_GetObjAreaInfo(
    int handle             // INPUT: handle of window
)
{
    int loop;
    swd_t* cwin;
    swdfield_t *curfld;
    
    obj_x = 0;
    obj_y = 0;
    obj_width = 0;
    obj_height = 0;
    
    cwin = g_wins[handle].win;
    curfld = (swdfield_t*)((char*)cwin + cwin->fldofs);
    
    for (loop = 0; loop < cwin->numflds; loop++)
    {
        if (curfld[loop].opt == FLD_OBJAREA)
        {
            obj_x = curfld[loop].x;
            obj_y = curfld[loop].y;
            obj_width = curfld[loop].lx;
            obj_height = curfld[loop].ly;
            return;
        }
    }
}

/*------------------------------------------------------------------------
  SWD_GetNextWindow() - Gets the Next Active Window
  ------------------------------------------------------------------------*/
void 
SWD_GetNextWindow(
    void
)
{
    int loop;
    int pos;
    pos = active_window - 1;
    
    active_window = -1;
    
    if (pos < 0)
        pos = MAX_WINDOWS - 1;
    
    for (loop = 0; loop < MAX_WINDOWS; loop++)
    {
        if (g_wins[pos].flag && g_wins[pos].win->display)
        {
            active_window = pos;
            active_field = g_wins[pos].win->firstfld;
            break;
        }
        pos--;
        if (pos < 0)
            pos = MAX_WINDOWS - 1;
    }
    
    if (active_window == -1)
        active_field = -1;
    
    lastfld = NULL;
}

/*------------------------------------------------------------------------
  SWD_GetFirstField() - Gets the first selectable field
  ------------------------------------------------------------------------*/
int 
SWD_GetFirstField(
    void
)
{
    return g_wins[active_window].win->firstfld;
}

/*------------------------------------------------------------------------
  SWD_GetLastField() - Gets the last selectable field
  ------------------------------------------------------------------------*/
int 
SWD_GetLastField(
    swdfield_t *firstfld,   // INPUT : pointer to first field
    int maxfields           // INPUT : number of fields
)
{
    int rval;
    int loop;
    rval = -1;
    
    for (loop = maxfields - 1; loop >= 0; loop--)
    {
        switch (firstfld[loop].opt)
        {
        case FLD_OFF:
        case FLD_TEXT:
        case FLD_DRAGBAR:
        case FLD_BUMPIN:
        case FLD_BUMPOUT:
        case FLD_ICON:
            break;
        
        default:
            if (firstfld[loop].selectable)
                rval = loop;
            break;
        }
        
        if (rval != -1)
            break;
    }
    
    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetNextField() - Gets the Next selectable field
  ------------------------------------------------------------------------*/
int
SWD_GetNextField(
    swdfield_t* firstfld,   // INPUT : pointer to first field
    int maxfields           // INPUT : number of fields
)
{
    int rval, loop, low, del;
    swdfield_t* activefld;

    low = 0x7fff;
    activefld = firstfld + active_field;
    rval = SWD_GetFirstField();

    for (loop = 0; loop < maxfields; loop++)
    {
        if (firstfld[loop].opt != FLD_DRAGBAR && firstfld[loop].selectable && firstfld[loop].id > activefld->id)
        {
            del = abs(firstfld[loop].id - activefld->id);

            if (del < low)
            {
                low = del;
                rval = loop;
            }
        }
    }

    if (rval != -1)
        active_field = rval;

    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetPrevField() - Gets the Previous selectable field
  ------------------------------------------------------------------------*/
int 
SWD_GetPrevField(
    swdfield_t *firstfld,   // INPUT : pointer to first field
    int maxfields           // INPUT : number of fields
)
{
    int rval, loop, low, del;
    swdfield_t *activefld;
    
    low = 0x7fff;
    activefld = firstfld + active_field;
    rval = SWD_GetLastField(firstfld, maxfields);
    
    for (loop = 0; loop < maxfields; loop++)
    {
        if (firstfld[loop].opt != FLD_DRAGBAR && firstfld[loop].selectable && firstfld[loop].id < activefld->id)
        {
            del = abs(activefld->id - firstfld[loop].id);
            
            if (del < low)
            {
                low = del;
                rval = loop;
            }
        }
    }
    
    if (rval != -1)
        active_field = rval;
    
    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetRightField() - Gets the closest right selectable field
  ------------------------------------------------------------------------*/
int 
SWD_GetRightField(
    swdfield_t* firstfld,   // INPUT : pointer to first field
    int maxfields           // INPUT : number of fields
) 
{
    int rval, low, loop, del;
    swdfield_t* activefld;

    rval = -1;
    low = 0x7fff;
    activefld = firstfld + active_field;
    
    for (loop = 0; loop < maxfields; loop++)
    {
        switch (firstfld[loop].opt)
        {
        default:
            if (firstfld[loop].x > activefld->x)
            {
                del = abs(firstfld[loop].x - activefld->x) + abs(firstfld[loop].y - activefld->y);
                
                if (del < low && firstfld[loop].selectable)
                {
                    low = del;
                    rval = loop;
                }
            }
            break;
        
        case FLD_OFF:
        case FLD_TEXT:
        case FLD_DRAGBAR:
        case FLD_BUMPIN:
        case FLD_BUMPOUT:
        case FLD_ICON:
            break;
        }
    }
    
    if (rval < 0)
        SWD_GetNextField(firstfld, maxfields);
    else
        active_field = rval;
    
    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetUpField() - Gets the closest right selectable field
  ------------------------------------------------------------------------*/
int 
SWD_GetUpField(
    swdfield_t *firstfld,  // INPUT : pointer to first field
    int maxfields          // INPUT : number of fields
)
{
    int rval, low, loop, del;
    swdfield_t *activefld;

    low = 0x7fff;
    rval = SWD_GetLastField(firstfld, maxfields);
    activefld = firstfld + active_field;
    
    for (loop = 0; loop < maxfields; loop++)
    {
        switch (firstfld[loop].opt)
        {
        default:
            if (firstfld[loop].y < activefld->y)
            {
                del = abs(firstfld[loop].x - activefld->x) + abs(firstfld[loop].y - activefld->y);
                
                if (del < low && firstfld[loop].selectable)
                {
                    low = del;
                    rval = loop;
                }
            }
            break;
        
        case FLD_OFF:
        case FLD_TEXT:
        case FLD_DRAGBAR:
        case FLD_BUMPIN:
        case FLD_BUMPOUT:
        case FLD_ICON:
            break;
        }
    }
    
    if (rval < 0)
        active_field = SWD_GetFirstField();
    else
        active_field = rval;
    
    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetDownField() - Gets the closest right selectable field
  ------------------------------------------------------------------------*/
int 
SWD_GetDownField(
    swdfield_t *firstfld,  // INPUT : pointer to first field
    int maxfields          // INPUT : number of fields
)
{
    int rval, low, loop, del;
    swdfield_t *activefld;

    low = 0x7fff;
    rval = SWD_GetFirstField();
    activefld = firstfld + active_field;
    
    for (loop = 0; loop < maxfields; loop++)
    {
        switch (firstfld[loop].opt)
        {
        default:
            if (firstfld[loop].y > activefld->y)
            {
                del = abs(firstfld[loop].x - activefld->x) + abs(firstfld[loop].y - activefld->y);
                
                if (del < low && firstfld[loop].selectable)
                {
                    low = del;
                    rval = loop;
                }
            }
            break;
        
        case FLD_OFF:
        case FLD_TEXT:
        case FLD_DRAGBAR:
        case FLD_BUMPIN:
        case FLD_BUMPOUT:
        case FLD_ICON:
            break;
        }
    }
    
    if (rval < 0)
        active_field = SWD_GetFirstField();
    else
        active_field = rval;
    
    return rval;
}

/*------------------------------------------------------------------------
  SWD_GetLeftField() - Gets the closest right selectable field
  ------------------------------------------------------------------------*/
int 
SWD_GetLeftField(
    swdfield_t *firstfld,   // INPUT : pointer to first field
    int maxfields           // INPUT : number of fields
)
{
    int rval, low, loop, del;
    swdfield_t *activefld;

    rval = -1;
    low = 0x7fff;
    activefld = firstfld + active_field;
    
    for (loop = 0; loop < maxfields; loop++)
    {
        switch (firstfld[loop].opt)
        {
        default:
            if (firstfld[loop].x < activefld->x)
            {
                del = abs(firstfld[loop].x - activefld->x) + abs(firstfld[loop].y - activefld->y);
                
                if (del < low && firstfld[loop].selectable)
                {
                    low = del;
                    rval = loop;
                }
            }
            break;
        
        case FLD_OFF:
        case FLD_TEXT:
        case FLD_DRAGBAR:
        case FLD_BUMPIN:
        case FLD_BUMPOUT:
        case FLD_ICON:
            break;
        }
    }
    
    if (rval < 0)
        SWD_GetPrevField(firstfld, maxfields);
    else
        active_field = rval;
    
    return rval;
}

int SWD_ShowAllFields(swd_t *a1)
{
    int i, v20, vbp;
    texture_t *tex;
    swdfield_t *vs = (swdfield_t*)((char*)a1 + a1->fldofs);
    for (i = 0; i < a1->numflds; i++)
    {
        if (vs[i].opt)
        {
            vbp = a1->x + vs[i].x;
            v20 = a1->y + vs[i].y;
            if (vs[i].saveflag && vs[i].sptr)
            {
                vs[i].sptr->width = (short)vs[i].lx;
                vs[i].sptr->height = (short)vs[i].ly;
                GFX_GetScreen(vs[i].sptr->charofs, vbp, v20, vs[i].lx, vs[i].ly);
            }
            if (vs[i].f_74)
            {
                if (vs[i].picflag != 3)
                {
                    GFX_LightBox(1, vbp - 1, v20 + 1, vs[i].lx, vs[i].ly);
                }
                else if (vs[i].item != -1)
                {
                    tex = (texture_t*)GLB_GetItem(vs[i].item);
                    GFX_ShadeShape(0, tex, vbp - 1, v20 + 1);
                }
            }
            SWD_PutField(a1, &vs[i]);
        }
        }
    return i;
}

void SWD_PutWin(int a1)
{
    static wdlg_t wdlg;
    swd_t *vs;
    texture_t *tex;
    int v20, v1c, vdi, v24, vc;
    vs = g_wins[a1].win;
    v20 = 8;
    v1c = vs->y + vs->f_70;
    vdi = vs->x - 8;
    v24 = vs->f_6c;
    vc = vs->y + 8;
    if (vs->display)
    {
        if (vs->f_74)
        {
            if (vs->f_44 == 3 && vs->f_40 != -1)
            {
                tex = (texture_t*)GLB_GetItem(vs->f_40);
                GFX_ShadeShape(0, tex, vdi, vc);
            }
            else
            {
                GFX_ShadeArea(0, vdi, vc, 8, vs->f_70 - 8);
                GFX_ShadeArea(0, vdi, v1c, v24, v20);
            }
        }
        switch (vs->f_44)
        {
        case 0:
            GFX_ColorBox(vs->x, vs->y, vs->f_6c, vs->f_70, vs->f_5c);
            if (vs->f_6c < 320 && vs->f_70 < 200)
                GFX_LightBox(1, vs->x, vs->y, vs->f_6c, vs->f_70);
            break;
        case 2:
            if (vs->f_40 != -1)
            {
                tex = (texture_t*)GLB_GetItem(vs->f_40);
                GFX_PutImage(tex, vs->x, vs->y, 0);
            }
            break;
        case 3:
            if (vs->f_40 != -1)
            {
                tex = (texture_t*)GLB_GetItem(vs->f_40);
                GFX_PutImage(tex, vs->x, vs->y, 1);
            }
            break;
        case 1:
            if (vs->f_40 != -1)
            {
                tex = (texture_t*)GLB_GetItem(vs->f_40);
                GFX_PutTexture(tex, vs->x, vs->y, vs->f_6c, vs->f_70);
                GFX_LightBox(1, vs->x, vs->y, vs->f_6c, vs->f_70);
            }
            break;
        case 4:
            if (vs->f_5c == 0)
            {
                GFX_ShadeArea(0, vs->x, vs->y, vs->f_6c, vs->f_70);
                GFX_LightBox(1, vs->x, vs->y, vs->f_6c, vs->f_70);
            }
            break;
        }
        if (vs->numflds)
            SWD_ShowAllFields(vs);
        if (winfuncs[a1])
        {
            SWD_GetObjAreaInfo(a1);
            wdlg.f_1c = obj_x;
            wdlg.f_20 = obj_y;
            wdlg.f_28 = obj_width;
            wdlg.f_24 = obj_height;
            wdlg.f_14 = vs->f_18;
            wdlg.f_18 = vs->f_1c;
            wdlg.f_0 = active_window;
            wdlg.field = active_field;
            winfuncs[a1](&wdlg);
        }
    }
}

int FUN_0002d1ac(void)
{
    if (keyboard[28])
        return 1;
    if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                       //Fixed ptr input
         return 1;
    return 0;
}

void SWD_Install(int a1)
{
    memset(g_wins, 0, sizeof(g_wins));
    if (a1)
    {
        movebuffer = (char*)malloc(4000 * 16);
        if (!movebuffer)
            EXIT_Error("SWD_Init() - DosMemAlloc");
    }
    else
        movebuffer = NULL;
    fldfuncs[0] = NULL;
    fldfuncs[1] = NULL;
    fldfuncs[2] = SWD_DoButton;
    fldfuncs[3] = SWD_FieldInput;
    fldfuncs[4] = SWD_DoButton;
    fldfuncs[5] = NULL;
    fldfuncs[6] = SWD_DoButton;
    fldfuncs[7] = NULL;
    fldfuncs[8] = NULL;
    fldfuncs[9] = NULL;
    fldfuncs[10] = NULL;
    fldfuncs[11] = NULL;
    fldfuncs[12] = NULL;
    fldfuncs[13] = NULL;
    fldfuncs[14] = NULL;
}

void SWD_End(void)
{
    memset(g_wins, 0, sizeof(g_wins));
    fldfuncs[0] = NULL;
    fldfuncs[1] = NULL;
    fldfuncs[2] = NULL;
    fldfuncs[3] = NULL;
    fldfuncs[4] = NULL;
    fldfuncs[5] = NULL;
    fldfuncs[6] = NULL;
    fldfuncs[7] = NULL;
    fldfuncs[8] = NULL;
    fldfuncs[9] = NULL;
    fldfuncs[10] = NULL;
    fldfuncs[11] = NULL;
    fldfuncs[12] = NULL;
    fldfuncs[13] = NULL;
    fldfuncs[14] = NULL;
}

swd_t* SWD_ReformatFieldData(swd_t* v1c, int a1)
{
    int fileLen = GLB_GetItemSize(a1);
    int len = sizeof(swd_t) + (v1c->numflds * sizeof(swdfield_t));
    int oldLen = sizeof(swd_t) + (v1c->numflds * sizeof(swdfield_32_t));
    int eof = fileLen - oldLen;

    swd_t* swdNewData = (swd_t*)calloc(1, len + eof);

    memcpy(swdNewData, v1c, sizeof(swd_t));
    memcpy((char*)swdNewData + len, (char*)v1c + oldLen, eof);

    swdfield_32_t* swdfield32 = (swdfield_32_t*)((char*)v1c + v1c->fldofs);
    swdfield_t* swdfield = (swdfield_t*)((char*)swdNewData + swdNewData->fldofs);

    for (size_t i = 0; i < v1c->numflds; i++)
    {
        swdfield[i].opt = swdfield32[i].f_0;
        swdfield[i].id = swdfield32[i].f_4;
        swdfield[i].f_8 = swdfield32[i].f_8;
        swdfield[i].f_c = swdfield32[i].f_c;
        swdfield[i].f_10 = swdfield32[i].f_10;
        swdfield[i].f_14 = swdfield32[i].f_14;
        swdfield[i].input_opt = swdfield32[i].f_18;
        swdfield[i].bstatus = swdfield32[i].f_1c;
        for (size_t j = 0; j < 16; j++)
        {
            swdfield[i].Name[j] = swdfield32[i].Name[j];
            swdfield[i].f_30[j] = swdfield32[i].f_30[j];
            swdfield[i].f_44[j] = swdfield32[i].f_44[j];
        }
        swdfield[i].item = swdfield32[i].f_40;
        swdfield[i].fontid = swdfield32[i].f_54;
        swdfield[i].fontbasecolor = swdfield32[i].f_58;
        swdfield[i].maxchars = swdfield32[i].f_5c;
        swdfield[i].picflag = swdfield32[i].f_60;
        swdfield[i].color = swdfield32[i].f_64;
        swdfield[i].lite = swdfield32[i].f_68;
        swdfield[i].mark = swdfield32[i].f_6c;
        swdfield[i].saveflag = swdfield32[i].f_70;
        swdfield[i].f_74 = swdfield32[i].f_74;
        swdfield[i].selectable = swdfield32[i].f_78;
        swdfield[i].x = swdfield32[i].f_7c;
        swdfield[i].y = swdfield32[i].f_80;
        swdfield[i].lx = swdfield32[i].f_84;
        swdfield[i].ly = swdfield32[i].f_88;
        swdfield[i].txtoff = swdfield32[i].f_8c;
        if (swdfield32[i].f_0 == 1 || swdfield32[i].f_0 == 2 || swdfield32[i].f_0 == 3 || swdfield32[i].f_0 == 6) {
            swdfield[i].txtoff += (v1c->numflds - i) * 4;
        }
    }
    //GLB_SetItemSize(a1, len + eof);
    //GLB_SetItemPointer(a1, (char*)swdNewData);
    //free(v1c);
    return swdNewData;
}

int SWD_InitWindow(int a1)
{
    swd_t *v1c;
    swdfield_t *vd;
    int i, j, vb;
    PTR_ResetJoyStick();
    old_win = -1;
    old_field = -1;
    kbactive = 0;
    highlight_flag = 0;
    if (lastfld)
    {
        lastfld->bstatus = 0;
        lastfld = NULL;
    }
    
    v1c = (swd_t*)GLB_LockItem(a1);
#if _MSC_VER
#if _WIN64
    v1c = SWD_ReformatFieldData(v1c, a1);
#endif
#endif
#if __GNUC__
#if __x86_64__
    v1c = SWD_ReformatFieldData(v1c, a1);
#endif
#endif
    vd = (swdfield_t*)((char*)v1c + v1c->fldofs);
    
    for (i = 0; i < 12; i++)
    {
        if (!g_wins[i].flag)
        {
            prev_window = active_window;
            g_wins[i].win = v1c;
            g_wins[i].flag = 1;
            g_wins[i].f_0 = a1;
            active_window = i;
            v1c->display = 1;
            active_field = g_wins[i].win->firstfld;
            if (!vd[active_field].selectable)
                active_field = SWD_GetFirstField();
            if (v1c->f_44)
            {
                v1c->f_40 = GLB_GetItemID(v1c->f_30);
                GLB_LockItem(v1c->f_40);
            }
            for (j = 0; j < v1c->numflds; j++)
            {
                if (vd[j].opt)
                {
                    if (vd[j].opt == 11)
                        g_wins[active_window].f_8 = 1;
                    switch (vd[j].opt)
                    {
                    case 2:
                    case 4:
                    case 5:
                    case 6:
                        if (usekb_flag && vd[j].selectable)
                            vd[j].f_c = 1;
                        else
                            vd[j].f_c = 0;
                        break;
                    case 3:
                        vd[j].f_c = 1;
                        break;
                    default:
                        vd[j].f_c = 0;
                        break;
                    }
                    vd[j].bstatus = 0;
                    vd[j].fontid = GLB_GetItemID(vd[j].f_44);
                    if (vd[j].fontid != -1)
                        GLB_LockItem(vd[j].fontid);
                    if (!vd[j].picflag)
                        vd[j].item = -1;
                    else
                        vd[j].item = GLB_GetItemID(vd[j].f_30);
                    if (vd[j].item != -1)
                        GLB_LockItem(vd[j].item);
                    vd[j].sptr = NULL;
                    if (vd[j].saveflag)
                    {
                        vb = vd[j].lx * vd[j].ly + 20;
                        if (vb < 0 || vb > 64000)
                            EXIT_Error("SWD Error: pic save to big...");
                        vd[j].sptr = (texture_t*)malloc(vb);
                        if (!vd[j].sptr)
                            EXIT_Error("SWD Error: out of memory");
                    }
                }
            }
            return i;
        }
    }
    return -1;
}

int SWD_InitMasterWindow(int a1)
{
    master_window = SWD_InitWindow(a1);
    return master_window;
}

void SWD_SetViewDrawHook(void (*a1)(void))
{
    viewdraw = a1;
}

void SWD_SetWinDrawFunc(int a1, void (*a2)(wdlg_t*))
{
    if (a2 && g_wins[a1].flag)
        winfuncs[a1] = a2;
}

void SWD_SetClearFlag(int a1)
{
    clearscreenflag = a1;
}

int SWD_ShowAllWindows(void)
{
    int i;
    if (active_window < 0)
        return 0;
    if (clearscreenflag && (master_window == -1 || viewdraw == NULL))
        memset(displaybuffer, 0, 64000);
    if (master_window != -1 && g_wins[master_window].flag)
        SWD_PutWin(master_window);
    if (viewdraw)
        viewdraw();
    for (i = 0; i < 12; i++)
    {
        if (g_wins[i].flag && i != active_window && i != master_window)
            SWD_PutWin(i);
    }
    if (movebuffer)
        memcpy(movebuffer, displaybuffer, 64000);
    if (active_window != -1 && active_window != master_window && g_wins[active_window].flag)
        SWD_PutWin(active_window);
    return 1;
}

void SWD_SetWindowPtr(int a1)
{
    swd_t* va;
    swdfield_t *fl;
    va = g_wins[a1].win;
    if (!ptractive || a1 == -1)
        return;

    if (active_field == -1 || !va->numflds)
    {
        if (!g_wins[a1].flag || va == NULL)
            return;
        PTR_SetPos(va->x + (va->f_6c>>1), va->y + (va->f_70>>1));
    }
    else
    {
        fl = (swdfield_t*)((char*)va + va->fldofs);
        fl += active_field;
        PTR_SetPos(fl->x + (fl->lx>>1), fl->y + (fl->ly>>1));
    }
}

void SWD_SetFieldPtr(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fl;
    va = g_wins[a1].win;
    if (!ptractive || a1 == -1)
        return;

    if (a2 == -1 || !va->numflds)
    {
        if (!g_wins[a1].flag || va == NULL)
            return;
        PTR_SetPos(va->x + (va->f_6c>>1), va->y + (va->f_70>>1));
    }
    else
    {
        fl = (swdfield_t*)((char*)va + va->fldofs);
        fl += a2;
        PTR_SetPos(fl->x + (fl->lx>>1), fl->y + (fl->ly>>1));
    }
}

void FUN_0002d7c8(int a1)
{
    if (!g_wins[a1].flag)
        EXIT_Error("SWD: SetActiveWindow #%u", a1);
    active_window = a1;
}

void SWD_SetActiveField(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fl;
    va = g_wins[a1].win;
    if (active_field != -1)
        lastfld = (swdfield_t*)((char*)va + va->fldofs) + active_field;
    fl = (swdfield_t*)((char*)va + va->fldofs);
    fl += a2;
    if (fl->f_c != 0)
        highlight_flag = 1;
    kbactive = fl->f_c != 0;
    active_field = a2;
}

void SWD_DestroyWindow(int a1)
{
    swd_t* va;
    swdfield_t *fl;
    int i, vd;
    va = g_wins[a1].win;
    PTR_ResetJoyStick();
    if (!g_wins[a1].flag)
        EXIT_Error("SWD: DestroyWindow %d", a1);
    fl = (swdfield_t*)((char*)va + va->fldofs);
    for (i = 0; i < va->numflds; i++)
    {
        if (fl[i].item != -1)
            GLB_FreeItem(fl[i].item);
        if (fl[i].fontid != -1)
            GLB_FreeItem(fl[i].fontid);
        if (fl[i].saveflag && fl[i].sptr)
            free(fl[i].sptr);
    }
    if (va->f_40)
        GLB_FreeItem(va->f_40);
    GLB_FreeItem(g_wins[a1].f_0);
    g_wins[a1].flag = 0;
    winfuncs[a1] = NULL;
    if (a1 == master_window)
        master_window = -1;
    kbactive = 0;
    highlight_flag = 0;
    lastfld = NULL;
    SWD_GetNextWindow();
    if (active_field != -1)
    {
        va = g_wins[active_window].win;
        fl = (swdfield_t*)((char*)va + va->fldofs);
        if (fl[active_field].f_c)
            kbactive = 1;
    }
    //if (g_wins[prev_window].f_4)
    if (prev_window >= 0) if (g_wins[prev_window].flag)
    {
        vd = prev_window;
        prev_window = active_window;
        active_window = vd;
        active_field = g_wins[active_window].win->firstfld;
    }
    if (active_window != -1)
        SWD_ShowAllWindows();
}

int FUN_0002d9d0(int a1, int a2)
{
    swd_t *va;
    int v18;
    int vb, vd;
    int i;

    va = g_wins[active_window].win;
    v18 = -1;
    vb = va->x + va->f_6c;
    vd = va->y + va->f_70;
    if (a1 > va->x && a1 < vb && a2 > va->y && a2 < vd)
        v18 = active_window;
    else
    {
        for (i = 0; i < 12; i++)
        {
            if (g_wins[i].flag == 1)
            {
                va = g_wins[i].win;
                vb = va->x + va->f_6c;
                vd = va->y + va->f_70;
                if (a1 > va->x && a1 < vb && a2 > va->y && a2 < vd)
                {
                    v18 = active_window;
                    break;
                }
            }
        }
    }
    return v18;
}

int FUN_0002daac(int a1, swd_t *a2, swdfield_t *a3)
{
    int v18;
    int v14;
    int vb;
    int i;
    int v24;
    int vs;
    int v1c;
    int v20;
    int vc;

    vb = 1;
    vc = 0;
    v18 = cur_mx;
    v14 = cur_my;
  
    for (i = 0; i < a2->numflds; i++)
    {
        v24 = a2->x + a3[i].x;
        vs = a2->y + a3[i].y;
        v1c = v24 + a3[i].lx + 1;
        v20 = vs + a3[i].ly + 1;
        if (v18 >= v24 && v18 <= v1c && vs <= v14 && v14 <= v20)
        {
            vb = 1;
            switch (a3[i].opt)
            {
            default:
                vb = 0;
                break;
            case 6:
                if (a3[i].selectable)
                {
                    active_field = i;
                    cur_act = 2;
                    cur_cmd = 15;
                    vc = 0;
                }
                else
                    vb = 0;
                break;
            case 2:
            case 3:
            case 4:
            case 5:
                active_field = i;
                cur_act = 1;
                cur_cmd = 10;
                vc = 0;
                break;
            case 10:
                cur_act = 1;
                cur_cmd = 11;
                vc = 0;
                break;
            case 11:
                cur_act = 1;
                cur_cmd = 12;
                vc = 0;
                break;
            }
            if (vb)
                break;
        }
    }
    if (vc)
    {
        while (mouseb1) {
        }
    }
    return vb;
}

int FUN_0002dbe4(wdlg_t *a1, swd_t *a2, swdfield_t *a3)
{
    int i;
    int vdi, vc;
    int v14, v18, v1c;
    int v20, v24;

    v14 = 0;
    v18 = cur_mx;
    v1c = cur_my;
    
    for (i = 0; i < a2->numflds; i++)
    {
        vdi = a2->x + a3[i].x;
        vc = a2->y + a3[i].y;
        v20 = vdi + a3[i].lx + 1;
        v24 = vc + a3[i].ly + 1;
        if (vdi <= v18 && v18 <= v20 && vc <= v1c && v1c <= v24)
        {
            switch (a3[i].opt)
            {
            case 11:
                v14 = 1;
                a1->viewactive = 1;
                a1->f_38 = a3[i].x;
                a1->f_3c = a3[i].y;
                a1->f_24 = a3[i].lx;
                a1->f_28 = a3[i].ly;
                a1->sfield = i;
                break;
            }
            if (v14)
                break;
        }
    }
    return v14;
}

void FUN_0002dcb8(void)
{
    int i, j;
    swd_t *vc;
    swdfield_t *fl;
    for (i = 0; i < 12; i++)
    {
        if (g_wins[i].flag)
        {
            vc = g_wins[i].win;
            fl = (swdfield_t*)((char*)vc + vc->fldofs);
            for (j = 0; j < vc->numflds; j++)
            {
                fl[j].bstatus = 0;
            }
        }
    }
}

void SWD_Dialog(wdlg_t *a1)
{
    int v28, i, v20, v24;
    swd_t *vc;
    swdfield_t *vcc, *v1c;
    //__disable();

    I_GetEvent();
    
    g_key = lastscan;
    lastscan = 0;
    g_ascii = lastascii;
    lastascii = 0;
    //__enable();

    v28 = 0;
    if (active_window == -1)
        return;

    vc = g_wins[active_window].win;
    vcc = (swdfield_t*)((char*)vc + vc->fldofs);
    v1c = vcc + active_field;
    cur_act = 0;
    cur_cmd = 0;
    if (highlight_flag)
    {
        highlight_flag = 0;
        if (lastfld)
        {
            lastfld->bstatus = 0;
            SWD_PutField(vc, lastfld);
            lastfld = NULL;
            v28 = 1;
        }
        if (kbactive)
        {
            v1c->bstatus = 1;
            SWD_PutField(vc, v1c);
            lastfld = v1c;
            v28 = 1;
        }
    }
    if (old_win != active_window)
    {
        FUN_0002dcb8();
        lastfld = NULL;
        cur_act = 2;
        highlight_flag = 1;
        cur_cmd = 0;
        if (v1c->f_c)
            kbactive = 1;
        else
            kbactive = 0;
    }
    old_win = active_window;
    a1->viewactive = 0;
    if (g_wins[active_window].f_8)
        FUN_0002dbe4(a1, vc, vcc);
    if (active_field == -1)
        return;

    if ((mouseb1 && !cur_act) || (AButton && !joy_ipt_MenuNew && !cur_act))                            //Fixed ptr input
    {
        old_field = active_field;
        if (FUN_0002daac(vc->f_48, vc, vcc))
        {
            if (old_win != active_window)
            {
                FUN_0002dcb8();
                lastfld = NULL;
                vc = g_wins[active_window].win;
                vcc = (swdfield_t*)((char*)vc + vc->fldofs);
                active_field = vc->firstfld;
                v1c = vcc + active_field;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            else if (old_field != active_field)
            {
                v1c = vcc + active_field;
                highlight_flag = 1;
            }
            if (v1c->f_c)
            {
                highlight_flag = 1;
                kbactive = 1;
            }
            else
            {
                kbactive = 0;
                highlight_flag = 1;
            }
        }
    }
    else
    {
        if (fldfuncs[v1c->opt] != NULL && cur_act == 0)
        {
            fldfuncs[v1c->opt](vc, v1c);
            for (i = 0; i < vc->numflds; i++)
            {
                if (vcc[i].f_8 && vcc[i].f_8 == g_key)
                {
                    if (!usekb_flag)
                        kbactive = 0;
                    active_field = i;
                    v1c = vcc + i;
                    if (lastfld)
                    {
                        lastfld->bstatus = 0;
                        SWD_PutField(vc, lastfld);
                        v28 = 1;
                        lastfld = NULL;
                    }
                    highlight_flag = 1;
                    cur_act = 1;
                    cur_cmd = 10;
                    lastfld = v1c;
                    break;
                }
            }
            if (cur_act && cur_cmd != 10 && !kbactive)
            {
                highlight_flag = 1;
                cur_act = 1;
                cur_cmd = 9;
                kbactive = 1;
            }
        }
    }
    old_field = active_field;
    a1->f_0 = active_window;
    a1->field = active_field;
    a1->f_14 = vc->f_18;
    a1->f_18 = vc->f_1c;
    a1->cur_act = cur_act;
    a1->cur_cmd = cur_cmd;
    a1->keypress = g_key;
    switch (cur_act)
    {
    case 1:
        a1->f_1c = v1c->saveflag;
        a1->f_20 = v1c->y;
        a1->f_28 = v1c->lx;
        a1->f_24 = v1c->ly;
        switch (cur_cmd)
        {
        case 1:
            SWD_GetDownField(vcc, vc->numflds);
            break;
        case 2:
            SWD_GetUpField(vcc, vc->numflds);
            break;
        case 3:
        case 5:
            SWD_GetNextField(vcc, vc->numflds);
            break;
        case 4:
        case 6:
            SWD_GetPrevField(vcc, vc->numflds);
            break;
        case 7:
            active_field = SWD_GetFirstField();
            break;
        case 8:
            active_field = SWD_GetLastField(vcc, vc->numflds);
            break;
        case 9:
            active_field = vc->firstfld;
            break;
        case 10:
            v1c->bstatus = 2;
            SWD_PutField(vc, v1c);
            v1c->mark ^= 1;
            if (lastfld && lastfld != v1c)
            {
                lastfld->bstatus = 0;
                SWD_PutField(vc, lastfld);
                lastfld = NULL;
            }
            GFX_DisplayUpdate();
            while (FUN_0002d1ac())
            {
                I_GetEvent();
            }
            if (kbactive || v1c->f_c)
                v1c->bstatus = 1;
            else
                v1c->bstatus = 0;
            SWD_PutField(vc, v1c);
            v28 = 1;
            break;
        }
        break;
    case 2:
        a1->f_1c = vc->x;
        a1->f_20 = vc->y;
        a1->f_28 = vc->f_6c;
        a1->f_24 = vc->f_70;
        switch (cur_cmd)
        {
        case 14:
            if (!vc->f_48)
            {
                SWD_GetNextWindow();
                if (active_window == master_window)
                    SWD_GetNextWindow();
                active_field = SWD_GetFirstField();
                if (lastfld)
                    lastfld->bstatus = 0;
                SWD_ShowAllWindows();
                v28 = 1;
                break;
            }
            break;
        case 15:
            if (movebuffer)
            {
                v1c->bstatus = 2;
                SWD_PutField(vc, v1c);
                if (lastfld && lastfld != v1c)
                {
                    lastfld->bstatus = 0;
                    SWD_PutField(vc, lastfld);
                    lastfld = NULL;
                }
                GFX_DisplayUpdate();
                v24 = cur_mx - vc->x;
                v20 = cur_my - vc->y;
                keyboard[28] = 0;
                lastscan = 0;
                SWD_ShowAllWindows();
                while (mouseb1)
                {
                    vc->x = cur_mx - v24;
                    vc->y = cur_my - v20;
                    GFX_MarkUpdate(0, 0, 320, 200);
                    memcpy(displaybuffer, movebuffer, 64000);
                    SWD_PutWin(active_window);
                    GFX_DisplayUpdate();
                }
                v1c->bstatus = 0;
                v28 = 1;
                SWD_PutField(vc, v1c);
                break;
            }
            break;
        case 16:
            FUN_0002dcb8();
            lastfld = NULL;
            kbactive = 0;
            break;
        case 17:
            FUN_0002dcb8();
            lastfld = NULL;
            kbactive = 0;
            break;
        }
        break;
    case 4:
        SWD_ShowAllWindows();
        v28 = 1;
        break;
    case 5:
        v28 = 1;
        break;
    }
    if (old_field != active_field && active_field >= 0 && kbactive)
        highlight_flag = 1;
    if (v28)
        GFX_DisplayUpdate();
}

void SWD_SetWindowLock(int a1, int a2)
{
    if (g_wins[a1].flag == 1)
        g_wins[a1].win->f_48 = a2;
}

int SWD_SetWindowXY(int a1, int a2, int a3)
{
    swd_t *va;
    va = g_wins[a1].win;
    va->x = a2;
    va->x = a3;
    return va->f_58;
}

int SWD_GetWindowXYL(int a1, int *a2, int *a3, int *a4, int *a5)
{
    swd_t* va;
    va = g_wins[a1].win;
    if (a2)
        *a2 = va->x;
    if (a3)
        *a3 = va->y;
    if (a4)
        *a4 = va->f_6c;
    if (a5)
        *a5 = va->f_70;
    return va->f_58;
}

int SWD_GetFieldText(int a1, int a2, char *a3)
{
    swd_t* va;
    swdfield_t *fld;
    char *t;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    t = (char*)fld + fld->txtoff;
    memcpy(a3, t, fld->maxchars);
    return fld->maxchars;
}

int SWD_SetFieldText(int a1, int a2, const char *a3)
{
    swd_t* va;
    swdfield_t *fld;
    char *t;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    t = (char*)fld + fld->txtoff;
    if (a3)
    {
        t[fld->maxchars - 1] = 0;
        memcpy(t, a3, fld->maxchars - 1);             
    }
    else
        *t = 0;
    return fld->maxchars;
}

int SWD_GetFieldValue(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    char *t;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    t = (char*)fld + fld->txtoff;
    return atoi(t);
}

int SWD_SetFieldValue(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    char *t;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    t = (char*)fld + fld->txtoff;
    sprintf(t, "%d", a3);
    return atoi(t);
}

void SWD_SetFieldSelect(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    fld->selectable = a3;
}

int SWD_GetFieldMark(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    return fld->mark;
}

void SWD_SetFieldMark(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    fld->mark = a3;
}

int SWD_GetFieldInputOpt(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    return fld->input_opt;
}

int SWD_SetFieldInputOpt(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    int o;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    o = fld->input_opt;
    fld->input_opt = a3;
    return o;
}

void SWD_SetFieldItem(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    if (a3 != -1)
    {
        if (fld->item != -1)
            GLB_FreeItem(fld->item);
        fld->item = a3;
        GLB_LockItem(a3);
    }
    else
        fld->item = -1;
}

int SWD_GetFieldItem(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    return fld->item;
}

void SWD_SetFieldName(int a1, int a2, const char *a3)
{
    swd_t* va;
    swdfield_t *fld;
    int it;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    it = GLB_GetItemID(a3);
    if (it != -1)
    {
        if (fld->item != -1)
            GLB_FreeItem(fld->item);
        memcpy(fld->f_30, a3, 16);
        fld->item = it;
        GLB_LockItem(it);
    }
}

int SWD_GetFieldItemName(int a1, int a2, char *a3)
{
    swd_t* va;
    swdfield_t *fld;
    int vc;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    vc = 10;
    memcpy(a3, fld->f_30, vc);
    return vc;
}

int SWD_SetWindowID(int a1, int a2)
{
    swd_t* va;
    int o;
    va = g_wins[a1].win;
    o = va->f_18;
    va->f_18 = o;
    return o;
}

int SWD_GetWindowID(int a1)
{
    swd_t* va;
    va = g_wins[a1].win;
    return va->f_18;
}

int FUN_0002e84c(int a1, int a2)
{
    swd_t* va;
    va = g_wins[a1].win;
    va->display = a2;
    SWD_GetNextWindow();
    return va->f_18;
}

int SWD_SetWindowType(int a1, int a2)
{
    swd_t* va;
    int o;
    va = g_wins[a1].win;
    o = va->f_1c;
    va->f_1c = o;
    return o;
}

int SWD_GetWindowType(int a1)
{
    swd_t* va;
    va = g_wins[a1].win;
    return va->f_1c;
}

int SWD_GetFieldXYL(int a1, int a2, int* a3, int* a4, int* a5, int* a6)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].win;
    fld = (swdfield_t*)((char*)va + va->fldofs) + a2;
    if (a3)
        *a3 = va->x + fld->x;
    if (a4)
        *a4 = va->y + fld->y;
    if (a5)
        *a5 = fld->lx;
    if (a6)
        *a6 = fld->ly;
    return fld->lx;
}
