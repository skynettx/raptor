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

window_t g_wins[12];

swdfield_t *lastfld;

char *movebuffer;

void (*winfuncs[12])(wdlg_t*);
void (*fldfuncs[15])(swd_t*, swdfield_t*);

void FUN_0002b750(int a1, int a2, int a3, int a4, int a5)
{
    switch (a1)
    {
    case 2:
        GFX_HShadeLine(0, a2, a3, a4);
        GFX_VShadeLine(0, a2 + a4 - 1, a3 + 1, a5 - 1);
        break;
    case 1:
        GFX_HShadeLine(1, a2 + 2, a3, a4 - 2);
        GFX_VShadeLine(1, a2 + a4 - 1, a3 + 1, a5 - 3);
    default:
        GFX_HShadeLine(1, a2 + 1, a3, a4 - 1);
        GFX_VShadeLine(1, a2 + a4 - 1, a3 + 1, a5 - 2);
        GFX_HShadeLine(0, a2, a3 + a5 - 1, a4);
        GFX_VShadeLine(0, a2, a3, a5 - 1);
        break;
    }
}

char tcmds[][14] = {
    "TEXT_IMAGE",
    "TEXT_COLOR",
    "TEXT_POS",
    "TEXT_RIGHT",
    "TEXT_DOWN"
};

int textcmd_flag;
int textdraw_x, textdraw_y;
int textcmd_x, textcmd_y;
int textcmd_x2, textcmd_y2;
int textcolor;
int textcmd_line;
char textfill[81];

int SWD_GetLine(const char *a1)
{
    char buf[81];
    static const char *text;
    const char *sep = "\n\v\r \t,;\b";
    const char *tok;
    int p, i, id, x, y, col;
    texture_t *img;
    if (a1)
        text = a1;
    textcmd_flag = 0;
    p = 0;
    memcpy(buf, text, 81);
    tok = strtok(buf, sep);
    for (i = 0; i < 5; i++)
    {
        if (!strcmp(tok, tcmds[i]))
        {
            textcmd_flag = 1;
            while (text[p] > 31)
                p++;
            while (text[p] <= 31)
                p++;
            text += p;
            tok = strtok(NULL, sep);
            switch (i + 1)
            {
            case 1:
                id = GLB_GetItemID(tok);
                if (id == -1)
                    break;
                img = (texture_t*)GLB_GetItem(id);
                tok = strtok(NULL, sep);
                if (!tok)
                {
                    x = textdraw_x;
                    y = textdraw_y;
                }
                else
                {
                    x = atoi(tok);
                    tok = strtok(NULL, sep);
                    y = atoi(tok);
                    x += textcmd_x;
                    y += textcmd_y;
                }
                if (x > textcmd_x2 || y > textcmd_y2)
                    break;
                textdraw_x += img->f_c + 1;
                textdraw_y = y;
                GFX_PutImage(img, x, y, 0);
                GLB_FreeItem(id);
                break;
            case 2:
                col = atoi(tok);
                if (col >= 0 && col < 256)
                    textcolor = col;
                break;
            case 3:
                x = atoi(tok);
                tok = strtok(NULL, sep);
                y = atoi(tok);
                if (x > textcmd_x2 || y > textcmd_y2)
                    break;
                textdraw_x = textcmd_x + x;
                textdraw_y = textcmd_y + y;
                textcmd_line = 0;
                break;
            case 4:
                x = atoi(tok);
                if (x > textcmd_x2)
                    break;
                if (tok)
                    textdraw_x += x;
                break;
            case 5:
                y = atoi(tok);
                if (y > textcmd_y2)
                    break;
                if (tok)
                    textdraw_y += y;
                break;
            }
            return p;
        }
    }
    while (text[p] > 31)
        p++;
    memcpy(textfill, text, p);
    textfill[p] = 0;
    while (text[p] <= 31)
        p++;
    text += p;
    return p;
}

void SWD_FillText(font_t *a1, int a2, int a3, int a4, int a5, int a6, int a7)
{
    char *dat;
    int vbp, vsi;
    if (a2 == -1)
        return;
    dat = GLB_LockItem(a2);
    if (!dat)
        return;
    textcmd_x = a4;
    textdraw_x = a4;
    textcmd_y = a5;
    textdraw_y = a5;
    textcolor = a3;
    textcmd_x2 = a4 + a6 - 1;
    textcmd_y2 = a5 + a7 - 1;
    textcmd_line = a4;
    vbp = GLB_ItemSize(a2);
    vsi = SWD_GetLine(dat);
    while (1)
    {
        if (!textcmd_flag)
        {
            GFX_Print(textdraw_x, textdraw_y, textfill, a1, textcolor);
            textdraw_y += a1->f_0 + 3;
        }
        if (vsi < vbp)
            vsi += SWD_GetLine(NULL);
        else
            break;
    }
    GLB_FreeItem(a2);
}

