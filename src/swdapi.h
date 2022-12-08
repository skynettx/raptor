#pragma once

#include "gfxapi.h"

enum CMD
{
    C_IDLE,
    F_DOWN,
    F_UP,
    F_NEXT,
    F_PREV,
    F_RIGHT,
    F_LEFT,
    F_TOP,
    F_BOTTOM,
    F_FIRST,
    F_SELECT,
    F_OBJ_AREA,
    F_VIEW_AREA,
    W_NEW,
    W_NEXT,
    W_MOVE,
    W_CLOSE,
    W_CLOSE_ALL
};

enum ACT
{
    S_IDLE,
    S_FLD_COMMAND,
    S_WIN_COMMAND,
    S_FIRSTCALL,
    S_REDRAW,
    S_UPDATE,
    S_END
};

enum FCMD
{
    R_IDLE,
    R_CLOSED,
    R_CLOSE_ALL,
    R_END_DIALOG
};

enum DSTYLE 
{
    FILL,
    TEXTURE,
    PICTURE,
    SEE_THRU,
    INVISABLE
};

enum BUTTON
{ 
    NORMAL, 
    UP, 
    DOWN 
};

enum INPUTOPT
{ 
    I_NORM, 
    I_TOUPPER, 
    I_NUMERIC 
};

#define FLD_OFF 0       // 0
#define FLD_TEXT 1      // 1
#define FLD_BUTTON 2    // 2 *
#define FLD_INPUT 3     // 3 *
#define FLD_MARK 4      // 5 *
#define FLD_CLOSE 5     // 6
#define FLD_DRAGBAR 6   // 7 *
#define FLD_BUMPIN 7    // 8
#define FLD_BUMPOUT 8   // 9
#define FLD_ICON 9      // 4
#define FLD_OBJAREA 10  // 10
#define FLD_VIEWAREA 11 // 11

#define SWD_FILENUM 1

// basic format of a window data object
//
//  SWIN structure
//  SFIELDS..
//  TEXT AREA for window
//

#define MAX_WINDOWS 12
#define MAX_FONTS 2

struct swd_t
{
    int f_0;     //not used
    int f_4;     //not used
    int arrowflag;                // Use Arrow Keys ( TRUE/FALSE )
    int display;                  // DISPLAY FLAG
    int f_10;    //not used
    int f_14;    //not used
    int id;                       // WINDOW ID NUMBER
    int type;                     // WINDOW TYPE NUMBER
    //int f_20;
    //int f_24;
    //int f_28;
    //int f_2c;
    char Name[16];
    char item_name[16];          // TEXT NAME OF ITEM
    int item;                    // ITEM ID NUMBER
    int picflag;                 // FILL/TEXTURE/PICTURE
    int f_48;
    int fldofs;                  // OFFSET IN BYTES TO FIRST FIELD
    int f_50;    //not used
    int firstfld;                // FIELD TO GOTO FIRST
    int f_58;
    int color;                   // COLOR OF WINDOW
    int numflds;                 // NUMBER OF FIELDS
    int x;                       // X POSITON ON SCREEN
    int y;                       // Y POSITION ON SCREEN
    int lx;                      // WIDTH IN PIXELS
    int ly;                      // HEIGHT IN PIXELS
    int shadow;                  // SHADOW TRUE/FALSE
};

struct swdfield_t {
    int opt;                     // FIELD TYPE
    int id;                      // FIELD ID
    int hotkey;                  // SCAN CODE OF HOT KEY
    int kbflag;                  // TRUE if field should be KBACTIVE
    int opt3;                    // not used
    int opt4;                    // not used
    int input_opt;               // OPTIONS used in INPUT FIELDS
    int bstatus;                 // BUTTON STATUS NORMAL/UP/DOWN
    char name[16];               // TEXT NAME OF FIELD ( NOT DISPLAYED )
    char item_name[16];          // TEXT NAME OF ITEM #
    int item;                    // ITEM ID NUMBER
    char font_name[16];          // FONT .GLB NAME
    int fontid;                  // FONT NUMBER
    int fontbasecolor;           // FONT BASE COLOR
    int maxchars;                // MAXCHARS IN FIELD TEXT
    int picflag;                 // PICTURE TRUE/FALSE
    int color;                   // COLOR OF FIELD
    int lite;                    // HIGHLIGHT COLOR
    int mark;                    // FIELD MARK ( TRUE/FLASE )
    int saveflag;                // MEM TO SAVE PIC UNDER FIELD ( Y/N )
    int shadow;                  // SHADOW ON/OFF
    int selectable;              // SELECTABLE ON/OFF
    int x;                       // X POSITION ON SCREEN
    int y;                       // Y POSITION ON SCREEN
    int lx;                      // WIDTH IN PIXELS
    int ly;                      // HEIGHT IN PIXELS
    int txtoff;                  // OFFSET TO TEXT DATA ( BYTES )
    texture_t *sptr;             // SEG POINTER TO SAVE INFO
}; 

