#include "SDL.h"
#define MAX_CONTROLLERS 4

typedef enum
{
	JOYSTICKX,
	JOYSTICKY,
	JOYTRIGGERLEFT,
	JOYTRIGGERRIGHT,
	JOYUP,
	JOYDOWN,
	JOYLEFT,
	JOYRIGHT,
	JOYSTART,
	JOYBACK,
	JOYLEFTSHOULDER,
	JOYRIGHTSHOULDER,
	JOYA,
	JOYB,
	JOYX,
	JOYY
} joy_buttons_axis;

typedef enum
{
	JOYCONVERTA,
	JOYCONVERTB,
	JOYCONVERTX,
	JOYCONVERTY
} joy_buttons_convert;

extern SDL_GameController* ControllerHandles[MAX_CONTROLLERS];

extern int joy_ack;

extern int AButtonconvert, BButtonconvert, XButtonconvert, YButtonconvert;

void IPT_CalJoy(void);
void IPT_CloJoy(int closeall);
void IPT_CalJoyRumbleLow(void);
void IPT_CalJoyRumbleMedium(void);
void IPT_CalJoyRumbleHigh(void);

int JOY_IsKey(int button);
void JOY_Wait(int index, int button);

void GetJoyButtonMapping(void);
int JOY_IsScroll(int scrollflag);
int JOY_MapsInput(void);
int JOY_GetInput(void);
int JOY_GetConvertButton(int button, int convertbutton);
int JOY_GetButton(int button);
int JOY_GetAxis(int axis);