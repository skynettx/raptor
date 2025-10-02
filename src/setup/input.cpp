#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textscreen.h"
#include "main.h"
#include "input.h"

extern "C" {
#include "txt_keyinput.h"
#include "txt_mouseinput.h"
#include "txt_joyinput.h"
}

int ControllerType;

int key_up = KEY_UPARROW;
int key_down = KEY_DOWNARROW;
int key_left = KEY_LEFTARROW;
int key_right = KEY_RIGHTARROW;
int key_fire = KEY_RCTRL;
int key_special = KEY_LALT;
int key_mega = ' ';
int convertflagkey;
int writeflagkey;
int key_up_convert, key_down_convert, key_left_convert, key_right_convert, key_fire_convert, key_special_convert, key_mega_convert;
static int* controls[] = { &key_up, &key_down, &key_left, &key_right, &key_fire, &key_special, &key_mega, NULL };

static const int scantokey[128] =
{
    0  ,    27,     '1',    '2',    '3',    '4',    '5',    '6',
    '7',    '8',    '9',    '0',    '-',    '=',    KEY_BACKSPACE, 9,
    'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',
    'o',    'p',    '[',    ']',    13,		KEY_RCTRL, 'a',    's',
    'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',
    '\'',   '`',    KEY_RSHIFT,'\\',   'z',    'x',    'c',    'v',
    'b',    'n',    'm',    ',',    '.',    '/',    KEY_RSHIFT,KEYP_MULTIPLY,
    KEY_RALT,  ' ',  KEY_CAPSLOCK,KEY_F1,  KEY_F2,   KEY_F3,   KEY_F4,   KEY_F5,
    KEY_F6,   KEY_F7,   KEY_F8,   KEY_F9,   KEY_F10,  /*KEY_NUMLOCK?*/KEY_PAUSE,KEY_SCRLCK,KEY_HOME,
    KEY_UPARROW,KEY_PGUP,KEY_MINUS,KEY_LEFTARROW,KEYP_5,KEY_RIGHTARROW,KEYP_PLUS,KEY_END,
    KEY_DOWNARROW,KEY_PGDN,KEY_INS,KEY_DEL,0,   0,      0,      KEY_F11,
    KEY_F12,  0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      0,      0,
    0,      0,      0,      0,      0,      0,      KEY_PRTSCR, 0
};

int mousebfire, mousebchweapon, mousebmega;
int mousebfireout, mousebchweaponout, mousebmegaout;
int writeflagmouse;

static int* all_mouse_buttons[] = {
    &mousebfire,
    &mousebchweapon,
    &mousebmega,
};

int joybfire, joybchweapon, joybmega, joybspeed;
int joybfireout, joybchweaponout, joybmegaout;
int writeflagjoy, writeflagjoybfire, writeflagjoybchweapon, writeflagjoybmega;

txt_window_t* getcontrolkeyboardwindow;
txt_window_t* getcontrolmousewindow;
txt_window_t* getcontroljoystickwindow;

void GetControl(TXT_UNCAST_ARG(widget), void* user_data)
{
    if (strcmp((char*)user_data, "Keyboard") == 0)
    {
        ControllerType = 1000;
        textbox_control = (char*)"Keyboard                    ";
    }
    if (strcmp((char*)user_data, "Mouse") == 0)
    {
        ControllerType = 1;
        textbox_control = (char*)"Mouse                       ";
    }
    if (strcmp((char*)user_data, "Joystick") == 0)
    {
        ControllerType = 2;
        textbox_control = (char*)"Joystick                    ";
    }

    TXT_ClearTable(infotable);
    infotable = TXT_NewTable(2);
    TXT_AddWidgets(infotable,
        TXT_NewLabel("Control Type  : "),
        TXT_NewLabel(textbox_control),
        TXT_NewLabel("Music Card    : "),
        TXT_NewLabel(textbox_musiccard),
        TXT_NewLabel("Sound FX Card : "),
        TXT_NewLabel(textbox_soundfx),
        NULL);
    TXT_AddWidget(infowindow, infotable);
}
////////////////////////////////////////////////Select ControlButtonConfigKeyboard///////////////////////////////////////////////////
static int VarInGroup(int* variable, int** group)
{
    unsigned int i;

    for (i = 0; group[i] != NULL; ++i)
    {
        if (group[i] == variable)
        {
            return 1;
        }
    }

    return 0;
}

