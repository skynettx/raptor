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
    int version;                 // VERSION NUMBER ( not used )
    int swdsize;                 // SIZE OF WIN/FIELD AND TEXT ( not used )
    int arrowflag;               // Use Arrow Keys ( TRUE/FALSE )
    int display;                 // DISPLAY FLAG
    int opt3;                    // OPTION 3 ( not used )
    int opt4;                    // OPTION 4 ( not used )
    int id;                      // WINDOW ID NUMBER
    int type;                    // WINDOW TYPE NUMBER
    char name[16];               // TEXT NAME OF WINDOW ( NOT DISPLAYED )
    char item_name[16];          // TEXT NAME OF ITEM
    int item;                    // ITEM ID NUMBER
    int picflag;                 // FILL/TEXTURE/PICTURE
    int lock;                    // TRUE = cannot goto other windows
    int fldofs;                  // OFFSET IN BYTES TO FIRST FIELD
    int txtofs;                  // OFFSET IN BYTES TO TEXT AREA ( not used )
    int firstfld;                // FIELD TO GOTO FIRST
    int opt;                     // WINDOW TYPE
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
    char *sptr;                  // SEG POINTER TO SAVE INFO
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
    int objactive;
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

void SWD_Install(int moveflag);
void SWD_End(void);
void SWD_Dialog(wdlg_t *swd_dlg);
swd_t* SWD_ReformatFieldData(swd_t* header, int handle);
int SWD_InitMasterWindow(int handle);
int SWD_ShowAllWindows(void);
void SWD_SetWindowPtr(int handle);
void SWD_SetFieldSelect(int handle, int field_id, int opt);
void SWD_SetFieldItem(int handle, int field_id, int item);
int SWD_SetFieldText(int handle, int field_id, const char *in_text);
void SWD_DestroyWindow(int handle);
int SWD_InitWindow(int handle);
void SWD_SetActiveField(int handle, int field_id);
int SWD_GetFieldXYL(int handle, int field_id, int* x, int* y, int* lx, int* ly);
void SWD_SetWinDrawFunc(int handle, void (*infunc)(wdlg_t*));
void SWD_SetFieldPtr(int handle, int field);
int SWD_GetFieldText(int handle, int field_id, char *out_text);
int SWD_GetFieldItem(int handle, int field_id);
void SWD_SetClearFlag(int inflag);