void SWD_PutField(swd_t *a1, swdfield_t *a2)
{
    font_t *v50;
    char *v24;
    int v4c;
    int v34;
    int vbp;
    int v38;
    int v1c;
    int vdi;
    int vc1;
    int v20;
    int v28;
    int vd;
    int i;
    texture_t *tex;
    
    v50 = (font_t*)GLB_GetItem(a2->f_54);
    v24 = (char*)a2 + a2->f_8c;
    v4c = v50->f_0;
    v34 = 0;
    vbp = a2->f_7c + a1->f_64;
    v38 = 0;
    v1c = a2->f_80 + a1->f_68;
    vc1 = strlen(v24);
    vd = GFX_StrPixelLen(v50, v24, vc1);
    v20 = vbp + ((a2->f_84 - vd) >> 1);
    v28 = v1c + ((a2->f_88 - v50->f_0) >> 1);
    if (a2->f_1c == 2 && a2->f_0 != 6)
    {
        if (v20 > 0)
            v20--;
        v28++;
    }
    if (a2->f_70 && a2->f_90)
        GFX_PutImage(a2->f_90, vbp, v1c, NULL);
    if (a2->f_60 && a2->f_60 != 4)
    {
        if (a2->f_40 == -1) goto LAB_0002c422;
        if (a1->f_60 == 3)
            v34 = 1;
        switch (a2->f_0)
        {
        case 2:
            tex = (texture_t*)GLB_GetItem(a2->f_40);
            if (a2->f_60 == 1)
            {
                GFX_PutTexture(tex, vbp, v1c, a2->f_84, a2->f_88);
                FUN_0002b750(a2->f_1c, vbp, v1c, a2->f_84, a2->f_88);
            }
            else
            {
                GFX_PutImage(tex, vbp, v1c, v34);
            }
            v38 = 1;
            break;
        case 6:
            tex = (texture_t*)GLB_GetItem(a2->f_40);
            if (a2->f_60 == 1)
            {
                GFX_PutTexture(tex, vbp, v1c, a2->f_84, a2->f_88);
                GFX_LightBox(1, vbp, v1c, a2->f_84, a2->f_88);
            }
            else
            {
                GFX_PutImage(tex, vbp, v1c, v34);
            }
            if (a1 != g_wins[active_window].f_c)
                GFX_ShadeArea(2, vbp, v1c, a2->f_84, a2->f_88);
            v38 = 1;
            break;
        case 9:
            tex = (texture_t*)GLB_GetItem(a2->f_40);
            if (!tex)
                break;
            if (a2->f_60 == 1)
            {
                GFX_PutTexture(tex, vbp, v1c, a2->f_84, a2->f_88);
                goto LAB_0002c422;
            }
            if (a2->f_84 < tex->f_c || a2->f_88 < tex->f_10)
            {
                GFX_ScalePic(tex, vbp, v1c, a2->f_84, a2->f_88, 0);
            }
            else
            {
                GFX_PutImage(tex, vbp, v1c, v34);
            }
            break;
        case 4:
        case 5:
            tex = (texture_t*)GLB_GetItem(a2->f_40);
            GFX_PutImage(tex, vbp, v1c, v34);
            break;
        case 1:
            SWD_FillText(v50, a2->f_40, a2->f_58, vbp, v1c, a2->f_84, a2->f_88);
            break;
        case 3:
        case 8:
        case 7:
            break;
        }
        if (!a2->f_1c)
            goto LAB_0002c422;
    }
    else
    {
        switch (a2->f_0)
        {
        case 1:
            if (a2->f_5c)
                GFX_Print(vbp, v1c, v24, v50, a2->f_58);
            break;
        case 2:
            if (a2->f_60 != 4)
            {
                GFX_ColorBox(vbp, v1c, a2->f_84, a2->f_88, a2->f_64);
                FUN_0002b750(a2->f_1c, vbp, v1c, a2->f_84, a2->f_88);
                v38 = 1;
            }
            else
            {
                GFX_Print(v20, v28, v24, v50, a2->f_58);
            }
            break;
        case 3:
            if (a2->f_1c == 0)
                GFX_ColorBox(vbp, v1c, a2->f_84, a2->f_88, a2->f_64);
            else
                GFX_ColorBox(vbp, v1c, a2->f_84, a2->f_88, a2->f_68);
            if (a2->f_5c)
                GFX_Print(vbp + 1, v28, v24, v50, a2->f_58);
            if (a2->f_1c)
            {
                vc1 = strlen(v24);
                vd = GFX_StrPixelLen(v50, v24, vc1);
                v20 = vbp + 1 + vd;
                if (vd + 2 < a2->f_84)
                    GFX_VLine(v20, v1c + 1, v4c - 1, a2->f_58);
            }
            break;
        case 4:
            GFX_ColorBox(vbp, v1c, a2->f_84, a2->f_88, a2->f_64);
            GFX_LightBox(1, vbp, v1c, a2->f_84, a2->f_88);
            GFX_ColorBox(vbp + 2, v1c + 2, a2->f_84 - 4, a2->f_88 - 4, 0);
            v20 = vbp + 3;
            v28 = v1c + 3;
            if (a2->f_6c)
            {
                GFX_ColorBox(vbp + 3, v1c + 3, a2->f_84 - 6, a2->f_88 - 6, a2->f_68);
                FUN_0002b750(a2->f_1c, vbp + 3, v1c + 3, a2->f_84 - 6, a2->f_88 - 6);
            }
            else
            {
                GFX_ColorBox(vbp + 3, v1c + 3, a2->f_84 - 6, a2->f_88 - 6, 0);
            }
            break;
        case 5:
            if (a2->f_60 == 4) goto LAB_0002c422;
            
            GFX_ColorBox(vbp, v1c, a2->f_84, a2->f_88, a2->f_68);
            GFX_LightBox(1, vbp, v1c, a2->f_84, a2->f_88);
            GFX_ColorBox(vbp + 2, v1c + 2, a2->f_84 - 4, a2->f_88 - 4, a2->f_68);
            GFX_ColorBox(vbp + 3, v1c + 3, a2->f_84 - 6, a2->f_88 - 6, a2->f_68);
            FUN_0002b750(a2->f_1c, vbp + 3, v1c + 3, a2->f_84 - 6, a2->f_88 - 6);
            v20 = vbp + 3;
            v28 = v1c + 3;
            break;
        case 6:
            if (a2->f_60 != 4)
                GFX_ColorBox(vbp, v1c, a2->f_84, a2->f_88, a2->f_64);
            if (a2->f_5c > 1)
                GFX_Print(v20, v28, v24, v50, a2->f_58);
            if (a2->f_60 != 4 && a1 != g_wins[active_window].f_c)
            {
                GFX_ShadeArea(0, vbp, v1c, a2->f_84, a2->f_88);
                if (a2->f_64)
                {
                    for (i = 0; i < a2->f_88; i += 2)
                    {
                        GFX_HShadeLine(0, vbp, v1c + i, a2->f_84);
                    }
                }
            }
            break;
        case 7:
            if (a2->f_64)
                GFX_ShadeArea(0, vbp + 1, v1c, a2->f_84 - 1, a2->f_88 - 1);
            GFX_LightBox(2, vbp, v1c, a2->f_84, a2->f_88);
            if (!a2->f_64)
                GFX_ColorBox(vbp + 1, v1c + 1, a2->f_84 - 2, a2->f_88 - 2, 0);
            break;
        case 8:
            GFX_ShadeArea(1, vbp + 1, v1c, a2->f_84 - 1, a2->f_88 - 1);
            GFX_LightBox(1, vbp, v1c, a2->f_84, a2->f_88);
            if (!a2->f_64)
                GFX_ColorBox(vbp + 1, v1c + 1, a2->f_84 - 2, a2->f_88 - 2, 0);
            break;
        }
    }
    if (a2->f_1c && a2->f_0 != 3)
    {
        if (a2->f_60 == 2)
            tex = (texture_t*)GLB_GetItem(a2->f_40);
        else
            tex = NULL;
        if (a2->f_1c == 2)
        {
            if (tex && tex->f_0 == 0)
                GFX_ShadeShape(0, tex, vbp, v1c);
            else
                GFX_ShadeArea(0, vbp, v1c, a2->f_84, a2->f_88);
        }
        else if (a2->f_1c == 1)
        {
            if (tex && tex->f_0 == 0)
                GFX_ShadeShape(1, tex, vbp, v1c);
            else
                GFX_ShadeArea(1, vbp, v1c, a2->f_84, a2->f_88);
        }
    }
LAB_0002c422:
    if (v38 && a2->f_5c > 1)
        GFX_Print(v20, v28, v24, v50, a2->f_58);
    }