static void CheckKeyGroup(int* variable, int** group)
{
    unsigned int i;

    // Don't check unless the variable is in this group.

    if (!VarInGroup(variable, group))
    {
        return;
    }

    // If another variable has the same value as the new value, reset it.

    for (i = 0; group[i] != NULL; ++i)
    {
        if (*variable == *group[i] && group[i] != variable)
        {
            // A different key has the same value.  Clear the existing
            // value. This ensures that no two keys can have the same
            // value.

            *group[i] = 0;
        }
    }
 }

static void KeySetCallback(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(variable))
{
    TXT_CAST_ARG(int, variable);

    CheckKeyGroup(variable, controls);
}

void ConvertKey(TXT_UNCAST_ARG(widget), void* user_data)
{
    int s;

    writeflagkey = 1;

    for (s = 0; s < 128; ++s)
    {
        if (scantokey[s] == key_up)
        {
            key_up_convert = s;
            break;
        }
    }
    for (s = 0; s < 128; ++s)
    {
        if (scantokey[s] == key_down)
        {
            key_down_convert = s;
            break;
        }
    }
    for (s = 0; s < 128; ++s)
    {
        if (scantokey[s] == key_left)
        {
            key_left_convert = s;
            break;
        }
    }
    for (s = 0; s < 128; ++s)
    {
        if (scantokey[s] == key_right)
        {
            key_right_convert = s;
            break;
        }
    }
    for (s = 0; s < 128; ++s)
    {
        if (scantokey[s] == key_fire)
        {
            key_fire_convert = s;
            break;
        }
    }
    for (s = 0; s < 128; ++s)
    {
        if (scantokey[s] == key_special)
        {
            key_special_convert = s;
            break;
        }
    }
    for (s = 0; s < 128; ++s)
    {
        if (scantokey[s] == key_mega)
        {
            key_mega_convert = s;
            break;
        }
    }
}

static void AddKeyControl(TXT_UNCAST_ARG(table), const char* name, int* var)
{
    TXT_CAST_ARG(txt_table_t, table);
    txt_key_input_t* key_input;

    TXT_AddWidget(table, TXT_NewSpecialLabel(name));
    key_input = TXT_NewKeyInput(var);
    TXT_AddWidget(table, key_input);

    TXT_SignalConnect(key_input, "set", KeySetCallback, var);

    if (strcmp(name, " Up                ") == 0)
        TXT_SetHelpLabel(key_input, "Move ship forward");
        
    if (strcmp(name, " Down              ") == 0)
        TXT_SetHelpLabel(key_input, "Move ship backwards");
        
    if (strcmp(name, " Left              ") == 0)
        TXT_SetHelpLabel(key_input, "Move ship to the left");
        
    if (strcmp(name, " Right             ") == 0)
        TXT_SetHelpLabel(key_input, "Move ship to the right");
        
    if (strcmp(name, " Fire              ") == 0)
        TXT_SetHelpLabel(key_input, "Fire main weapons");
        
    if (strcmp(name, " Change Special    ") == 0)
        TXT_SetHelpLabel(key_input, "Fire special weapons the you pickup/buy");
        
    if (strcmp(name, " Mega Bomb         ") == 0)
        TXT_SetHelpLabel(key_input, "Change to next available special weapon");
}