struct swdfield_32_t {
    int opt;                     // FIELD TYPE
    int id;                      // FIELD ID
    int hotkey;                  // SCAN CODE OF HOT KEY
    int kbflag;                  // TRUE if field should be KBACTIVE
    int opt3;                    // not used
    int opt4;                    // not used
    int input_opt;               // OPTIONS used in INPUT FIELDS
    int bstatus;                 // BUTTON STATUS NORMAL/UP/DOWN
    char name[16];               // TEXT NAME OF FIELD ( NOT DISPLAYED )
    char item_name[16];          // TEXT NAME OF ITEM #
    int item;                    // ITEM ID NUMBER
    char font_name[16];          // FONT .GLB NAME
    int fontid;                  // FONT NUMBER
    int fontbasecolor;           // FONT BASE COLOR
    int maxchars;                // MAXCHARS IN FIELD TEXT
    int picflag;                 // PICTURE TRUE/FALSE
    int color;                   // COLOR OF FIELD
    int lite;                    // HIGHLIGHT COLOR
    int mark;                    // FIELD MARK ( TRUE/FLASE )
    int saveflag;                // MEM TO SAVE PIC UNDER FIELD ( Y/N )
    int shadow;                  // SHADOW ON/OFF
    int selectable;              // SELECTABLE ON/OFF
    int x;                       // X POSITION ON SCREEN
    int y;                       // Y POSITION ON SCREEN
    int lx;                      // WIDTH IN PIXELS
    int ly;                      // HEIGHT IN PIXELS
    int txtoff;                  // OFFSET TO TEXT DATA ( BYTES )
    int PlaceHolder;             // PLACEHOLDER TO KEEP SIZE
}; 

struct wdlg_t {
    int window;
    int field;
    int cur_act;
    int cur_cmd;
    int keypress;
    int id;
    int type;
    int x;
    int y;
    int height;
    int width;
    int f_2c;
    int viewactive;
    int sfield;
    int sx;
    int sy;
};

struct window_t {
    int gitem;                   // GLB ITEM ID
    int flag;                    // TRUE = in use ,FALSE = not in use
    int viewflag;                // TRUE = has viewarea(s) FALSE = none
    swd_t *win;                  // POINTER TO WINDOW
};

extern int usekb_flag;
extern int g_button_flag;
extern unsigned int fi_joy_count;
extern bool fi_sec_field;

void SWD_Install(int a1);
void SWD_End(void);
void SWD_Dialog(wdlg_t *a1);
swd_t* SWD_ReformatFieldData(swd_t* swdData, int a1);
int SWD_InitMasterWindow(int a1);
int SWD_ShowAllWindows(void);
void SWD_SetWindowPtr(int a1);
void SWD_SetFieldSelect(int a1, int a2, int a3);
void SWD_SetFieldItem(int a1, int a2, int a3);
int SWD_SetFieldText(int a1, int a2, const char *a3);
void SWD_DestroyWindow(int a1);
int SWD_InitWindow(int a1);
void SWD_SetActiveField(int a1, int a2);
int SWD_GetFieldXYL(int a1, int a2, int* a3, int* a4, int* a5, int* a6);
void SWD_SetWinDrawFunc(int a1, void (*a2)(wdlg_t*));
void SWD_SetFieldPtr(int a1, int a2);
int SWD_GetFieldText(int a1, int a2, char *a3);
int SWD_GetFieldItem(int a1, int a2);
void SWD_SetClearFlag(int a1);