void SWD_DoButton(swd_t *a1, swdfield_t *a2)
{
    if (StickY > 0)                                                   //Controller Input DoButton
    {
        JOY_IsKey(StickY);
        g_key = 80;
    }
    if (StickY < 0)
    {
        JOY_IsKey(StickY);
        g_key = 72;
    }
    if (StickX > 0)
    {
        JOY_IsKey(StickX);
        g_key = 77;
    }
    if (StickX < 0)
    {
        JOY_IsKey(StickX);
        g_key = 75;
    }
    switch (Down)
    {       
    case 1: 
        JOY_IsKey(Down);
        g_key = 80;
        break;
    }
    switch (Up)
    {        
    case 1: 
        JOY_IsKey(Up);
        g_key = 72;
        break;
    }
    switch (Left)
    {
    case 1: 
        JOY_IsKey(Left);
        g_key = 75;
        break;
    }
    switch (Right)
    {
    case 1: 
        JOY_IsKey(Right);
        g_key = 77;
        break;
    }
    switch (AButton)
    {
    case 1: 
        JOY_IsKey(AButton);
        g_key = 28;
        break;
    }
    if (!g_button_flag)
        return;
    switch (g_key)
    {
    case 15:
        if (keyboard[56])
        {
            cur_act = 2;
            cur_cmd = 14;
            while (keyboard[15]) {
            }
        }
        else
        {
            cur_act = 1;
            if (capslock + keyboard[42] + keyboard[54])
                cur_cmd = 4;
            else
                cur_cmd = 3;
        }
        break;
    case 28:
        cur_act = 1;                                                         //Enter
        cur_cmd = 10;
        break;
    case 80:
        if (a1->f_8)                                                         //Cursor Down
        {
            cur_act = 1;
            cur_cmd = 1;
        }
        break;
    case 72:                                                                //Cursor Up
        if (a1->f_8)
        {
            cur_act = 1;
            cur_cmd = 2;
        }
        break;
    case 77:
        if (a1->f_8)                                                        //Cursor Right
        {
            cur_act = 1;
            cur_cmd = 5;
        }
        break;
    case 75:
        if (a1->f_8)                                                        //Cursor Left
        {
            cur_act = 1;
            cur_cmd = 6;
        }
        break;
    }
}