void GetControlKeyboard(TXT_UNCAST_ARG(widget), void* user_data)
{
    txt_window_action_t* close_button;
    txt_window_action_t* accept_button;
    txt_window_action_t* select_button;

    if (!convertflagkey)
    {
        key_up = scantokey[keymoveup];
        key_down = scantokey[keymovedown];
        key_left = scantokey[keymoveleft];
        key_right = scantokey[keymoveright];
        key_fire = scantokey[keyfire];
        key_special = scantokey[keyspecial];
        key_mega = scantokey[keymega];
        convertflagkey = 1;
    }

    getcontrolkeyboardwindow = TXT_NewWindow("Keyboard Configuration           ");
    TXT_SetWindowPosition(getcontrolkeyboardwindow, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 3);
    
    TXT_AddWidget(getcontrolkeyboardwindow, TXT_TABLE_EMPTY);
    
    TXT_SetTableColumns(getcontrolkeyboardwindow, 2);
    TXT_SetColumnWidths(getcontrolkeyboardwindow, 4, 3);
    TXT_AddWidget(getcontrolkeyboardwindow, TXT_NewSpecialSeparator(" Movement ", 6, 6, TXT_COLOR_BRIGHT_BLUE));
    AddKeyControl(getcontrolkeyboardwindow, " Up                ", &key_up);
    AddKeyControl(getcontrolkeyboardwindow, " Down              ", &key_down);
    AddKeyControl(getcontrolkeyboardwindow, " Left              ", &key_left);
    AddKeyControl(getcontrolkeyboardwindow, " Right             ", &key_right);
    
    TXT_AddWidget(getcontrolkeyboardwindow, TXT_NewStrut(0, 2));

    TXT_AddWidget(getcontrolkeyboardwindow, TXT_NewSpecialSeparator(" Action ", 6, 5, TXT_COLOR_BRIGHT_BLUE));
    AddKeyControl(getcontrolkeyboardwindow, " Fire              ", &key_fire);
    AddKeyControl(getcontrolkeyboardwindow, " Change Special    ", &key_special);
    AddKeyControl(getcontrolkeyboardwindow, " Mega Bomb         ", &key_mega);

    TXT_AddWidget(getcontrolkeyboardwindow, TXT_NewStrut(0, 1));

    accept_button = TXT_NewWindowAction(KEY_F10, "Accept");
    select_button = TXT_NewWindowAction(KEY_ENTER, "Select");
    close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");

    TXT_SetHelpLabel(close_button, " Press ESC to Abort");
    TXT_SetHelpLabel(accept_button, " Press F10 to Accept");
    TXT_SetHelpLabel(select_button, " Press ENTER to Select");

    TXT_SignalConnect(close_button, "pressed", ClosePwnBox, getcontrolkeyboardwindow);
    
    TXT_SignalConnect(select_button, "pressed", WindowSelect, getcontrolkeyboardwindow);

    TXT_SignalConnect(accept_button, "pressed", ClosePwnBox, getcontrolkeyboardwindow);
    TXT_SignalConnect(accept_button, "pressed", ConvertKey, NULL);
    
    TXT_SetWindowAction(getcontrolkeyboardwindow, TXT_HORIZ_LEFT, close_button);
    TXT_SetWindowAction(getcontrolkeyboardwindow, TXT_HORIZ_CENTER, select_button);
    TXT_SetWindowAction(getcontrolkeyboardwindow, TXT_HORIZ_RIGHT, accept_button);
}
////////////////////////////////////////////////Select ControlButtonConfigMouse///////////////////////////////////////////////////
static void MouseSetCallback(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(variable))
{
    TXT_CAST_ARG(int, variable);
    unsigned int i;

    // Check if the same mouse button is used for a different action
    // If so, set the other action(s) to -1 (unset)

    for (i = 0; i < arrlen(all_mouse_buttons); ++i)
    {
        if (*all_mouse_buttons[i] == *variable
            && all_mouse_buttons[i] != variable)
        {
            *all_mouse_buttons[i] = -1;
        }
    }
}

