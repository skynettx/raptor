#pragma once
#include "textscreen.h"

#define arrlen(s) (sizeof(s)/sizeof(*s))

extern txt_window_t* getcontrolkeyboardwindow;
extern txt_window_t* getcontrolmousewindow;
extern txt_window_t* getcontroljoystickwindow;

extern int ControllerType;
extern int key_up_convert, key_down_convert, key_left_convert, key_right_convert, key_fire_convert, key_special_convert, key_mega_convert;
extern int writeflagkey;

extern int mousebfire, mousebchweapon, mousebmega;
extern int mousebfireout, mousebchweaponout, mousebmegaout;
extern int writeflagmouse;

extern int joybfire, joybchweapon, joybmega, joybspeed;
extern int joybfireout, joybchweaponout, joybmegaout;
extern int writeflagjoy, writeflagjoybfire, writeflagjoybchweapon, writeflagjoybmega;

void GetControl(TXT_UNCAST_ARG(widget), void* user_data);
void GetControlKeyboard(TXT_UNCAST_ARG(widget), void* user_data);
void GetControlMouse(TXT_UNCAST_ARG(widget), void* user_data);
void GetControlJoystick(TXT_UNCAST_ARG(widget), void* user_data);