void SWD_FieldInput(swd_t *a1, swdfield_t *a2)
{
    static int curpos;
    font_t *vbx;
    char *vs;
    int vd;
    vd = 0;
    vbx = (font_t*)GLB_GetItem(a2->f_54);
    vs = (char*)a2 + a2->f_8c;
    curpos = strlen(vs);
    
    if (StickY > 0)                                                   //Controller Input FieldInput
    {
        JOY_IsKey(StickY);
        g_key = 80;
    }
    if (StickY < 0)
    {
        JOY_IsKey(StickY);
        g_key = 72;
    }
    if (StickX > 0)
    {
        JOY_IsKey(StickX);
        g_key = 77;
    }
    if (StickX < 0)
    {
        JOY_IsKey(StickX);
        g_key = 75;
    }
    switch (Down)
    {
    case 1:
        JOY_IsKey(Down);
        g_key = 80;
        break;
    }
    switch (Up)
    {
    case 1:
        JOY_IsKey(Up);
        g_key = 72;
        break;
    }
    switch (Left)
    {
    case 1:
        JOY_IsKey(Left);
        g_key = 75;
        break;
    }
    switch (Right)
    {
    case 1:
        JOY_IsKey(Right);
        g_key = 77;
        break;
    }
    switch (AButton)
    {
    case 1:
        JOY_IsKey(AButton);
        g_key = 28;
        break;
    }
    switch (g_key)
    {
    case 15:
        if (keyboard[56])
        {
            while (keyboard[15]) {
            }
            cur_act = 2;
            cur_cmd = 14;
        }
        else
        {
            cur_act = 1;
            if (capslock + keyboard[42] + keyboard[54])
                cur_cmd = 4;
            else
                cur_cmd = 3;
        }
        break;
    case 28:
        cur_act = 1;
        cur_cmd = 10;
        break;
    case 80:
        if (a1->f_8)
        {
            cur_act = 1;
            cur_cmd = 1;
        }
        break;
    case 72:
        if (a1->f_8)
        {
            cur_act = 1;
            cur_cmd = 2;
        }
        break;
    case 77:
        if (a1->f_8)
        {
            cur_act = 1;
            cur_cmd = 5;
        }
        break;
    case 75:
        if (a1->f_8)
        {
            cur_act = 1;
            cur_cmd = 6;
        }
        break;
    case 14:
        vd = 1;
        if (curpos > 0)
            curpos--;
        vs[curpos] = 0;
        break;
    default:
        if (keyboard[21] && keyboard[29])
        {
            curpos = 0;
            vd = 1;
            *vs = 0;
        }
        else if (!keyboard[56] && !keyboard[29] && g_key > 0 && a2->f_5c-1 > curpos)
        {
            if (g_ascii > 31 && g_ascii < 127)
            {
                switch (a2->f_18)
                {
                case 0:
                    vs[curpos] = g_ascii;
                    break;
                case 1:
                    vs[curpos] = toupper(g_ascii);
                    break;
                case 2:
                    if (isdigit(g_ascii) || g_ascii == '-')
                        vs[curpos] = g_ascii;
                    else
                        vs[curpos] = 0;
                    break;
                }
            }
            else
                vs[curpos] = 0;
            if (GFX_StrPixelLen(vbx, vs, curpos + 1) >= a2->f_84)
                curpos--;
            vd = 1;
            vs[curpos + 1] = 0;
        }
        break;
    }
    if (vd)
    {
        SWD_PutField(a1, a2);
        cur_act = 5;
        cur_cmd = 0;
    }
}

void FUN_0002c8e4(int a1)
{
    int i;
    swd_t* vb;
    swdfield_t *va;
    obj_x = 0;
    obj_y = 0;
    obj_width = 0;
    obj_height = 0;
    vb = g_wins[a1].f_c;
    va = (swdfield_t*)((char*)vb + vb->f_4c);
    for (i = 0; i < vb->f_60; i++)
    {
        if (va[i].f_0 == 10)
        {
            obj_x = va[i].f_7c;
            obj_y = va[i].f_80;
            obj_width = va[i].f_84;
            obj_height = va[i].f_88;
            return;
        }
    }
}

void SWD_SetWindowFlag(void)
{
    int i;
    int va;
    va = active_window - 1;
    active_window = -1;
    if (va < 0)
        va = 11;
    for (i = 0; i < 12; i++)
    {
        if (g_wins[va].f_4 && g_wins[va].f_c->f_c)
        {
            active_window = va;
            active_field = g_wins[va].f_c->f_54;
            break;
        }
        va--;
        if (va < 0)
            va = 11;
    }
    if (active_window == -1)
        active_field = -1;
    lastfld = NULL;
}

int FUN_0002c9d8(void)
{
    return g_wins[active_window].f_c->f_54;
}

int FUN_0002c9ec(swdfield_t *a1, int a2)
{
    int vc;
    int i;
    vc = -1;
    for (i = a2 - 1; i >= 0; i--)
    {
        switch (a1[i].f_0)
        {
        case 0:
        case 1:
        case 6:
        case 7:
        case 8:
            break;
        default:
            if (a1[i].f_78)
                vc = i;
            break;
        }
        if (vc != -1)
            break;
    }
    return vc;
}

int SWD_GetRightField(swdfield_t *a1, int a2)
{
    int vbp, i, vdi, va;
    swdfield_t *vs;
    vdi = 0x7fff;
    vs = a1 + active_field;
    vbp = FUN_0002c9d8();
    for (i = 0; i < a2; i++)
    {
        if (a1[i].f_0 != 6 && a1[i].f_78 && a1[i].f_4 > vs->f_4)
        {
            va = abs(a1[i].f_4 - vs->f_4);
            if (va < vdi)
            {
                vdi = va;
                vbp = i;
            }
        }
    }
    if (vbp != -1)
        active_field = vbp;
    return vbp;
}

int SWD_GetPrevField(swdfield_t *a1, int a2)
{
    int vbp, i, vdi, va;
    swdfield_t *vs;
    vdi = 0x7fff;
    vs = a1 + active_field;
    vbp = FUN_0002c9d8();
    for (i = 0; i < a2; i++)
    {
        if (a1[i].f_0 != 6 && a1[i].f_78 && a1[i].f_4 < vs->f_4)
        {
            va = abs(vs->f_4 - a1[i].f_4);
            if (va < vdi)
            {
                vdi = va;
                vbp = i;
            }
        }
    }
    if (vbp != -1)
        active_field = vbp;
    return vbp;
}

int FUN_0002cb50(swdfield_t *a1, int a2)
{
    int v1c, v18, i, d;
    swdfield_t *vd;

    v1c = -1;
    v18 = 0x7fff;
    vd = a1 + active_field;
    for (i = 0; i < a2; i++)
    {
        switch (a1[i].f_0)
        {
        default:
            if (a1[i].f_7c > vd->f_7c)
            {
                d = abs(a1[i].f_7c - vd->f_7c) + abs(a1[i].f_80 - vd->f_80);
                if (d < v18 && a1[i].f_78)
                {
                    v18 = d;
                    v1c = i;
                }
            }
            break;
        case 0:
        case 1:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        }
    }
    if (v1c < 0)
        SWD_GetRightField(a1, a2);
    else
        active_field = v1c;
    return v1c;
}

