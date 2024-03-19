#pragma once
#include "gfxapi.h"

typedef enum
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
}CMD;

typedef enum
{
    S_IDLE,
    S_FLD_COMMAND,
    S_WIN_COMMAND,
    S_FIRSTCALL,
    S_REDRAW,
    S_UPDATE,
    S_END
}ACT;

typedef enum
{
    R_IDLE,
    R_CLOSED,
    R_CLOSE_ALL,
    R_END_DIALOG
}FCMD;

typedef enum
{
    FILL,
    TEXTURE,
    PICTURE,
    SEE_THRU,
    INVISABLE
}DSTYLE;

typedef enum
{
    NORMAL,
    UP,
    DOWN
}BUTTON;

typedef enum
{
    I_NORM,
    I_TOUPPER,
    I_NUMERIC
}INPUTOPT;

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

typedef struct
{
    little_int32_t version;                 // VERSION NUMBER ( not used )
    little_int32_t swdsize;                 // SIZE OF WIN/FIELD AND TEXT ( not used )
    little_int32_t arrowflag;               // Use Arrow Keys ( TRUE/FALSE )
    little_int32_t display;                 // DISPLAY FLAG
    little_int32_t opt3;                    // OPTION 3 ( not used )
    little_int32_t opt4;                    // OPTION 4 ( not used )
    little_int32_t id;                      // WINDOW ID NUMBER
    little_int32_t type;                    // WINDOW TYPE NUMBER
    char name[16];                          // TEXT NAME OF WINDOW ( NOT DISPLAYED )
    char item_name[16];                     // TEXT NAME OF ITEM
    little_int32_t item;                    // ITEM ID NUMBER
    little_int32_t picflag;                 // FILL/TEXTURE/PICTURE
    little_int32_t lock;                    // TRUE = cannot goto other windows
    little_int32_t fldofs;                  // OFFSET IN BYTES TO FIRST FIELD
    little_int32_t txtofs;                  // OFFSET IN BYTES TO TEXT AREA ( not used )
    little_int32_t firstfld;                // FIELD TO GOTO FIRST
    little_int32_t opt;                     // WINDOW TYPE
    little_int32_t color;                   // COLOR OF WINDOW
    little_int32_t numflds;                 // NUMBER OF FIELDS
    little_int32_t x;                       // X POSITON ON SCREEN
    little_int32_t y;                       // Y POSITION ON SCREEN
    little_int32_t lx;                      // WIDTH IN PIXELS
    little_int32_t ly;                      // HEIGHT IN PIXELS
    little_int32_t shadow;                  // SHADOW TRUE/FALSE
}SWIN;

typedef struct
{
    little_int32_t opt;                     // FIELD TYPE
    little_int32_t id;                      // FIELD ID
    little_int32_t hotkey;                  // SCAN CODE OF HOT KEY
    little_int32_t kbflag;                  // TRUE if field should be KBACTIVE
    little_int32_t opt3;                    // not used
    little_int32_t opt4;                    // not used
    little_int32_t input_opt;               // OPTIONS used in INPUT FIELDS
    little_int32_t bstatus;                 // BUTTON STATUS NORMAL/UP/DOWN
    char name[16];                          // TEXT NAME OF FIELD ( NOT DISPLAYED )
    char item_name[16];                     // TEXT NAME OF ITEM #
    little_int32_t item;                    // ITEM ID NUMBER
    char font_name[16];                     // FONT .GLB NAME
    little_int32_t fontid;                  // FONT NUMBER
    little_int32_t fontbasecolor;           // FONT BASE COLOR
    little_int32_t maxchars;                // MAXCHARS IN FIELD TEXT
    little_int32_t picflag;                 // PICTURE TRUE/FALSE
    little_int32_t color;                   // COLOR OF FIELD
    little_int32_t lite;                    // HIGHLIGHT COLOR
    little_int32_t mark;                    // FIELD MARK ( TRUE/FLASE )
    little_int32_t saveflag;                // MEM TO SAVE PIC UNDER FIELD ( Y/N )
    little_int32_t shadow;                  // SHADOW ON/OFF
    little_int32_t selectable;              // SELECTABLE ON/OFF
    little_int32_t x;                       // X POSITION ON SCREEN
    little_int32_t y;                       // Y POSITION ON SCREEN
    little_int32_t lx;                      // WIDTH IN PIXELS
    little_int32_t ly;                      // HEIGHT IN PIXELS
    little_int32_t txtoff;                  // OFFSET TO TEXT DATA ( BYTES )
    char *sptr;                             // SEG POINTER TO SAVE INFO
}SFIELD;

typedef struct
{
    little_int32_t opt;                     // FIELD TYPE
    little_int32_t id;                      // FIELD ID
    little_int32_t hotkey;                  // SCAN CODE OF HOT KEY
    little_int32_t kbflag;                  // TRUE if field should be KBACTIVE
    little_int32_t opt3;                    // not used
    little_int32_t opt4;                    // not used
    little_int32_t input_opt;               // OPTIONS used in INPUT FIELDS
    little_int32_t bstatus;                 // BUTTON STATUS NORMAL/UP/DOWN
    char name[16];                          // TEXT NAME OF FIELD ( NOT DISPLAYED )
    char item_name[16];                     // TEXT NAME OF ITEM #
    little_int32_t item;                    // ITEM ID NUMBER
    char font_name[16];                     // FONT .GLB NAME
    little_int32_t fontid;                  // FONT NUMBER
    little_int32_t fontbasecolor;           // FONT BASE COLOR
    little_int32_t maxchars;                // MAXCHARS IN FIELD TEXT
    little_int32_t picflag;                 // PICTURE TRUE/FALSE
    little_int32_t color;                   // COLOR OF FIELD
    little_int32_t lite;                    // HIGHLIGHT COLOR
    little_int32_t mark;                    // FIELD MARK ( TRUE/FLASE )
    little_int32_t saveflag;                // MEM TO SAVE PIC UNDER FIELD ( Y/N )
    little_int32_t shadow;                  // SHADOW ON/OFF
    little_int32_t selectable;              // SELECTABLE ON/OFF
    little_int32_t x;                       // X POSITION ON SCREEN
    little_int32_t y;                       // Y POSITION ON SCREEN
    little_int32_t lx;                      // WIDTH IN PIXELS
    little_int32_t ly;                      // HEIGHT IN PIXELS
    little_int32_t txtoff;                  // OFFSET TO TEXT DATA ( BYTES )
    little_int32_t PlaceHolder;             // PLACEHOLDER TO KEEP SIZE
}SFIELD32;

typedef struct
{
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
}SWD_DLG;

typedef struct
{
    int gitem;                   // GLB ITEM ID
    int flag;                    // TRUE = in use ,FALSE = not in use
    int viewflag;                // TRUE = has viewarea(s) FALSE = none
    SWIN *win;                   // POINTER TO WINDOW
}SWD_WIN;

extern int usekb_flag;
extern int g_button_flag;
extern unsigned int fi_joy_count;
extern bool fi_sec_field;

void SWD_Install(int moveflag);
void SWD_End(void);
void SWD_Dialog(SWD_DLG *swd_dlg);
SWIN* SWD_ReformatFieldData(SWIN *header, int handle);
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
void SWD_SetWinDrawFunc(int handle, void (*infunc)(SWD_DLG*));
void SWD_SetFieldPtr(int handle, int field);
int SWD_GetFieldText(int handle, int field_id, char *out_text);
int SWD_GetFieldItem(int handle, int field_id);
void SWD_SetClearFlag(int inflag);