static void AddMouseControl(TXT_UNCAST_ARG(table), const char* label, int* var)
{
    TXT_CAST_ARG(txt_table_t, table);
    txt_mouse_input_t* mouse_input;

    TXT_AddWidget(table, TXT_NewLabel(label));

    mouse_input = TXT_NewMouseInput(var);
    TXT_AddWidget(table, mouse_input);

    TXT_SignalConnect(mouse_input, "set", MouseSetCallback, var);

    if (strcmp(label, " Fire            ") == 0)
        TXT_SetHelpLabel(mouse_input, "Fire main weapons");
        
    if (strcmp(label, " Change Weapon   ") == 0)
        TXT_SetHelpLabel(mouse_input, "Change to next available special weapon");
        
    if (strcmp(label, " Mega Bomb       ") == 0)
        TXT_SetHelpLabel(mouse_input, "Launch a Mega Bomb");
}

void SaveMouseConfig(TXT_UNCAST_ARG(widget), void* user_data)
{
    txt_window_t* window;
    txt_window_action_t* close_button;

    if ((mousebfire > 2) || (mousebchweapon > 2) || (mousebmega > 2))
    {
        window = TXT_CustomMessageBox("Error", ("Only LEFT BUTTON, RIGHT BUTTON and MID BUTTON are supported!\n"
                             "Please reconfigure mouse buttons before save!"), TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_RED, TXT_COLOR_RED, TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_BRIGHT_WHITE);
        
        close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
        
        TXT_SetHelpLabel(close_button, " Press ESC to Abort");

        TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);

        TXT_SetWindowAction(window, TXT_HORIZ_CENTER, close_button);
       
        writeflagmouse = 0;

        return;
    }
    
    writeflagmouse = 1;
}

void GetControlMouse(TXT_UNCAST_ARG(widget), void* user_data)
{
    txt_window_action_t* close_button;
    txt_window_action_t* accept_button;
    txt_window_action_t* select_button;

    getcontrolmousewindow = TXT_NewWindow("Mouse Configuration              ");
    TXT_SetWindowPosition(getcontrolmousewindow, TXT_HORIZ_CENTER, TXT_VERT_TOP, 40, 5);

    TXT_AddWidget(getcontrolmousewindow, TXT_NewStrut(0, 1));
    
    TXT_SetTableColumns(getcontrolmousewindow, 2);

    AddMouseControl(getcontrolmousewindow, " Fire            ", &mousebfire);
    AddMouseControl(getcontrolmousewindow, " Change Weapon   ", &mousebchweapon);
    AddMouseControl(getcontrolmousewindow, " Mega Bomb       ", &mousebmega);

    TXT_AddWidget(getcontrolmousewindow, TXT_NewStrut(0, 1));

    accept_button = TXT_NewWindowAction(KEY_F10, "Accept");
    select_button = TXT_NewWindowAction(KEY_ENTER, "Select");
    close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");

    TXT_SetHelpLabel(close_button, " Press ESC to Abort");
    TXT_SetHelpLabel(accept_button, " Press F10 to Accept");
    TXT_SetHelpLabel(select_button, " Press ENTER to Select");
    
    TXT_SignalConnect(close_button, "pressed", ClosePwnBox, getcontrolmousewindow);
    
    TXT_SignalConnect(select_button, "pressed", WindowSelect, getcontrolmousewindow);

    TXT_SignalConnect(accept_button, "pressed", ClosePwnBox, getcontrolmousewindow);
    TXT_SignalConnect(accept_button, "pressed", SaveMouseConfig, NULL);

    TXT_SetWindowAction(getcontrolmousewindow, TXT_HORIZ_LEFT, close_button);
    TXT_SetWindowAction(getcontrolmousewindow, TXT_HORIZ_CENTER, select_button);
    TXT_SetWindowAction(getcontrolmousewindow, TXT_HORIZ_RIGHT, accept_button);
}
////////////////////////////////////////////////Select ControlButtonConfigJoystick////////////////////////////////////////////////
static void AddJoystickControl(TXT_UNCAST_ARG(table), const char* label, int* var)
{
    TXT_CAST_ARG(txt_table_t, table);
    txt_joystick_input_t* joy_input;

    TXT_AddWidget(table, TXT_NewLabel(label));

    joy_input = TXT_NewJoystickInput(var);

    TXT_AddWidget(table, joy_input);

    if (strcmp(label, "    Fire           ") == 0)
        TXT_SetHelpLabel(joy_input, "Fire main weapons");
        
    if (strcmp(label, "    Change Special ") == 0)
        TXT_SetHelpLabel(joy_input, "Change to the next available special weapon");
        
    if (strcmp(label, "    Mega           ") == 0)
        TXT_SetHelpLabel(joy_input, "Fires a Mega Bomb");
}