int SWD_GetUpField(swdfield_t *a1, int a2)
{
    int v1c, v18, i, d;
    swdfield_t *vd;

    v18 = 0x7fff;
    v1c = FUN_0002c9ec(a1, a2);
    vd = a1 + active_field;
    for (i = 0; i < a2; i++)
    {
        switch (a1[i].f_0)
        {
        default:
            if (a1[i].f_80 < vd->f_80)
            {
                d = abs(a1[i].f_7c - vd->f_7c) + abs(a1[i].f_80 - vd->f_80);
                if (d < v18 && a1[i].f_78)
                {
                    v18 = d;
                    v1c = i;
                }
            }
            break;
        case 0:
        case 1:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        }
    }
    if (v1c < 0)
        active_field = FUN_0002c9d8();
    else
        active_field = v1c;
    return v1c;
}

int SWD_GetDownField(swdfield_t *a1, int a2)
{
    int v1c, v18, i, d;
    swdfield_t *vd;

    v18 = 0x7fff;
    v1c = FUN_0002c9d8();
    vd = a1 + active_field;
    for (i = 0; i < a2; i++)
    {
        switch (a1[i].f_0)
        {
        default:
            if (a1[i].f_80 > vd->f_80)
            {
                d = abs(a1[i].f_7c - vd->f_7c) + abs(a1[i].f_80 - vd->f_80);
                if (d < v18 && a1[i].f_78)
                {
                    v18 = d;
                    v1c = i;
                }
            }
            break;
        case 0:
        case 1:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        }
    }
    if (v1c < 0)
        active_field = FUN_0002c9d8();
    else
        active_field = v1c;
    return v1c;
}

int FUN_0002cdbc(swdfield_t *a1, int a2)
{
    int v1c, v18, i, d;
    swdfield_t *vd;

    v1c = -1;
    v18 = 0x7fff;
    vd = a1 + active_field;
    for (i = 0; i < a2; i++)
    {
        switch (a1[i].f_0)
        {
        default:
            if (a1[i].f_7c < vd->f_7c)
            {
                d = abs(a1[i].f_7c - vd->f_7c) + abs(a1[i].f_80 - vd->f_80);
                if (d < v18 && a1[i].f_78)
                {
                    v18 = d;
                    v1c = i;
                }
            }
            break;
        case 0:
        case 1:
        case 6:
        case 7:
        case 8:
        case 9:
            break;
        }
    }
    if (v1c < 0)
        SWD_GetPrevField(a1, a2);
    else
        active_field = v1c;
    return v1c;
}


