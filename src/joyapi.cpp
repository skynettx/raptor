#include "SDL.h"
#include "i_video.h"
#include "joyapi.h"

int joy_ack;

bool Up, Down, Left, Right;
bool Start, Back, LeftShoulder, RightShoulder;
bool AButton, BButton, XButton, YButton;

int16_t StickX, StickY, TriggerLeft, TriggerRight;

SDL_GameController* ControllerHandles[MAX_CONTROLLERS];
SDL_Haptic* RumbleHandles[MAX_CONTROLLERS] ;

int MaxJoysticks;
int ControllerIndex;
int JoystickIndex;

int AButtonconvert, BButtonconvert, XButtonconvert, YButtonconvert;

void IPT_CalJoy(void)
{
	SDL_Init(SDL_INIT_GAMECONTROLLER | SDL_INIT_HAPTIC);

	MaxJoysticks = SDL_NumJoysticks();
	ControllerIndex = 0;
	AButtonconvert = 0;
	BButtonconvert = 0;
	XButtonconvert = 0;
	YButtonconvert = 0;

	for (JoystickIndex = 0; JoystickIndex < MaxJoysticks; ++JoystickIndex)
	{
		if (!SDL_IsGameController(JoystickIndex))
		{
			continue;
		}
		if (ControllerIndex >= MAX_CONTROLLERS)
		{
			break;
		}
		ControllerHandles[ControllerIndex] = SDL_GameControllerOpen(JoystickIndex);
		RumbleHandles[ControllerIndex] = SDL_HapticOpen(JoystickIndex);
		if (SDL_HapticRumbleInit(RumbleHandles[ControllerIndex]) != 0)
		{
			SDL_HapticClose(RumbleHandles[ControllerIndex]);
			RumbleHandles[ControllerIndex] = 0;
		}
	    ControllerIndex++;
		GetJoyButtonMapping();
	}
}

void  IPT_CloJoy(void)
{
	for (ControllerIndex = 0; ControllerIndex < MAX_CONTROLLERS; ++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex])
		{
			if (RumbleHandles[ControllerIndex])
				SDL_HapticClose(RumbleHandles[ControllerIndex]);
			SDL_GameControllerClose(ControllerHandles[ControllerIndex]);
		}
	}
}

void I_HandleJoystickEvent(SDL_Event *sdlevent)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (ControllerHandles[ControllerIndex] != 0 && SDL_GameControllerGetAttached(ControllerHandles[ControllerIndex]))
		{
			Up = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_UP);
			Down = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_DOWN);
			Left = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_LEFT);
			Right = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
			Start = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_START);
			Back = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_BACK);
			LeftShoulder = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
			RightShoulder = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
			AButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_A);
			BButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_B);
			XButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_X);
			YButton = SDL_GameControllerGetButton(ControllerHandles[ControllerIndex], SDL_CONTROLLER_BUTTON_Y);

			StickX = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTX) / 8000;
			StickY = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_LEFTY) / 8000;
			TriggerLeft = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERLEFT) / 8000;
			TriggerRight = SDL_GameControllerGetAxis(ControllerHandles[ControllerIndex], SDL_CONTROLLER_AXIS_TRIGGERRIGHT) / 8000;
	    }
		if (sdlevent->type == SDL_CONTROLLERBUTTONUP) 
			joy_ack = 0;
		if (sdlevent->type == SDL_CONTROLLERBUTTONDOWN) 
			joy_ack = 1;
	}
}

void GetJoyButtonMapping(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		switch (SDL_GameControllerTypeForIndex(ControllerIndex))
		{
		case SDL_CONTROLLER_TYPE_PS3:
		case SDL_CONTROLLER_TYPE_PS4:
		case SDL_CONTROLLER_TYPE_PS5:
			AButtonconvert = 0;
			BButtonconvert = 1;
			XButtonconvert = 3;
			YButtonconvert = 2;
			break;
		case SDL_CONTROLLER_TYPE_NINTENDO_SWITCH_PRO:
		case SDL_CONTROLLER_TYPE_XBOX360:
		case SDL_CONTROLLER_TYPE_XBOXONE:
			AButtonconvert = 0;
			BButtonconvert = 1;
			XButtonconvert = 2;
			YButtonconvert = 3;
			break;
		default:
			if ((AButtonconvert == 0) && (BButtonconvert == 0) && (XButtonconvert == 0) && (YButtonconvert == 0))
			{
				AButtonconvert = 0;
				BButtonconvert = 1;
				XButtonconvert = 2;
				YButtonconvert = 3;
			}
		}
	}
}

void IPT_CalJoyRumbleLow(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
		if (RumbleHandles[ControllerIndex])
		{
			SDL_HapticRumblePlay(RumbleHandles[ControllerIndex], 0.4f, 1000);
		}
    }
}

void IPT_CalJoyRumbleMedium(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
        if (RumbleHandles[ControllerIndex])
		{
			SDL_HapticRumblePlay(RumbleHandles[ControllerIndex], 0.6f, 1000);
		}
    }
}

void IPT_CalJoyRumbleHigh(void)
{
	for (ControllerIndex = 0;
		ControllerIndex < MAX_CONTROLLERS;
		++ControllerIndex)
	{
        if (RumbleHandles[ControllerIndex])
		{
			SDL_HapticRumblePlay(RumbleHandles[ControllerIndex], 0.8f, 1000);
		}
    }
}

void JOY_Wait(int a1)
{
	while (StickX || StickY || Up || Down || Left || Right || Start || Back || LeftShoulder || RightShoulder || AButton || BButton || XButton || YButton)
	{
		I_GetEvent();
	}
}

int JOY_IsKey(int a1)
{
	if (StickX || StickY || Up || Down || Left || Right || Start || Back || LeftShoulder || RightShoulder || AButton || BButton || XButton || YButton)
	{
		JOY_Wait(a1);
		return 1;
	}
    return 0;
}

int JOY_IsKeyInGameStart(int a1)
{
	if (Start)
	{
		JOY_Wait(a1);
		return 1;
	}
	return 0;
}

int JOY_IsKeyInGameBack(int a1)
{

	if (Back)
	{
		JOY_Wait(a1);
		return 1;
	}
	return 0;
}

int JOY_IsKeyHelp(int a1)
{
	if (RightShoulder)
	{
		JOY_Wait(a1);
		return 1;
	}
	return 0;
}
