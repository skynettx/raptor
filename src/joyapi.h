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
	FIRE,
	CHWEAPON,
	MEGABOMB,
	MEGAFIRE
} joy_buttons_map;

extern int joy_ack;

void IPT_CalJoy(void);
void IPT_CloJoy(int closeall);
void IPT_CalJoyRumbleLow(void);
void IPT_CalJoyRumbleMedium(void);
void IPT_CalJoyRumbleHigh(void);

int JOY_IsKey(int button);
void JOY_Wait(int index, int button);

int JOY_MapsInput(void);
int JOY_GetInput(void);
int JOY_GetMappedButton(int button);
int JOY_GetButton(int button);
int JOY_GetAxis(int axis);