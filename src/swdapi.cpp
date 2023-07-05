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
        if (KBD_Key(SC_ALT))
        {
            cur_act = S_WIN_COMMAND;
            cur_cmd = W_NEXT;
            while (KBD_Key(SC_TAB)) {
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
        if (KBD_Key(SC_ALT))
        {
            while (KBD_Key(SC_TAB)) {
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
        if (KBD_Key(SC_Y) && KBD_Key(SC_CTRL))
        {
            curpos = 0;
            flag = 1;
            *wrkbuf = 0;
        }
        else if (!KBD_Key(SC_ALT) && !KBD_Key(SC_CTRL) && g_key > 0 && curfld->maxchars-1 > curpos)
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
  SWD_GetUpField() - Gets the closest up selectable field
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
  SWD_GetDownField() - Gets the closest down selectable field
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
  SWD_GetLeftField() - Gets the closest left selectable field
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

/*------------------------------------------------------------------------
  SWD_ShowAllFields() - Displays all Fields in a window
  ------------------------------------------------------------------------*/
int                        // RETURN: number of fields displayed
SWD_ShowAllFields(
    swd_t *inptr           // INPUT : pointer to window data
)
{
    int loop, fy, fx;
    texture_t *picdata;
    swdfield_t *fld = (swdfield_t*)((char*)inptr + inptr->fldofs);
    
    for (loop = 0; loop < inptr->numflds; loop++)
    {
        if (fld[loop].opt)
        {
            fx = inptr->x + fld[loop].x;
            fy = inptr->y + fld[loop].y;
            
            if (fld[loop].saveflag && fld[loop].sptr)
            {
                fld[loop].sptr->width = (short)fld[loop].lx;
                fld[loop].sptr->height = (short)fld[loop].ly;
                GFX_GetScreen(fld[loop].sptr->charofs, fx, fy, fld[loop].lx, fld[loop].ly);
            }
            
            if (fld[loop].shadow)
            {
                if (fld[loop].picflag != SEE_THRU)
                {
                    GFX_LightBox(UPPER_RIGHT, fx - 1, fy + 1, fld[loop].lx, fld[loop].ly);
                }
                else if (fld[loop].item != -1)
                {
                    picdata = (texture_t*)GLB_GetItem(fld[loop].item);
                    GFX_ShadeShape(DARK, picdata, fx - 1, fy + 1);
                }
            }
            
            SWD_PutField(inptr, &fld[loop]);
        }
    }
    
    return loop;
}

/*------------------------------------------------------------------------
  SWD_PutWin() - Displays a single window
  ------------------------------------------------------------------------*/
void 
SWD_PutWin(
    int handle            // INPUT : number/handle of window
)
{
    static wdlg_t wdlg;
    swd_t *cwin;
    texture_t *pic;
    int ly, y2, x, lx, y;
    cwin = g_wins[handle].win;
    ly = 8;
    y2 = cwin->y + cwin->ly;
    x = cwin->x - 8;
    lx = cwin->lx;
    y = cwin->y + 8;
    
    if (cwin->display)
    {
        if (cwin->shadow)
        {
            if (cwin->picflag == SEE_THRU && cwin->item != -1)
            {
                pic = (texture_t*)GLB_GetItem(cwin->item);
                GFX_ShadeShape(DARK, pic, x, y);
            }
            else
            {
                GFX_ShadeArea(DARK, x, y, 8, cwin->ly - 8);
                GFX_ShadeArea(DARK, x, y2, lx, ly);
            }
        }
        
        switch (cwin->picflag)
        {
        case FILL:
            GFX_ColorBox(cwin->x, cwin->y, cwin->lx, cwin->ly, cwin->color);
            
            if (cwin->lx < 320 && cwin->ly < 200)
                GFX_LightBox(UPPER_RIGHT, cwin->x, cwin->y, cwin->lx, cwin->ly);
            break;
        
        case PICTURE:
            if (cwin->item != -1)
            {
                pic = (texture_t*)GLB_GetItem(cwin->item);
                GFX_PutImage(pic, cwin->x, cwin->y, 0);
            }
            break;
        
        case SEE_THRU:
            if (cwin->item != -1)
            {
                pic = (texture_t*)GLB_GetItem(cwin->item);
                GFX_PutImage(pic, cwin->x, cwin->y, 1);
            }
            break;
        
        case TEXTURE:
            if (cwin->item != -1)
            {
                pic = (texture_t*)GLB_GetItem(cwin->item);
                GFX_PutTexture(pic, cwin->x, cwin->y, cwin->lx, cwin->ly);
                GFX_LightBox(UPPER_RIGHT, cwin->x, cwin->y, cwin->lx, cwin->ly);
            }
            break;
        
        case INVISABLE:
            if (cwin->color == 0)
            {
                GFX_ShadeArea(DARK, cwin->x, cwin->y, cwin->lx, cwin->ly);
                GFX_LightBox(UPPER_RIGHT, cwin->x, cwin->y, cwin->lx, cwin->ly);
            }
            break;
        }
        
        if (cwin->numflds)
            SWD_ShowAllFields(cwin);
        
        if (winfuncs[handle])
        {
            SWD_GetObjAreaInfo(handle);
            wdlg.x = obj_x;
            wdlg.y = obj_y;
            wdlg.width = obj_width;
            wdlg.height = obj_height;
            wdlg.id = cwin->id;
            wdlg.type = cwin->type;
            wdlg.window = active_window;
            wdlg.field = active_field;
            winfuncs[handle](&wdlg);
        }
    }
}

/*------------------------------------------------------------------------
   SWD_IsButtonDown () - returns TRUE if any SWD Buttons are down
  ------------------------------------------------------------------------*/
int 
SWD_IsButtonDown(
    void
)
{
    if (KBD_Key(SC_ENTER))
        return 1;
    
    if ((mouseb1) || (AButton && !joy_ipt_MenuNew))                       
         return 1;
    
    return 0;
}

/***************************************************************************
 SWD_Install() - Initializes Window system
 ***************************************************************************/
void 
SWD_Install(
    int moveflag           // INPUT : Use Move Window feature ( 64k )
)
{
    memset(g_wins, 0, sizeof(g_wins));
    
    if (moveflag)
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

/***************************************************************************
   SWD_End () Frees up resources used by SWD System
 ***************************************************************************/
void 
SWD_End(
    void
)
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

/***************************************************************************
   SWD_ReformatFieldData () Reformat field data for 64-bit compatibility
 ***************************************************************************/
swd_t* 
SWD_ReformatFieldData(
    swd_t* header, 
    int handle
)
{
    int fileLen = GLB_GetItemSize(handle);
    int len = sizeof(swd_t) + (header->numflds * sizeof(swdfield_t));
    int oldLen = sizeof(swd_t) + (header->numflds * sizeof(swdfield_32_t));
    int eof = fileLen - oldLen;

    swd_t* swdNewData = (swd_t*)calloc(1, len + eof);

    memcpy(swdNewData, header, sizeof(swd_t));
    memcpy((char*)swdNewData + len, (char*)header + oldLen, eof);

    swdfield_32_t* swdfield32 = (swdfield_32_t*)((char*)header + header->fldofs);
    swdfield_t* swdfield = (swdfield_t*)((char*)swdNewData + swdNewData->fldofs);

    for (size_t loop = 0; loop < header->numflds; loop++)
    {
        swdfield[loop].opt = swdfield32[loop].opt;
        swdfield[loop].id = swdfield32[loop].id;
        swdfield[loop].hotkey = swdfield32[loop].hotkey;
        swdfield[loop].kbflag = swdfield32[loop].kbflag;
        swdfield[loop].opt3 = swdfield32[loop].opt3;
        swdfield[loop].opt4 = swdfield32[loop].opt4;
        swdfield[loop].input_opt = swdfield32[loop].input_opt;
        swdfield[loop].bstatus = swdfield32[loop].bstatus;
        
        for (size_t i = 0; i < 16; i++)
        {
            swdfield[loop].name[i] = swdfield32[loop].name[i];
            swdfield[loop].item_name[i] = swdfield32[loop].item_name[i];
            swdfield[loop].font_name[i] = swdfield32[loop].font_name[i];
        }
        
        swdfield[loop].item = swdfield32[loop].item;
        swdfield[loop].fontid = swdfield32[loop].fontid;
        swdfield[loop].fontbasecolor = swdfield32[loop].fontbasecolor;
        swdfield[loop].maxchars = swdfield32[loop].maxchars;
        swdfield[loop].picflag = swdfield32[loop].picflag;
        swdfield[loop].color = swdfield32[loop].color;
        swdfield[loop].lite = swdfield32[loop].lite;
        swdfield[loop].mark = swdfield32[loop].mark;
        swdfield[loop].saveflag = swdfield32[loop].saveflag;
        swdfield[loop].shadow = swdfield32[loop].shadow;
        swdfield[loop].selectable = swdfield32[loop].selectable;
        swdfield[loop].x = swdfield32[loop].x;
        swdfield[loop].y = swdfield32[loop].y;
        swdfield[loop].lx = swdfield32[loop].lx;
        swdfield[loop].ly = swdfield32[loop].ly;
        swdfield[loop].txtoff = swdfield32[loop].txtoff;
        
        if (swdfield32[loop].opt == FLD_TEXT || swdfield32[loop].opt == FLD_BUTTON || swdfield32[loop].opt == FLD_INPUT || swdfield32[loop].opt == FLD_DRAGBAR) {
            swdfield[loop].txtoff += (header->numflds - loop) * 4;
        }
    }
    
    //GLB_SetItemSize(a1, len + eof);
    //GLB_SetItemPointer(a1, (char*)swdNewData);
    //free(v1c);
    
    return swdNewData;
}

/***************************************************************************
 SWD_InitWindow() - Adds window to list and initializes
 ***************************************************************************/
int                           // RETURN: handle to window
SWD_InitWindow(
    int handle                // INPUT : GLB Item Number
)
{
    swd_t *header;
    swdfield_t *curfld;
    int rec_num, loop, pic_size;
    
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
    
    header = (swd_t*)GLB_LockItem(handle);

#if _MSC_VER
#if _WIN64
    header = SWD_ReformatFieldData(header, handle);
#endif // _WIN64
#endif // _MSC_VER
#if __GNUC__
#if __x86_64__
    header = SWD_ReformatFieldData(header, handle);
#endif // __x86_64__
#if __aarch64__
    header = SWD_ReformatFieldData(header, handle);
#endif // __aarch64__
#endif // __GNUC__
    
    curfld = (swdfield_t*)((char*)header + header->fldofs);
    
    for (rec_num = 0; rec_num < MAX_WINDOWS; rec_num++)
    {
        if (!g_wins[rec_num].flag)
        {
            prev_window = active_window;
            g_wins[rec_num].win = header;
            g_wins[rec_num].flag = 1;
            g_wins[rec_num].gitem = handle;
            active_window = rec_num;
            header->display = 1;
            active_field = g_wins[rec_num].win->firstfld;
            
            if (!curfld[active_field].selectable)
                active_field = SWD_GetFirstField();
            
            if (header->picflag)
            {
                header->item = GLB_GetItemID(header->item_name);
                GLB_LockItem(header->item);
            }
            
            for (loop = 0; loop < header->numflds; loop++)
            {
                if (curfld[loop].opt)
                {
                    if (curfld[loop].opt == FLD_VIEWAREA)
                        g_wins[active_window].viewflag = 1;
                    
                    switch (curfld[loop].opt)
                    {
                    case FLD_BUTTON:
                    case FLD_MARK:
                    case FLD_CLOSE:
                    case FLD_DRAGBAR:
                        if (usekb_flag && curfld[loop].selectable)
                            curfld[loop].kbflag = 1;
                        else
                            curfld[loop].kbflag = 0;
                        break;
                    
                    case FLD_INPUT:
                        curfld[loop].kbflag = 1;
                        break;
                    
                    default:
                        curfld[loop].kbflag = 0;
                        break;
                    }
                    
                    curfld[loop].bstatus = NORMAL;
                    curfld[loop].fontid = GLB_GetItemID(curfld[loop].font_name);
                    
                    if (curfld[loop].fontid != -1)
                        GLB_LockItem(curfld[loop].fontid);
                    
                    if (!curfld[loop].picflag)
                        curfld[loop].item = -1;
                    else
                        curfld[loop].item = GLB_GetItemID(curfld[loop].item_name);
                    
                    if (curfld[loop].item != -1)
                        GLB_LockItem(curfld[loop].item);
                    
                    curfld[loop].sptr = NULL;
                    
                    if (curfld[loop].saveflag)
                    {
                        pic_size = curfld[loop].lx * curfld[loop].ly + 20;
                        
                        if (pic_size < 0 || pic_size > 64000)
                            EXIT_Error("SWD Error: pic save to big...");
                        
                        curfld[loop].sptr = (texture_t*)malloc(pic_size);
                        
                        if (!curfld[loop].sptr)
                            EXIT_Error("SWD Error: out of memory");
                    }
                }
            }
            
            return rec_num;
        }
    }
    
    return -1;
}

/***************************************************************************
   SWD_InitMasterWindow () - Inits the Master Window ( must be full screen )
 ***************************************************************************/
int 
SWD_InitMasterWindow(
    int handle              // INPUT : GLB Item Number    
)
{
    master_window = SWD_InitWindow(handle);
    
    return master_window;
}

/***************************************************************************
   SWD_SetViewDrawHook () Sets Function to draw after the master window
 ***************************************************************************/
void 
SWD_SetViewDrawHook(
    void (*func)(void)      // INPUT : pointer to function
)
{
    viewdraw = func;
}

/***************************************************************************
   SWD_SetWinDrawFunc () - Function called after window is drawn
 ***************************************************************************/
void 
SWD_SetWinDrawFunc(
    int handle,             // INPUT :handle of window
    void (*infunc)(wdlg_t*) // INPUT :pointer to function
)
{
    if (infunc && g_wins[handle].flag)
        winfuncs[handle] = infunc;
}

/***************************************************************************
SWD_SetClearFlag() - Turns ON/OFF memsetting of display buffer in showallwins
 ***************************************************************************/
void 
SWD_SetClearFlag(
    int inflag
)
{
    clearscreenflag = inflag;
}

/***************************************************************************
 SWD_ShowAllWindows()- Diplays all windows.. puts active window on top
 ***************************************************************************/
int                           // RETURN : TRUE = OK, FALSE = Error
SWD_ShowAllWindows(
    void
)
{
    int loop;
    
    if (active_window < 0)
        return 0;
    
    if (clearscreenflag && (master_window == -1 || viewdraw == NULL))
        memset(displaybuffer, 0, 64000);
    
    if (master_window != -1 && g_wins[master_window].flag)
        SWD_PutWin(master_window);
    
    if (viewdraw)
        viewdraw();
    
    for (loop = 0; loop < MAX_WINDOWS; loop++)
    {
        if (g_wins[loop].flag && loop != active_window && loop != master_window)
            SWD_PutWin(loop);
    }
    
    if (movebuffer)
        memcpy(movebuffer, displaybuffer, 64000);
    
    if (active_window != -1 && active_window != master_window && g_wins[active_window].flag)
        SWD_PutWin(active_window);
    
    return 1;
}

/***************************************************************************
SWD_SetWindowPtr() - Sets Pointer to center of active field
 ***************************************************************************/
void 
SWD_SetWindowPtr(
    int handle             // INPUT : number/handle of window
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    if (!ptractive || handle == -1)
        return;

    if (active_field == -1 || !curwin->numflds)
    {
        if (!g_wins[handle].flag || curwin == NULL)
            return;
        
        PTR_SetPos(curwin->x + (curwin->lx>>1), curwin->y + (curwin->ly>>1));
    }
    else
    {
        curfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
        curfld += active_field;
        
        PTR_SetPos(curfld->x + (curfld->lx>>1), curfld->y + (curfld->ly>>1));
    }
}

/***************************************************************************
SWD_SetFieldPtr () - Sets Pointer on a field
 ***************************************************************************/
void 
SWD_SetFieldPtr(
    int handle,            // INPUT : number/handle of window
    int field              // INPUT : field
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    if (!ptractive || handle == -1)
        return;

    if (field == -1 || !curwin->numflds)
    {
        if (!g_wins[handle].flag || curwin == NULL)
            return;
        
        PTR_SetPos(curwin->x + (curwin->lx>>1), curwin->y + (curwin->ly>>1));
    }
    else
    {
        curfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
        curfld += field;
        
        PTR_SetPos(curfld->x + (curfld->lx>>1), curfld->y + (curfld->ly>>1));
    }
}

/***************************************************************************
 SWD_SetActiveWindow() - Sets the current working window
 ***************************************************************************/
void 
SWD_SetActiveWindow(
    int handle            // INPUT : number/handle of window
)
{
    if (!g_wins[handle].flag)
        EXIT_Error("SWD: SetActiveWindow #%u", handle);
    
    active_window = handle;
}

/***************************************************************************
 SWD_SetActiveField() - Sets the current working field
 ***************************************************************************/
void 
SWD_SetActiveField(
    int handle,            // INPUT : handle of window
    int field_id           // INPUT : number/handle of field
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    if (active_field != -1)
        lastfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + active_field;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
    curfld += field_id;
    
    if (curfld->kbflag != 0)
        highlight_flag = 1;
    
    kbactive = curfld->kbflag != 0;
    active_field = field_id;
}

/***************************************************************************
 SWD_DestroyWindow() - removes a window from SWD system
 ***************************************************************************/
void 
SWD_DestroyWindow(
    int handle            // INPUT : handle of window
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    int loop, hold;
    curwin = g_wins[handle].win;
    
    PTR_ResetJoyStick();
    
    if (!g_wins[handle].flag)
        EXIT_Error("SWD: DestroyWindow %d", handle);
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
    
    for (loop = 0; loop < curwin->numflds; loop++)
    {
        if (curfld[loop].item != -1)
            GLB_FreeItem(curfld[loop].item);
        
        if (curfld[loop].fontid != -1)
            GLB_FreeItem(curfld[loop].fontid);
        
        if (curfld[loop].saveflag && curfld[loop].sptr)
            free(curfld[loop].sptr);
    }
    
    if (curwin->item)
        GLB_FreeItem(curwin->item);
    
    GLB_FreeItem(g_wins[handle].gitem);
    
    g_wins[handle].flag = 0;
    winfuncs[handle] = NULL;
    
    if (handle == master_window)
        master_window = -1;
    
    kbactive = 0;
    highlight_flag = 0;
    lastfld = NULL;
    
    SWD_GetNextWindow();
    
    if (active_field != -1)
    {
        curwin = g_wins[active_window].win;
        curfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
        
        if (curfld[active_field].kbflag)
            kbactive = 1;
    }
    
    if (prev_window >= 0) if (g_wins[prev_window].flag)
    {
        hold = prev_window;
        prev_window = active_window;
        active_window = hold;
        active_field = g_wins[active_window].win->firstfld;
    }
    
    if (active_window != -1)
        SWD_ShowAllWindows();
}

/*------------------------------------------------------------------------
   SWD_FindWindow() - finds window at x, y pos else returns EMPTY
  ------------------------------------------------------------------------*/
int 
SWD_FindWindow(
    int x,                 // INPUT : x position
    int y                  // INPUT : y position
)
{
    swd_t *curwin;
    int rval;
    int x2, y2;
    int loop;

    curwin = g_wins[active_window].win;
    rval = -1;
    x2 = curwin->x + curwin->lx;
    y2 = curwin->y + curwin->ly;
    
    if (x > curwin->x && x < x2 && y > curwin->y && y < y2)
        rval = active_window;
    else
    {
        for (loop = 0; loop < MAX_WINDOWS; loop++)
        {
            if (g_wins[loop].flag == 1)
            {
                curwin = g_wins[loop].win;
                x2 = curwin->x + curwin->lx;
                y2 = curwin->y + curwin->ly;
                
                if (x > curwin->x && x < x2 && y > curwin->y && y < y2)
                {
                    rval = loop;
                    break;
                }
            }
        }
    }
    
    return rval;
}

/*------------------------------------------------------------------------
   SWD_CheckMouse () does mouse stuff and returns SWD_XXX code
  ------------------------------------------------------------------------*/
int 
SWD_CheckMouse(
    int a1, 
    swd_t *curwin,         // INPUT : pointer to current window
    swdfield_t *curfld     // INPUT : pointer to current field
)
{
    int px;
    int py;
    int flag;
    int loop;
    int x1;
    int y1;
    int x2;
    int y2;
    int mflag;

    flag = 1;
    mflag = 0;
    px = cur_mx;
    py = cur_my;
  
    for (loop = 0; loop < curwin->numflds; loop++)
    {
        x1 = curwin->x + curfld[loop].x;
        y1 = curwin->y + curfld[loop].y;
        x2 = x1 + curfld[loop].lx + 1;
        y2 = y1 + curfld[loop].ly + 1;
        
        if (px >= x1 && px <= x2 && y1 <= py && py <= y2)
        {
            flag = 1;
            switch (curfld[loop].opt)
            {
            default:
                flag = 0;
                break;
            
            case FLD_DRAGBAR:
                if (curfld[loop].selectable)
                {
                    active_field = loop;
                    cur_act = S_WIN_COMMAND;
                    cur_cmd = W_MOVE;
                    mflag = 0;
                }
                else
                    flag = 0;
                break;
            
            case FLD_BUTTON:
            case FLD_INPUT:
            case FLD_MARK:
            case FLD_CLOSE:
                active_field = loop;
                cur_act = S_FLD_COMMAND;
                cur_cmd = F_SELECT;
                mflag = 0;
                break;
            
            case FLD_OBJAREA:
                cur_act = S_FLD_COMMAND;
                cur_cmd = F_OBJ_AREA;
                mflag = 0;
                break;
            
            case FLD_VIEWAREA:
                cur_act = S_FLD_COMMAND;
                cur_cmd = F_VIEW_AREA;
                mflag = 0;
                break;
            }
            
            if (flag)
                break;
        }
    }
    
    if (mflag)
    {
        while (mouseb1) {
        }
    }
    
    return flag;
}

/*------------------------------------------------------------------------
   SWD_CheckViewArea ()
  ------------------------------------------------------------------------*/
int 
SWD_CheckViewArea(
    wdlg_t *dlg,             // INPUT : pointer to DLG window messages 
    swd_t *curwin,           // INPUT : pointer to current window 
    swdfield_t *curfld       // INPUT : pointer to current field
)
{
    int loop;
    int x1, y1;
    int flag, px, py;
    int x2, y2;

    flag = 0;
    px = cur_mx;
    py = cur_my;
    
    for (loop = 0; loop < curwin->numflds; loop++)
    {
        x1 = curwin->x + curfld[loop].x;
        y1 = curwin->y + curfld[loop].y;
        x2 = x1 + curfld[loop].lx + 1;
        y2 = y1 + curfld[loop].ly + 1;
        
        if (x1 <= px && px <= x2 && y1 <= py && py <= y2)
        {
            switch (curfld[loop].opt)
            {
            case FLD_VIEWAREA:
                flag = 1;
                dlg->viewactive = 1;
                dlg->sx = curfld[loop].x;
                dlg->sy = curfld[loop].y;
                dlg->height = curfld[loop].lx;
                dlg->width = curfld[loop].ly;
                dlg->sfield = loop;
                break;
            }
            
            if (flag)
                break;
        }
    }
    
    return flag;
}

/*------------------------------------------------------------------------
   SWD_ClearAllButtons () Clears all buttons in all windows to NORMAL
  ------------------------------------------------------------------------*/
void 
SWD_ClearAllButtons(
    void
)
{
    int wloop, loop;
    swd_t *curwin;
    swdfield_t *curfld;
    
    for (wloop = 0; wloop < MAX_WINDOWS; wloop++)
    {
        if (g_wins[wloop].flag)
        {
            curwin = g_wins[wloop].win;
            curfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
            
            for (loop = 0; loop < curwin->numflds; loop++)
            {
                curfld[loop].bstatus = NORMAL;
            }
        }
    }
}

/***************************************************************************
   SWD_Dialog () - performs all window in/out/display/move stuff
 ***************************************************************************/
void 
SWD_Dialog(
    wdlg_t *swd_dlg       // OUTPUT: pointer to info structure
)
{
    int update, loop, sy, sx;
    swd_t *curwin;
    swdfield_t *firstfld, *curfld;
    
    //__disable();
    I_GetEvent();
    g_key = lastscan;
    lastscan = SC_NONE;
    g_ascii = lastascii;
    lastascii = SC_NONE;
    //__enable();

    update = 0;
    
    if (active_window == -1)
        return;

    curwin = g_wins[active_window].win;
    firstfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
    curfld = firstfld + active_field;
    
    cur_act = S_IDLE;
    cur_cmd = C_IDLE;
    
    if (highlight_flag)
    {
        highlight_flag = 0;
        if (lastfld)
        {
            lastfld->bstatus = NORMAL;
            SWD_PutField(curwin, lastfld);
            lastfld = NULL;
            update = 1;
        }
        
        if (kbactive)
        {
            curfld->bstatus = UP;
            SWD_PutField(curwin, curfld);
            lastfld = curfld;
            update = 1;
        }
    }
    
    if (old_win != active_window)
    {
        SWD_ClearAllButtons();
        lastfld = NULL;
        cur_act = S_WIN_COMMAND;
        highlight_flag = 1;
        cur_cmd = C_IDLE;
        
        if (curfld->kbflag)
            kbactive = 1;
        else
            kbactive = 0;
    }
    
    old_win = active_window;
    
    swd_dlg->viewactive = 0;
    
    if (g_wins[active_window].viewflag)
        SWD_CheckViewArea(swd_dlg, curwin, firstfld);
    
    if (active_field == -1)
        return;

    if ((mouseb1 && !cur_act) || (AButton && !joy_ipt_MenuNew && !cur_act))                            
    {
        old_field = active_field;
        
        if (SWD_CheckMouse(curwin->lock, curwin, firstfld))
        {
            if (old_win != active_window)
            {
                SWD_ClearAllButtons();
                lastfld = NULL;
                curwin = g_wins[active_window].win;
                firstfld = (swdfield_t*)((char*)curwin + curwin->fldofs);
                active_field = curwin->firstfld;
                curfld = firstfld + active_field;
                SWD_ShowAllWindows();
                GFX_DisplayUpdate();
            }
            else if (old_field != active_field)
            {
                curfld = firstfld + active_field;
                highlight_flag = 1;
            }
            
            if (curfld->kbflag)
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
        if (fldfuncs[curfld->opt] != NULL && cur_act == S_IDLE)
        {
            fldfuncs[curfld->opt](curwin, curfld);
            
            for (loop = 0; loop < curwin->numflds; loop++)
            {
                if (firstfld[loop].hotkey && firstfld[loop].hotkey == g_key)
                {
                    if (!usekb_flag)
                        kbactive = 0;
                    
                    active_field = loop;
                    curfld = firstfld + loop;
                    
                    if (lastfld)
                    {
                        lastfld->bstatus = NORMAL;
                        SWD_PutField(curwin, lastfld);
                        update = 1;
                        lastfld = NULL;
                    }
                    
                    highlight_flag = 1;
                    cur_act = S_FLD_COMMAND;
                    cur_cmd = F_SELECT;
                    lastfld = curfld;
                    break;
                }
            }
            if (cur_act && cur_cmd != F_SELECT && !kbactive)
            {
                highlight_flag = 1;
                cur_act = S_FLD_COMMAND;
                cur_cmd = F_FIRST;
                kbactive = 1;
            }
        }
    }
    
    old_field = active_field;
    
    swd_dlg->window = active_window;
    swd_dlg->field = active_field;
    swd_dlg->id = curwin->id;
    swd_dlg->type = curwin->type;
    swd_dlg->cur_act = cur_act;
    swd_dlg->cur_cmd = cur_cmd;
    swd_dlg->keypress = g_key;
    
    switch (cur_act)
    {
    case S_FLD_COMMAND:
        swd_dlg->x = curfld->x;
        swd_dlg->y = curfld->y;
        swd_dlg->width = curfld->lx;
        swd_dlg->height = curfld->ly;
        
        switch (cur_cmd)
        {
        case F_DOWN:
            SWD_GetDownField(firstfld, curwin->numflds);
            break;
        
        case F_UP:
            SWD_GetUpField(firstfld, curwin->numflds);
            break;
        
        case F_NEXT:
        case F_RIGHT:
            SWD_GetNextField(firstfld, curwin->numflds);
            break;
        
        case F_PREV:
        case F_LEFT:
            SWD_GetPrevField(firstfld, curwin->numflds);
            break;
        
        case F_TOP:
            active_field = SWD_GetFirstField();
            break;
        
        case F_BOTTOM:
            active_field = SWD_GetLastField(firstfld, curwin->numflds);
            break;
        
        case F_FIRST:
            active_field = curwin->firstfld;
            break;
        
        case F_SELECT:
            curfld->bstatus = DOWN;
            SWD_PutField(curwin, curfld);
            curfld->mark ^= 1;
            if (lastfld && lastfld != curfld)
            {
                lastfld->bstatus = NORMAL;
                SWD_PutField(curwin, lastfld);
                lastfld = NULL;
            }
            GFX_DisplayUpdate();
            while (SWD_IsButtonDown())
            {
                I_GetEvent();
            }
            if (kbactive || curfld->kbflag)
                curfld->bstatus = UP;
            else
                curfld->bstatus = NORMAL;
            
            SWD_PutField(curwin, curfld);
            update = 1;
            break;
        }
        break;
    
    case S_WIN_COMMAND:
        swd_dlg->x = curwin->x;
        swd_dlg->y = curwin->y;
        swd_dlg->width = curwin->lx;
        swd_dlg->height = curwin->ly;
        
        switch (cur_cmd)
        {
        case W_NEXT:
            if (!curwin->lock)
            {
                SWD_GetNextWindow();
                if (active_window == master_window)
                    SWD_GetNextWindow();
                active_field = SWD_GetFirstField();
                if (lastfld)
                    lastfld->bstatus = 0;
                SWD_ShowAllWindows();
                update = 1;
                break;
            }
            break;
        
        case W_MOVE:
            if (movebuffer)
            {
                curfld->bstatus = DOWN;
                SWD_PutField(curwin, curfld);
                if (lastfld && lastfld != curfld)
                {
                    lastfld->bstatus = NORMAL;
                    SWD_PutField(curwin, lastfld);
                    lastfld = NULL;
                }
                GFX_DisplayUpdate();
                sx = cur_mx - curwin->x;
                sy = cur_my - curwin->y;
                
                KBD_Key(SC_ENTER) = 0;
                lastscan = SC_NONE;
                SWD_ShowAllWindows();
                while (mouseb1)
                {
                    curwin->x = cur_mx - sx;
                    curwin->y = cur_my - sy;
                    GFX_MarkUpdate(0, 0, 320, 200);
                    memcpy(displaybuffer, movebuffer, 64000);
                    SWD_PutWin(active_window);
                    GFX_DisplayUpdate();
                }
                curfld->bstatus = NORMAL;
                update = 1;
                SWD_PutField(curwin, curfld);
                break;
            }
            break;
        
        case W_CLOSE:
            SWD_ClearAllButtons();
            lastfld = NULL;
            kbactive = 0;
            break;
        
        case W_CLOSE_ALL:
            SWD_ClearAllButtons();
            lastfld = NULL;
            kbactive = 0;
            break;
        }
        break;
    
    case S_REDRAW:
        SWD_ShowAllWindows();
        update = 1;
        break;
    
    case S_UPDATE:
        update = 1;
        break;
    }
    
    if (old_field != active_field && active_field >= 0 && kbactive)
        highlight_flag = 1;
    
    if (update)
        GFX_DisplayUpdate();
}

/***************************************************************************
   SWD_SetWindowLock() - Locks Window so no others can be selected
 ***************************************************************************/
void 
SWD_SetWindowLock(
    int handle,           // INPUT : handle to window
    int lock              // INPUT : TRUE/FALSE
)
{
    if (g_wins[handle].flag == 1)
        g_wins[handle].win->lock = lock;
}

/***************************************************************************
 SWD_SetWindowXY() - Sets the window x,y position
 ***************************************************************************/
int                           // RETURN: window opt flag
SWD_SetWindowXY(
    int handle,            // INPUT : handle to window
    int xpos,              // INPUT : x position
    int ypos               // INPUT : y position
)
{
    swd_t *curwin;
    curwin = g_wins[handle].win;
    
    curwin->x = xpos;
    curwin->x = ypos;
    
    return curwin->opt;
}

/***************************************************************************
 SWD_GetWindowXYL () - gets the window x, y, x length, y length
 ***************************************************************************/
int                            // RETURN: window opt flag
SWD_GetWindowXYL(
    int handle,              // INPUT : handle to window 
    int *xpos,               // OUTPUT: x position
    int *ypos,               // OUTPUT: y position
    int *lx,                 // OUTPUT: x length
    int *ly                  // OUTPUT: y length
)
{
    swd_t* curwin;
    curwin = g_wins[handle].win;
    
    if (xpos)
        *xpos = curwin->x;
    
    if (ypos)
        *ypos = curwin->y;
    
    if (lx)
        *lx = curwin->lx;
    
    if (ly)
        *ly = curwin->ly;
    
    return curwin->opt;
}

/***************************************************************************
 SWD_GetFieldText() - Gets the field text
 ***************************************************************************/
int                           // RETURN: text max length
SWD_GetFieldText(
    int handle,            // INPUT : window handle
    int field_id,          // INPUT : field handle
    char *out_text         // OUTPUT: text
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    char *text;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    text = (char*)curfld + curfld->txtoff;
    
    memcpy(out_text, text, curfld->maxchars);
    
    return curfld->maxchars;
}

/***************************************************************************
 SWD_SetFieldText() - Sets The default field text
 ***************************************************************************/
int                           // RETURN: text max length
SWD_SetFieldText(
    int handle,            // INPUT : window handle 
    int field_id,          // INPUT : field handle
    const char *in_text    // OUTPUT: pointer to string
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    char *text;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    text = (char*)curfld + curfld->txtoff;
    
    if (in_text)
    {
        text[curfld->maxchars - 1] = 0;
        memcpy(text, in_text, curfld->maxchars - 1);             
    }
    else
        *text = 0;
    
    return curfld->maxchars;
}

/***************************************************************************
   SWD_GetFieldValue () Returns INT value of field text string
 ***************************************************************************/
int 
SWD_GetFieldValue(
    int handle,            // INPUT : window handle
    int field_id           // INPUT : field handle
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    char *text;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    text = (char*)curfld + curfld->txtoff;
    
    return atoi(text);
}

/***************************************************************************
   SWD_SetFieldValue () Sets Numeric (INT) Value into Field Text
 ***************************************************************************/
int 
SWD_SetFieldValue(
    int handle,            // INPUT : window handle
    int field_id,          // INPUT : field handle
    int num                // INPUT : number to set in fld text
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    char *text;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    text = (char*)curfld + curfld->txtoff;
    
    sprintf(text, "%d", num);
    
    return atoi(text);
}

/***************************************************************************
SWD_SetFieldSelect() - Sets Field Selectable status
 ***************************************************************************/
void 
SWD_SetFieldSelect(
    int handle,           // INPUT : window handle
    int field_id,         // INPUT : field handle
    int opt               // INPUT : TRUE, FALSE
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    curfld->selectable = opt;
}

/***************************************************************************
 SWD_GetFieldMark() - Gets the field mark status ( TRUE or FALSE )
 ***************************************************************************/
int                        // RETURN: mark status ( TRUE, FALSE )
SWD_GetFieldMark(
    int handle,            // INPUT : window handle 
    int field_id           // INPUT : field handle
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    return curfld->mark;
}

/***************************************************************************
 SWD_SetFieldMark() - Sets the Field Mark ( button )
 ***************************************************************************/
void 
SWD_SetFieldMark(
    int handle,            // INPUT : window handle
    int field_id,          // INPUT : field handle
    int opt                // INPUT : TRUE, FALSE
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    curfld->mark = opt;
}

/***************************************************************************
 SWD_GetFieldInputOpt() - Gets the field InputOpt status
 ***************************************************************************/
int                           // RETURN: InputOpt status
SWD_GetFieldInputOpt(
    int handle,           // INPUT : window handle
    int field_id          // INPUT : field handle
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    return curfld->input_opt;
}

/***************************************************************************
 SWD_SetFieldInputOpt() - Sets the Field InputOpt ( button )
 ***************************************************************************/
int                           // RETURN: InputOpt status
SWD_SetFieldInputOpt(
    int handle,            // INPUT : window handle
    int field_id,          // INPUT : field handle
    int opt                // INPUT : input option
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    int old_opt;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    old_opt = curfld->input_opt;
    curfld->input_opt = opt;
    
    return old_opt;
}

/***************************************************************************
   SWD_SetFieldItem () - Sets field Item ID ( picture )
 ***************************************************************************/
void 
SWD_SetFieldItem(
    int handle,            // INPUT : window handle
    int field_id,          // INPUT : field handle
    int item               // INPUT : GLB item id
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    if (item != -1)
    {
        if (curfld->item != -1)
            GLB_FreeItem(curfld->item);
        
        curfld->item = item;
        
        GLB_LockItem(item);
    }
    else
        curfld->item = -1;
}

/***************************************************************************
   SWD_GetFieldItem () - Returns Field Item number
 ***************************************************************************/
int                           // RETURN: Item GLB ID
SWD_GetFieldItem(
    int handle,            // INPUT : window handle 
    int field_id           // INPUT : field handle
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    return curfld->item;
}

/***************************************************************************
   SWD_SetFieldItemName () - Sets Field Item Name and Loads it in
 ***************************************************************************/
void 
SWD_SetFieldItemName(
    int handle,            // INPUT : window handle
    int field_id,          // INPUT : field handle
    const char *item_name  // INPUT : pointer to Item Name
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    int item;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    item = GLB_GetItemID(item_name);
    
    if (item != -1)
    {
        if (curfld->item != -1)
            GLB_FreeItem(curfld->item);
        
        memcpy(curfld->item_name, item_name, 16);
        curfld->item = item;
        GLB_LockItem(item);
    }
}

/***************************************************************************
   SWD_GetFieldItemName () - Gets Field Item Name
 ***************************************************************************/
void 
SWD_GetFieldItemName(
    int handle,           // INPUT : window handle
    int field_id,         // INPUT : field handle
    char *item_name       // OUTPUT: pointer to Item Name
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    memcpy(item_name, curfld->item_name, 16);
}

/***************************************************************************
   SWD_SetWindowID () - Sets Window ID number
 ***************************************************************************/
int                           // RETURN: old Window ID
SWD_SetWindowID(
    int handle,            // INPUT : window handle
    int id                 // INPUT : NEW window ID
)
{
    swd_t* curwin;
    int old_id;
    curwin = g_wins[handle].win;
    old_id = curwin->id;
    
    curwin->id = id;
    
    return old_id;
}

/***************************************************************************
   SWD_GetWindowID () - Returns Window ID number
 ***************************************************************************/
int 
SWD_GetWindowID(
    int handle             // INPUT : window handle
)
{
    swd_t* curwin;
    curwin = g_wins[handle].win;
    
    return curwin->id;
}

/***************************************************************************
SWD_SetWindowFlag () - Sets A window to be turned on/off
 ***************************************************************************/
int 
SWD_SetWindowFlag(
    int handle,             // INPUT : window handle
    int flag                // INPUT : TRUE/FALSE
)
{
    swd_t* curwin;
    curwin = g_wins[handle].win;
    
    curwin->display = flag;
    
    SWD_GetNextWindow();
    
    return curwin->id;
}

/***************************************************************************
   SWD_SetWindowType () Sets Window TYPE number
 ***************************************************************************/
int                           // RETURN: old Window TYPE
SWD_SetWindowType(
    int handle,            // INPUT : window handle 
    int type               // INPUT : NEW window TYPE
)
{
    swd_t* curwin;
    int old_type;
    curwin = g_wins[handle].win;
    old_type = curwin->type;
    
    curwin->type = type;
    
    return old_type;
}

/***************************************************************************
   SWD_GetWindowType () - Returns Window TYPE number
 ***************************************************************************/
int                           // RETURN: window TYPE
SWD_GetWindowType(
    int handle             // INPUT : window handle
)
{
    swd_t* curwin;
    curwin = g_wins[handle].win;
    
    return curwin->type;
}

/***************************************************************************
   SWD_GetFieldXYL () Gets Field X,Y, WIDTH, HEIGHT
 ***************************************************************************/
int 
SWD_GetFieldXYL(
    int handle,            // INPUT : window handle
    int field_id,          // INPUT : field handle
    int* x,                // OUTPUT: x
    int* y,                // OUTPUT: y
    int* lx,               // OUTPUT: width 
    int* ly                // OUTPUT: height
)
{
    swd_t* curwin;
    swdfield_t *curfld;
    curwin = g_wins[handle].win;
    
    curfld = (swdfield_t*)((char*)curwin + curwin->fldofs) + field_id;
    
    if (x)
        *x = curwin->x + curfld->x;
    
    if (y)
        *y = curwin->y + curfld->y;
    
    if (lx)
        *lx = curfld->lx;
    
    if (ly)
        *ly = curfld->ly;
    
    return curfld->lx;
}