void SaveJoyConfig(TXT_UNCAST_ARG(widget), void* user_data)
{
    txt_window_t* window;
    txt_window_action_t* close_button;
    
    if ((joybfireout > 3) || (joybchweaponout > 3) || (joybmegaout > 3))
    {
        window = TXT_CustomMessageBox("Error", ("Only BUTTON 1, BUTTON 2, BUTTON 3 and BUTTON 4 are supported!\n"
                       "Please reconfigure joystick/gamepad buttons before save!"), TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_RED, TXT_COLOR_RED, TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_BRIGHT_WHITE, TXT_COLOR_BRIGHT_WHITE);
        
        close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");

        TXT_SetHelpLabel(close_button, " Press ESC to Abort");

        TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);

        TXT_SetWindowAction(window, TXT_HORIZ_CENTER, close_button);
        
        writeflagjoy = 0;

        return;
    }

    writeflagjoy = 1;
}

void GetControlJoystick(TXT_UNCAST_ARG(widget), void* user_data)
{
    txt_window_action_t* close_button;
    txt_window_action_t* accept_button;
    txt_window_action_t* select_button;

    if (!joybfireout)
    joybfireout = joybfire;
    if (!joybchweaponout)
    joybchweaponout = joybchweapon;
    if (!joybmegaout)
    joybmegaout = joybmega;

    getcontroljoystickwindow = TXT_NewWindow("Joystick/Gamepad Config          ");
    TXT_SetWindowPosition(getcontroljoystickwindow, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 7);

    TXT_AddWidget(getcontroljoystickwindow, TXT_NewStrut(0, 1));
    
    TXT_SetTableColumns(getcontroljoystickwindow, 2);

    AddJoystickControl(getcontroljoystickwindow, "    Fire           ", &joybfire);
    AddJoystickControl(getcontroljoystickwindow, "    Change Special ", &joybchweapon);
    AddJoystickControl(getcontroljoystickwindow, "    Mega           ", &joybmega);

    TXT_AddWidget(getcontroljoystickwindow, TXT_NewStrut(0, 1));
    
    accept_button = TXT_NewWindowAction(KEY_F10, "Accept");
    select_button = TXT_NewWindowAction(KEY_ENTER, "Select");
    close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");

    TXT_SetHelpLabel(close_button, " Press ESC to Abort");
    TXT_SetHelpLabel(accept_button, " Press F10 to Accept");
    TXT_SetHelpLabel(select_button, " Press ENTER to Select");

    TXT_SignalConnect(close_button, "pressed", ClosePwnBox, getcontroljoystickwindow);
    
    TXT_SignalConnect(select_button, "pressed", WindowSelect, getcontroljoystickwindow);
    
    TXT_SignalConnect(accept_button, "pressed", ClosePwnBox, getcontroljoystickwindow);
    TXT_SignalConnect(accept_button, "pressed", SaveJoyConfig, NULL);

    TXT_SetWindowAction(getcontroljoystickwindow, TXT_HORIZ_LEFT, close_button);
    TXT_SetWindowAction(getcontroljoystickwindow, TXT_HORIZ_CENTER, select_button);
    TXT_SetWindowAction(getcontroljoystickwindow, TXT_HORIZ_RIGHT, accept_button);
}