int SWD_ShowAllFields(swd_t *a1)
{
    int v1c, i, v20, vbp;
    texture_t *tex;
    swdfield_t *vs = (swdfield_t*)((char*)a1 + a1->f_4c);
    v1c = 0;
    for (i = 0; i < a1->f_60; i++)
    {
        if (vs[i].f_0)
        {
            vbp = a1->f_64 + vs[i].f_7c;
            v20 = a1->f_68 + vs[i].f_80;
            if (vs[i].f_70 && vs[i].f_90)
            {
                vs[i].f_90->f_c = (short)vs[i].f_84;
                vs[i].f_90->f_10 = (short)vs[i].f_88;
                GFX_GetScreen(vs[i].f_90->f_14, vbp, v20, vs[i].f_84, vs[i].f_88);
            }
            if (vs[i].f_74)
            {
                if (vs[i].f_60 != 3)
                {
                    GFX_LightBox(1, vbp - 1, v20 + 1, vs[i].f_84, vs[i].f_88);
                }
                else if (vs[i].f_40 != -1)
                {
                    tex = (texture_t*)GLB_GetItem(vs[i].f_40);
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
    vs = g_wins[a1].f_c;
    v20 = 8;
    v1c = vs->f_68 + vs->f_70;
    vdi = vs->f_64 - 8;
    v24 = vs->f_6c;
    vc = vs->f_68 + 8;
    if (vs->f_c)
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
            GFX_ColorBox(vs->f_64, vs->f_68, vs->f_6c, vs->f_70, vs->f_5c);
            if (vs->f_6c < 320 && vs->f_70 < 200)
                GFX_LightBox(1, vs->f_64, vs->f_68, vs->f_6c, vs->f_70);
            break;
        case 2:
            if (vs->f_40 != -1)
            {
                tex = (texture_t*)GLB_GetItem(vs->f_40);
                GFX_PutImage(tex, vs->f_64, vs->f_68, 0);
            }
            break;
        case 3:
            if (vs->f_40 != -1)
            {
                tex = (texture_t*)GLB_GetItem(vs->f_40);
                GFX_PutImage(tex, vs->f_64, vs->f_68, 1);
            }
            break;
        case 1:
            if (vs->f_40 != -1)
            {
                tex = (texture_t*)GLB_GetItem(vs->f_40);
                GFX_PutTexture(tex, vs->f_64, vs->f_68, vs->f_6c, vs->f_70);
                GFX_LightBox(1, vs->f_64, vs->f_68, vs->f_6c, vs->f_70);
            }
            break;
        case 4:
            if (vs->f_5c == 0)
            {
                GFX_ShadeArea(0, vs->f_64, vs->f_68, vs->f_6c, vs->f_70);
                GFX_LightBox(1, vs->f_64, vs->f_68, vs->f_6c, vs->f_70);
            }
            break;
        }
        if (vs->f_60)
            SWD_ShowAllFields(vs);
        if (winfuncs[a1])
        {
            FUN_0002c8e4(a1);
            wdlg.f_1c = obj_x;
            wdlg.f_20 = obj_y;
            wdlg.f_28 = obj_width;
            wdlg.f_24 = obj_height;
            wdlg.f_14 = vs->f_18;
            wdlg.f_18 = vs->f_1c;
            wdlg.f_0 = active_window;
            wdlg.f_4 = active_field;
            winfuncs[a1](&wdlg);
        }
    }
}

int FUN_0002d1ac(void)
{
    if (keyboard[28])
        return 1;
    if (mouseb1)
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
        lastfld->f_1c = 0;
        lastfld = NULL;
    }
    v1c = (swd_t*)GLB_LockItem(a1);
    vd = (swdfield_t*)((char*)v1c + v1c->f_4c);
    for (i = 0; i < 12; i++)
    {
        if (!g_wins[i].f_4)
        {
            prev_window = active_window;
            g_wins[i].f_c = v1c;
            g_wins[i].f_4 = 1;
            g_wins[i].f_0 = a1;
            active_window = i;
            v1c->f_c = 1;
            active_field = g_wins[i].f_c->f_54;
            if (!vd[active_field].f_78)
                active_field = FUN_0002c9d8();
            if (v1c->f_44)
            {
                v1c->f_40 = GLB_GetItemID(v1c->f_30);
                GLB_LockItem(v1c->f_40);
            }
            for (j = 0; j < v1c->f_60; j++)
            {
                if (vd[j].f_0)
                {
                    if (vd[j].f_0 == 11)
                        g_wins[active_window].f_8 = 1;
                    switch (vd[j].f_0)
                    {
                    case 2:
                    case 4:
                    case 5:
                    case 6:
                        if (usekb_flag && vd[j].f_78)
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
                    vd[j].f_1c = 0;
                    vd[j].f_54 = GLB_GetItemID(vd[j].f_44);
                    if (vd[j].f_54 != -1)
                        GLB_LockItem(vd[j].f_54);
                    if (!vd[j].f_60)
                        vd[j].f_40 = -1;
                    else
                        vd[j].f_40 = GLB_GetItemID(vd[j].f_30);
                    if (vd[j].f_40 != -1)
                        GLB_LockItem(vd[j].f_40);
                    vd[j].f_90 = NULL;
                    if (vd[j].f_70)
                    {
                        vb = vd[j].f_84 * vd[j].f_88 + 20;
                        if (vb < 0 || vb > 64000)
                            EXIT_Error("SWD Error: pic save to big...");
                        vd[j].f_90 = (texture_t*)malloc(vb);
                        if (!vd[j].f_90)
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
    if (a2 && g_wins[a1].f_4)
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
    if (master_window != -1 && g_wins[master_window].f_4)
        SWD_PutWin(master_window);
    if (viewdraw)
        viewdraw();
    for (i = 0; i < 12; i++)
    {
        if (g_wins[i].f_4 && i != active_window && i != master_window)
            SWD_PutWin(i);
    }
    if (movebuffer)
        memcpy(movebuffer, displaybuffer, 64000);
    if (active_window != -1 && active_window != master_window && g_wins[active_window].f_4)
        SWD_PutWin(active_window);
    return 1;
}

void SWD_SetWindowPtr(int a1)
{
    swd_t* va;
    swdfield_t *fl;
    va = g_wins[a1].f_c;
    if (!ptractive || a1 == -1)
        return;

    if (active_field == -1 || !va->f_60)
    {
        if (!g_wins[a1].f_4 || va == NULL)
            return;
        PTR_SetPos(va->f_64 + (va->f_6c>>1), va->f_68 + (va->f_70>>1));
    }
    else
    {
        fl = (swdfield_t*)((char*)va + va->f_4c);
        fl += active_field;
        PTR_SetPos(fl->f_7c + (fl->f_84>>1), fl->f_80 + (fl->f_88>>1));
    }
}

void SWD_SetFieldPtr(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fl;
    va = g_wins[a1].f_c;
    if (!ptractive || a1 == -1)
        return;

    if (a2 == -1 || !va->f_60)
    {
        if (!g_wins[a1].f_4 || va == NULL)
            return;
        PTR_SetPos(va->f_64 + (va->f_6c>>1), va->f_68 + (va->f_70>>1));
    }
    else
    {
        fl = (swdfield_t*)((char*)va + va->f_4c);
        fl += a2;
        PTR_SetPos(fl->f_7c + (fl->f_84>>1), fl->f_80 + (fl->f_88>>1));
    }
}

void FUN_0002d7c8(int a1)
{
    if (!g_wins[a1].f_4)
        EXIT_Error("SWD: SetActiveWindow #%u", a1);
    active_window = a1;
}

void SWD_SetActiveField(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fl;
    va = g_wins[a1].f_c;
    if (active_field != -1)
        lastfld = (swdfield_t*)((char*)va + va->f_4c) + active_field;
    fl = (swdfield_t*)((char*)va + va->f_4c);
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
    va = g_wins[a1].f_c;
    PTR_ResetJoyStick();
    if (!g_wins[a1].f_4)
        EXIT_Error("SWD: DestroyWindow %d", a1);
    fl = (swdfield_t*)((char*)va + va->f_4c);
    for (i = 0; i < va->f_60; i++)
    {
        if (fl[i].f_40 != -1)
            GLB_FreeItem(fl[i].f_40);
        if (fl[i].f_54 != -1)
            GLB_FreeItem(fl[i].f_54);
        if (fl[i].f_70 && fl[i].f_90)
            free(fl[i].f_90);
    }
    if (va->f_40)
        GLB_FreeItem(va->f_40);
    GLB_FreeItem(g_wins[a1].f_0);
    g_wins[a1].f_4 = 0;
    winfuncs[a1] = NULL;
    if (a1 == master_window)
        master_window = -1;
    kbactive = 0;
    highlight_flag = 0;
    lastfld = NULL;
    SWD_SetWindowFlag();
    if (active_field != -1)
    {
        va = g_wins[active_window].f_c;
        fl = (swdfield_t*)((char*)va + va->f_4c);
        if (fl[active_field].f_c)
            kbactive = 1;
    }
    //if (g_wins[prev_window].f_4)
    if (prev_window >= 0) if (g_wins[prev_window].f_4)
    {
        vd = prev_window;
        prev_window = active_window;
        active_window = vd;
        active_field = g_wins[active_window].f_c->f_54;
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

    va = g_wins[active_window].f_c;
    v18 = -1;
    vb = va->f_64 + va->f_6c;
    vd = va->f_68 + va->f_70;
    if (a1 > va->f_64 && a1 < vb && a2 > va->f_68 && a2 < vd)
        v18 = active_window;
    else
    {
        for (i = 0; i < 12; i++)
        {
            if (g_wins[i].f_4 == 1)
            {
                va = g_wins[i].f_c;
                vb = va->f_64 + va->f_6c;
                vd = va->f_68 + va->f_70;
                if (a1 > va->f_64 && a1 < vb && a2 > va->f_68 && a2 < vd)
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
    for (i = 0; i < a2->f_60; i++)
    {
        v24 = a2->f_64 + a3[i].f_7c;
        vs = a2->f_68 + a3[i].f_80;
        v1c = v24 + a3[i].f_84 + 1;
        v20 = vs + a3[i].f_88 + 1;
        if (v18 >= v24 && v18 <= v1c && vs <= v14 && v14 <= v20)
        {
            vb = 1;
            switch (a3[i].f_0)
            {
            default:
                vb = 0;
                break;
            case 6:
                if (a3[i].f_78)
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

    for (i = 0; i < a2->f_60; i++)
    {
        vdi = a2->f_64 + a3[i].f_7c;
        vc = a2->f_68 + a3[i].f_80;
        v20 = vdi + a3[i].f_84 + 1;
        v24 = vc + a3[i].f_88 + 1;
        if (vdi <= v18 && v18 <= v20 && vc <= v1c && v1c <= v24)
        {
            switch (a3[i].f_0)
            {
            case 11:
                v14 = 1;
                a1->f_30 = 1;
                a1->f_38 = a3[i].f_7c;
                a1->f_3c = a3[i].f_80;
                a1->f_24 = a3[i].f_84;
                a1->f_28 = a3[i].f_88;
                a1->f_34 = i;
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
        if (g_wins[i].f_4)
        {
            vc = g_wins[i].f_c;
            fl = (swdfield_t*)((char*)vc + vc->f_4c);
            for (j = 0; j < vc->f_60; j++)
            {
                fl[j].f_1c = 0;
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

    vc = g_wins[active_window].f_c;
    vcc = (swdfield_t*)((char*)vc + vc->f_4c);
    v1c = vcc + active_field;
    cur_act = 0;
    cur_cmd = 0;
    if (highlight_flag)
    {
        highlight_flag = 0;
        if (lastfld)
        {
            lastfld->f_1c = 0;
            SWD_PutField(vc, lastfld);
            lastfld = NULL;
            v28 = 1;
        }
        if (kbactive)
        {
            v1c->f_1c = 1;
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
    a1->f_30 = 0;
    if (g_wins[active_window].f_8)
        FUN_0002dbe4(a1, vc, vcc);
    if (active_field == -1)
        return;

    if (mouseb1 && !cur_act)
    {
        old_field = active_field;
        if (FUN_0002daac(vc->f_48, vc, vcc))
        {
            if (old_win != active_window)
            {
                FUN_0002dcb8();
                lastfld = NULL;
                vc = g_wins[active_window].f_c;
                vcc = (swdfield_t*)((char*)vc + vc->f_4c);
                active_field = vc->f_54;
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
        if (fldfuncs[v1c->f_0] != NULL && cur_act == 0)
        {
            fldfuncs[v1c->f_0](vc, v1c);
            for (i = 0; i < vc->f_60; i++)
            {
                if (vcc[i].f_8 && vcc[i].f_8 == g_key)
                {
                    if (!usekb_flag)
                        kbactive = 0;
                    active_field = i;
                    v1c = vcc + i;
                    if (lastfld)
                    {
                        lastfld->f_1c = 0;
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
    a1->f_4 = active_field;
    a1->f_14 = vc->f_18;
    a1->f_18 = vc->f_1c;
    a1->f_8 = cur_act;
    a1->f_c = cur_cmd;
    a1->f_10 = g_key;
    switch (cur_act)
    {
    case 1:
        a1->f_1c = v1c->f_70;
        a1->f_20 = v1c->f_80;
        a1->f_28 = v1c->f_84;
        a1->f_24 = v1c->f_88;
        switch (cur_cmd)
        {
        case 1:
            SWD_GetDownField(vcc, vc->f_60);
            break;
        case 2:
            SWD_GetUpField(vcc, vc->f_60);
            break;
        case 3:
        case 5:
            SWD_GetRightField(vcc, vc->f_60);
            break;
        case 4:
        case 6:
            SWD_GetPrevField(vcc, vc->f_60);
            break;
        case 7:
            active_field = FUN_0002c9d8();
            break;
        case 8:
            active_field = FUN_0002c9ec(vcc, vc->f_60);
            break;
        case 9:
            active_field = vc->f_54;
            break;
        case 10:
            v1c->f_1c = 2;
            SWD_PutField(vc, v1c);
            v1c->f_6c ^= 1;
            if (lastfld && lastfld != v1c)
            {
                lastfld->f_1c = 0;
                SWD_PutField(vc, lastfld);
                lastfld = NULL;
            }
            GFX_DisplayUpdate();
            while (FUN_0002d1ac())
            {
                I_GetEvent();
            }
            if (kbactive || v1c->f_c)
                v1c->f_1c = 1;
            else
                v1c->f_1c = 0;
            SWD_PutField(vc, v1c);
            v28 = 1;
            break;
        }
        break;
    case 2:
        a1->f_1c = vc->f_64;
        a1->f_20 = vc->f_68;
        a1->f_28 = vc->f_6c;
        a1->f_24 = vc->f_70;
        switch (cur_cmd)
        {
        case 14:
            if (!vc->f_48)
            {
                SWD_SetWindowFlag();
                if (active_window == master_window)
                    SWD_SetWindowFlag();
                active_field = FUN_0002c9d8();
                if (lastfld)
                    lastfld->f_1c = 0;
                SWD_ShowAllWindows();
                v28 = 1;
                break;
            }
            break;
        case 15:
            if (movebuffer)
            {
                v1c->f_1c = 2;
                SWD_PutField(vc, v1c);
                if (lastfld && lastfld != v1c)
                {
                    lastfld->f_1c = 0;
                    SWD_PutField(vc, lastfld);
                    lastfld = NULL;
                }
                GFX_DisplayUpdate();
                v24 = cur_mx - vc->f_64;
                v20 = cur_my - vc->f_68;
                keyboard[28] = 0;
                lastscan = 0;
                SWD_ShowAllWindows();
                while (mouseb1)
                {
                    vc->f_64 = cur_mx - v24;
                    vc->f_68 = cur_my - v20;
                    GFX_MarkUpdate(0, 0, 320, 200);
                    memcpy(displaybuffer, movebuffer, 64000);
                    SWD_PutWin(active_window);
                    GFX_DisplayUpdate();
                }
                v1c->f_1c = 0;
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
    if (g_wins[a1].f_4 == 1)
        g_wins[a1].f_c->f_48 = a2;
}

int SWD_SetWindowXY(int a1, int a2, int a3)
{
    swd_t *va;
    va = g_wins[a1].f_c;
    va->f_64 = a2;
    va->f_64 = a3;
    return va->f_58;
}

int SWD_GetWindowXYL(int a1, int *a2, int *a3, int *a4, int *a5)
{
    swd_t* va;
    va = g_wins[a1].f_c;
    if (a2)
        *a2 = va->f_64;
    if (a3)
        *a3 = va->f_68;
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
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    t = (char*)fld + fld->f_8c;
    memcpy(a3, t, fld->f_5c);
    return fld->f_5c;
}

int SWD_SetFieldText(int a1, int a2, const char *a3)
{
    swd_t* va;
    swdfield_t *fld;
    char *t;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    t = (char*)fld + fld->f_8c;
    if (a3)
    {
        t[strlen(a3)] = 0;
        memcpy(t, a3, strlen(a3));
        //t[fld->f_5c - 1] = 0;
        //memcpy(t, a3, fld->f_5c - 1);
    }
    else
        *t = 0;
    return fld->f_5c;
}

int SWD_GetFieldValue(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    char *t;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    t = (char*)fld + fld->f_8c;
    return atoi(t);
}

int SWD_SetFieldValue(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    char *t;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    t = (char*)fld + fld->f_8c;
    sprintf(t, "%d", a3);
    return atoi(t);
}

void SWD_SetFieldSelect(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    fld->f_78 = a3;
}

int SWD_GetFieldMark(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    return fld->f_6c;
}

void SWD_SetFieldMark(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    fld->f_6c = a3;
}

int SWD_GetFieldInputOpt(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    return fld->f_18;
}

int SWD_SetFieldInputOpt(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    int o;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    o = fld->f_18;
    fld->f_18 = a3;
    return o;
}

void SWD_SetFieldItem(int a1, int a2, int a3)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    if (a3 != -1)
    {
        if (fld->f_40 != -1)
            GLB_FreeItem(fld->f_40);
        fld->f_40 = a3;
        GLB_LockItem(a3);
    }
    else
        fld->f_40 = -1;
}

int SWD_GetFieldItem(int a1, int a2)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    return fld->f_40;
}

void SWD_SetFieldName(int a1, int a2, const char *a3)
{
    swd_t* va;
    swdfield_t *fld;
    int it;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    it = GLB_GetItemID(a3);
    if (it != -1)
    {
        if (fld->f_40 != -1)
            GLB_FreeItem(fld->f_40);
        memcpy(fld->f_30, a3, 16);
        fld->f_40 = it;
        GLB_LockItem(it);
    }
}

int SWD_GetFieldItemName(int a1, int a2, char *a3)
{
    swd_t* va;
    swdfield_t *fld;
    int vc;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    vc = 10;
    memcpy(a3, fld->f_30, vc);
    return vc;
}

int SWD_SetWindowID(int a1, int a2)
{
    swd_t* va;
    int o;
    va = g_wins[a1].f_c;
    o = va->f_18;
    va->f_18 = o;
    return o;
}

int SWD_GetWindowID(int a1)
{
    swd_t* va;
    va = g_wins[a1].f_c;
    return va->f_18;
}

int FUN_0002e84c(int a1, int a2)
{
    swd_t* va;
    va = g_wins[a1].f_c;
    va->f_c = a2;
    SWD_SetWindowFlag();
    return va->f_18;
}

int SWD_SetWindowType(int a1, int a2)
{
    swd_t* va;
    int o;
    va = g_wins[a1].f_c;
    o = va->f_1c;
    va->f_1c = o;
    return o;
}

int SWD_GetWindowType(int a1)
{
    swd_t* va;
    va = g_wins[a1].f_c;
    return va->f_1c;
}

int SWD_GetFieldXYL(int a1, int a2, int* a3, int* a4, int* a5, int* a6)
{
    swd_t* va;
    swdfield_t *fld;
    va = g_wins[a1].f_c;
    fld = (swdfield_t*)((char*)va + va->f_4c) + a2;
    if (a3)
        *a3 = va->f_64 + fld->f_7c;
    if (a4)
        *a4 = va->f_68 + fld->f_80;
    if (a5)
        *a5 = fld->f_84;
    if (a6)
        *a6 = fld->f_88;
    return fld->f_84;
}
