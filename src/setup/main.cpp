#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <io.h>
#endif // _WIN32
#ifdef __GNUC__
#include <unistd.h>
#define PATH_MAX        4096
#endif // __GNUC__

#include "SDL_filesystem.h"
#include "textscreen.h"
#include "prefapi.h"
#include "main.h"
#include "music.h"
#include "sound.h"
#include "input.h"

#ifdef _MSC_VER
#define PATH_MAX        4096
#define access _access
#endif // _MSC_VER

int setupflag, writesetupflag;
char* textbox_control = NULL;
char* textbox_musiccard = NULL;
char* textbox_soundfx = NULL;
int hasdatapath = 0;
char g_data_path[PATH_MAX];
char g_setup_path[PATH_MAX];
int controltype;
int musiccard;
int soundfxcard;
int fullscreen, aspect_ratio, txt_fullscreen, haptic, joy_ipt_MenuNew, sys_midi, winmm_mpu_device, core_dls_synth, core_midi_port, alsaclient, alsaport;
int keymoveup, keymovedown, keymoveleft, keymoveright, keyfire, keyspecial, keymega;
static char soundfont[128];
static char* sf;

txt_window_t* mainwindow;
txt_window_t* infowindow;
txt_table_t* infotable;

char* stringduplicate(const char* source)
{
	char* destination = (char*)malloc(strlen(source) + 1);
	if (destination == NULL)
		return NULL;
	strcpy(destination, source);
	return destination;
}

void WindowSelect(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(window))
{
	TXT_CAST_ARG(txt_window_t, window);

	TXT_WidgetKeyPress(window, KEY_ENTER);
}

void ClosePwnBox(TXT_UNCAST_ARG(widget), TXT_UNCAST_ARG(window))
{
	TXT_CAST_ARG(txt_window_t, window);

	TXT_CloseWindow(window);
}
///////////////////////////////////////////////////Check Settings from Setup.ini///////////////////////////////////////////////////////////////
void CheckSetupSettings(void)
{
	switch (controltype)
	{
	case 0:
		textbox_control =(char*) "Keyboard                    ";
		break;
	case 1:
		textbox_control =(char*) "Mouse                       ";
		break;
	case 2:
		textbox_control =(char*) "Joystick                    ";
		break;
	default:
		textbox_control =(char*) "None                        ";
		break;
	}
	////////////////////////////////////////////////////////////////////////////////////////////////////////////
	switch (musiccard)
	{
	case 0:
		textbox_musiccard = (char*) "None                        ";
		break;
	case 2:
		textbox_musiccard = (char*) "Adlib                       ";
		break;
	case 3:
		textbox_musiccard = (char*) "Gravis Ultra Sound          ";
		break;
	case 4:
		textbox_musiccard = (char*) "Pro Audio Spectrum          ";
		break;
	case 5:
		textbox_musiccard = (char*) "Sound Blaster               ";
		break;
	case 6:
		textbox_musiccard = (char*) "WaveBlaster                 ";
		break;
	case 7:
		textbox_musiccard = (char*)"Roland Sound Canvas         ";
		break;
	case 8:
		textbox_musiccard = (char*)"General Midi                ";
		break;
	case 9:
		textbox_musiccard = (char*)"Sound Blaster AWE 32        ";
		break;
	default:
		textbox_musiccard = (char*)"None                        ";
		break;
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	switch (soundfxcard)
	{
	case 0:
		textbox_soundfx = (char*)"None                        ";
		break;
	case 1:
		textbox_soundfx = (char*)"PC Speaker                  ";
		break;
	case 2:
		textbox_soundfx = (char*)"Adlib                       ";
		break;
	case 3:
		textbox_soundfx = (char*)"Gravis Ultra Sound          ";
		break;
	case 4:
		textbox_soundfx = (char*)"Pro Audio Spectrum          ";
		break;
	case 5:
		textbox_soundfx = (char*)"Sound Blaster               ";
		break;
	case 7:
		textbox_soundfx = (char*)"Roland Sound Canvas         ";
		break;
	case 8:
		textbox_soundfx = (char*)"General Midi                ";
		break;
	default:
		textbox_soundfx = (char*)"None                        ";
		break;
	}
}
/////////////////////////////////////////////Get Settings from Setup.ini//////////////////////////////////////////////////////
void GetSetupSettings(void)
{
	controltype = INI_GetPreferenceLong("Setup", "Control", 3);
	musiccard = INI_GetPreferenceLong("Music", "CardType", 0);
	soundfxcard = INI_GetPreferenceLong("SoundFX", "CardType", 0);
	keymoveup = INI_GetPreferenceLong("Keyboard", "MoveUp", 72);
	keymovedown = INI_GetPreferenceLong("Keyboard", "MoveDn", 80);
	keymoveleft = INI_GetPreferenceLong("Keyboard", "MoveLeft", 75);
	keymoveright = INI_GetPreferenceLong("Keyboard", "MoveRight", 77);
	keyfire = INI_GetPreferenceLong("Keyboard", "Fire", 29);
	keyspecial = INI_GetPreferenceLong("Keyboard", "FireSp", 56);
	keymega = INI_GetPreferenceLong("Keyboard", "ChangeSp", 57);
	mousebfire = INI_GetPreferenceLong("Mouse", "Fire", 0);
	mousebchweapon = INI_GetPreferenceLong("Mouse", "FireSp", 1);
	mousebmega = INI_GetPreferenceLong("Mouse", "ChangeSp", 2);
	joybfire = INI_GetPreferenceLong("JoyStick", "Fire", 0);
	joybchweapon = INI_GetPreferenceLong("JoyStick", "FireSp", 1);
	joybmega = INI_GetPreferenceLong("JoyStick", "ChangeSp", 2);
	fullscreen = INI_GetPreferenceLong("Video", "fullscreen", 0);
	aspect_ratio = INI_GetPreferenceLong("Video", "aspect_ratio_correct", 1);
	txt_fullscreen = INI_GetPreferenceLong("Video", "txt_fullscreen", 0);
	haptic = INI_GetPreferenceLong("Setup", "Haptic", 1);
	joy_ipt_MenuNew = INI_GetPreferenceLong("Setup", "joy_ipt_MenuNew", 0);
	sys_midi = INI_GetPreferenceLong("Setup", "sys_midi", 0);
	winmm_mpu_device = INI_GetPreferenceLong("Setup", "winmm_mpu_device", 0);
	core_dls_synth = INI_GetPreferenceLong("Setup", "core_dls_synth", 1);
	core_midi_port = INI_GetPreferenceLong("Setup", "core_midi_port", 0);
	alsaclient = INI_GetPreferenceLong("Setup", "alsa_output_client", 128);
	alsaport = INI_GetPreferenceLong("Setup", "alsa_output_port", 0);
	sf = (char*)INI_GetPreference("Setup", "SoundFont", soundfont, 127, "SoundFont.sf2");
}
/////////////////////////////////////////////Get Setup.ini/////////////////////////////////////////////////////////////////////
const char* RAP_DataPath(void)
{
#if _WIN32 || __linux__ || __APPLE__
	char* gethome;

	gethome = SDL_GetPrefPath("", "Raptor");

	if (gethome != NULL)
	{
		strcpy(g_data_path, gethome);
		strcpy(g_setup_path, gethome);
		sprintf(g_setup_path, "%s%s", g_setup_path, "SETUP.INI");
		hasdatapath = 1;
		SDL_free(gethome);
	}
	else
	{
		exit(0);
	}

	return g_data_path;
#else
	memset(g_data_path, 0, sizeof(g_data_path));

	hasdatapath = 0;

	strcpy(g_setup_path, "SETUP.INI");

	return g_data_path;
#endif // _WIN32 || __linux__ || __APPLE__
}

const char* RAP_GetSetupPath(void)
{
	return g_setup_path;
}
////////////////////////////////////////////////Save Settings to SETUP.INI/////////////////////////////////////////////////////
void SaveSettings(TXT_UNCAST_ARG(widget), void* user_data)
{
	INI_PutPreferenceLong("Setup", "Detail", 1);

	if (ControllerType)                                                         //Save Controller Type to SETUP.INI
	{
		INI_PutPreferenceLong("Setup", "Control", ControllerType);
	}
	if (ControllerType == 1000)
	{
		INI_PutPreferenceLong("Setup", "Control", 0);
	}

	INI_PutPreferenceLong("Setup", "Haptic", haptic);                           //Save Additional Feature Haptic to SETUP.INI
	INI_PutPreferenceLong("Setup", "joy_ipt_MenuNew", joy_ipt_MenuNew);         //Save Additional Feature joy_ipt_MenuNew to SETUP.INI
	INI_PutPreferenceLong("Setup", "sys_midi", sys_midi);                       //Save Additional Feature sys_midi to SETUP.INI
	INI_PutPreferenceLong("Setup", "winmm_mpu_device", winmm_mpu_device);       //Save Additional Feature winmm_mpu_device to SETUP.INI
	INI_PutPreferenceLong("Setup", "core_dls_synth", core_dls_synth);           //Save Additional Feature core_dls_synth to SETUP.INI
	INI_PutPreferenceLong("Setup", "core_midi_port", core_midi_port);           //Save Additional Feature core_dls_synth to SETUP.INI
	INI_PutPreferenceLong("Setup", "alsa_output_client", alsaclient);           //Save Additional Feature alsa_output_client to SETUP.INI
	INI_PutPreferenceLong("Setup", "alsa_output_port", alsaport);               //Save Additional Feature alsa_output_port to SETUP.INI
	INI_PutPreference("Setup", "SoundFont", sf);                                //Save Additional Feature soundfont to SETUP.INI

	if (CardType)                                                               //Save Music Card to SETUP.INI
	{
		INI_PutPreferenceLong("Music", "Volume", 85);
		INI_PutPreferenceLong("Music", "CardType", CardType);
	}
	if (CardType == 4 || CardType == 3)
	{
		INI_DeletePreference("Music", "MidiPort");
		INI_PutPreferenceLong("Music", "BasePort", 220);
		INI_PutPreferenceLong("Music", "Irq", 7);
		INI_PutPreferenceLong("Music", "Dma", 1);
	}
	if (CardType == 2)
	{
		INI_DeletePreference("Music", "MidiPort");
		INI_DeletePreference("Music", "BasePort");
		INI_DeletePreference("Music", "Irq");
		INI_DeletePreference("Music", "Dma");
	}
	if (CardType == 1000)
	{
		INI_DeletePreference("Music", "MidiPort");
		INI_DeletePreference("Music", "BasePort");
		INI_DeletePreference("Music", "Irq");
		INI_DeletePreference("Music", "Dma");
		INI_PutPreferenceLong("Music", "Volume", 85);
		INI_PutPreferenceLong("Music", "CardType", 0);
	}
	if (BasePort)
	{
		INI_DeletePreference("Music", "MidiPort");
		INI_PutPreferenceLong("Music", "BasePort", BasePort);
		INI_PutPreferenceLong("Music", "Irq", 7);
		INI_PutPreferenceLong("Music", "Dma", 1);
	}
	if (MidiPort)
	{
		INI_DeletePreference("Music", "BasePort");
		INI_DeletePreference("Music", "Irq");
		INI_DeletePreference("Music", "Dma");
		INI_PutPreferenceLong("Music", "MidiPort", MidiPort);
	}

	if (SoundCardType)                                                              //Save Sound Card to SETUP.INI
	{
		INI_PutPreferenceLong("SoundFX", "Volume", 85);
		INI_PutPreferenceLong("SoundFX", "CardType", SoundCardType);
	}
	if (SoundCardType == 8 || SoundCardType == 7)
	{
		INI_DeletePreference("SoundFX", "BasePort");
		INI_DeletePreference("SoundFX", "Irq");
		INI_DeletePreference("SoundFX", "Dma");
		INI_PutPreferenceLong("SoundFX", "Channels", 6);
		INI_PutPreferenceLong("SoundFX", "MidiPort", SoundMidiPort);
	}
	if (SoundCardType == 5)
	{
		INI_DeletePreference("SoundFX", "MidiPort");
		INI_PutPreferenceLong("SoundFX", "Channels", SoundChannels);
		INI_PutPreferenceLong("SoundFX", "BasePort", SoundBasePort);
		INI_PutPreferenceLong("SoundFX", "Irq", SoundIrq);
		if (SoundDma == 1000)
			SoundDma = 0;
		INI_PutPreferenceLong("SoundFX", "Dma", SoundDma);
	}
	if (SoundCardType == 4)
	{
		INI_DeletePreference("SoundFX", "BasePort");
		INI_DeletePreference("SoundFX", "MidiPort");
		INI_DeletePreference("SoundFX", "Irq");
		INI_DeletePreference("SoundFX", "Dma");
		INI_PutPreferenceLong("SoundFX", "Channels", SoundChannels);
	}
	if (SoundCardType == 3)
	{
		INI_DeletePreference("SoundFX", "MidiPort");
		INI_PutPreferenceLong("SoundFX", "Channels", SoundChannels);
		INI_PutPreferenceLong("SoundFX", "BasePort", 220);
		INI_PutPreferenceLong("SoundFX", "Irq", 7);
		INI_PutPreferenceLong("SoundFX", "Dma", 1);
	}
	if (SoundCardType == 2)
	{
		INI_DeletePreference("SoundFX", "BasePort");
		INI_DeletePreference("SoundFX", "MidiPort");
		INI_DeletePreference("SoundFX", "Irq");
		INI_DeletePreference("SoundFX", "Dma");
		INI_PutPreferenceLong("SoundFX", "Channels", 3);
	}
	if (SoundCardType == 1)
	{
		INI_DeletePreference("SoundFX", "BasePort");
		INI_DeletePreference("SoundFX", "MidiPort");
		INI_DeletePreference("SoundFX", "Irq");
		INI_DeletePreference("SoundFX", "Dma");
		INI_PutPreferenceLong("SoundFX", "Channels", 4);
	}
	if (SoundCardType == 1000)
	{
		INI_DeletePreference("SoundFX", "BasePort");
		INI_DeletePreference("SoundFX", "MidiPort");
		INI_DeletePreference("SoundFX", "Irq");
		INI_DeletePreference("SoundFX", "Dma");
		INI_PutPreferenceLong("SoundFX", "Volume", 85);
		INI_PutPreferenceLong("SoundFX", "CardType", 0);
		INI_PutPreferenceLong("SoundFX", "Channels", 4);
	}
	if (writesetupflag && !writeflagkey)                                     //Save Defaultkeyboardlayout when SETUP.INI not in folder and layout not set
	{
		INI_PutPreferenceLong("Keyboard", "MoveUp", 72);
		INI_PutPreferenceLong("Keyboard", "MoveDn", 80);
		INI_PutPreferenceLong("Keyboard", "MoveLeft", 75);
		INI_PutPreferenceLong("Keyboard", "MoveRight", 77);
		INI_PutPreferenceLong("Keyboard", "Fire", 29);
		INI_PutPreferenceLong("Keyboard", "FireSp", 56);
		INI_PutPreferenceLong("Keyboard", "ChangeSp", 57);
		INI_PutPreferenceLong("Keyboard", "MegaFire", 54);
	}
	if (writeflagkey)                                                        //Save Keyboardlayout to SETUP.INI
	{
		INI_PutPreferenceLong("Keyboard", "MoveUp", key_up_convert);
		INI_PutPreferenceLong("Keyboard", "MoveDn", key_down_convert);
		INI_PutPreferenceLong("Keyboard", "MoveLeft", key_left_convert);
		INI_PutPreferenceLong("Keyboard", "MoveRight", key_right_convert);
		INI_PutPreferenceLong("Keyboard", "Fire", key_fire_convert);
		INI_PutPreferenceLong("Keyboard", "FireSp", key_special_convert);
		INI_PutPreferenceLong("Keyboard", "ChangeSp", key_mega_convert);
		INI_PutPreferenceLong("Keyboard", "MegaFire", 54);
	}
	if (writesetupflag && !writeflagmouse)                                  //Save Defaultmouselayout when SETUP.INI not in folder and layout not set
	{
		INI_PutPreferenceLong("Mouse", "Fire", 0);
		INI_PutPreferenceLong("Mouse", "FireSp", 1);
		INI_PutPreferenceLong("Mouse", "ChangeSp", 2);
	}
	if (writeflagmouse)                                                    //Save Mouselayout to SETUP.INI
	{
		INI_PutPreferenceLong("Mouse", "Fire", mousebfire);
		INI_PutPreferenceLong("Mouse", "FireSp", mousebchweapon);
		INI_PutPreferenceLong("Mouse", "ChangeSp", mousebmega);
	}
	if (writesetupflag && !writeflagjoy)                                  //Save Defaultjoysticklayout when SETUP.INI not in folder and layout not set
	{
		INI_PutPreferenceLong("JoyStick", "Fire", 0);
		INI_PutPreferenceLong("JoyStick", "FireSp", 1);
		INI_PutPreferenceLong("JoyStick", "ChangeSp", 2);
		INI_PutPreferenceLong("JoyStick", "MegaFire", 3);
	}
	if (writeflagjoy)                                                     //Save joysticklayout to SETUP.INI
	{
		if (writeflagjoybfire || writesetupflag)
			INI_PutPreferenceLong("JoyStick", "Fire", joybfireout);
		if (writeflagjoybchweapon || writesetupflag)
			INI_PutPreferenceLong("JoyStick", "FireSp", joybchweaponout);
		if (writeflagjoybmega || writesetupflag)
			INI_PutPreferenceLong("JoyStick", "ChangeSp", joybmegaout);

		INI_PutPreferenceLong("JoyStick", "MegaFire", 3);
	}

	INI_PutPreferenceLong("Video", "fullscreen", fullscreen);                          //Save Additional Feature fullscreen to SETUP.INI
	INI_PutPreferenceLong("Video", "aspect_ratio_correct", aspect_ratio);              //Save Additional Feature aspect_ratio_correct to SETUP.INI
	INI_PutPreferenceLong("Video", "txt_fullscreen", txt_fullscreen);                  //Save Additional Feature txt_fullscreen to SETUP.INI
}
/////////////////////////////////////////////////////////Info Window/////////////////////////////////////////////////////////
void InfoWindow(TXT_UNCAST_ARG(widget), void* user_data)
{
	CheckSetupSettings();

	infowindow = TXT_NewCustomWindow("Current Configuration:                      ", TXT_COLOR_DARK_GREY, TXT_COLOR_GREY, TXT_COLOR_GREY, TXT_COLOR_DARK_GREY, TXT_COLOR_DARK_GREY, TXT_COLOR_DARK_GREY, TXT_COLOR_GREY);
	TXT_SetWindowAction(infowindow, TXT_HORIZ_LEFT, NULL);
	TXT_SetWindowAction(infowindow, TXT_HORIZ_RIGHT, NULL);
	TXT_SetWindowPosition(infowindow, TXT_HORIZ_CENTER, TXT_VERT_TOP, TXT_SCREEN_W / 2, 2);

	infotable = TXT_NewTable(2);
	TXT_AddWidgets(infotable,
		TXT_NewLabel("Control Type  : "),
		TXT_NewLabel(textbox_control),
		TXT_NewLabel("Music Card    : "),
		TXT_NewLabel(textbox_musiccard),
		TXT_NewLabel("Sound FX Card : "),
		TXT_NewLabel(textbox_soundfx),
		NULL);

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(infowindow, infotable);
}
////////////////////////////////////////////////Select Additional Features////////////////////////////////////////////////////////
void AdditionalFeatures(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;

	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;

	txt_checkbox_t* fullscreenbox;
	txt_checkbox_t* aspectratiobox;
	txt_checkbox_t* textmodefullbox;

	txt_checkbox_t* systemmidibox;
	txt_inputbox_t* winmmmpudevicebox;
	txt_checkbox_t* coredlssynthbox;
	txt_inputbox_t* coremidiportbox;
	txt_inputbox_t* alsaclientbox;
	txt_inputbox_t* alsaportbox;
	txt_inputbox_t* sfbox;

	txt_checkbox_t* hapticbox;
	txt_checkbox_t* menunewbox;

	fullscreenbox = TXT_NewCheckBox("Fullscreen", &fullscreen);
	aspectratiobox = TXT_NewCheckBox("Aspect Ratio", &aspect_ratio);
	textmodefullbox = TXT_NewCheckBox("Text Mode Fullscreen", &txt_fullscreen);

	systemmidibox = TXT_NewCheckBox("System MIDI", &sys_midi);
	winmmmpudevicebox = TXT_NewIntInputBox(&winmm_mpu_device, 3);
	coredlssynthbox = TXT_NewCheckBox("Core Audio DLS Synthesizer", &core_dls_synth);
	coremidiportbox = TXT_NewIntInputBox(&core_midi_port, 3);
	alsaclientbox = TXT_NewIntInputBox(&alsaclient, 4);
	alsaportbox = TXT_NewIntInputBox(&alsaport, 1);
	sfbox = TXT_NewInputBoxCustomSize(&sf, 115, 35);
	
	hapticbox = TXT_NewCheckBox("Haptic (Game Controller Rumble Support)", &haptic);
	menunewbox = TXT_NewCheckBox("New Joystick Menu Input", &joy_ipt_MenuNew);

	sf = stringduplicate(sf);

	window = TXT_NewWindow("Additional Features                                        ");

	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");

	TXT_SetWidgetFocus(mainwindow, 1);

	TXT_AddWidgets(window, TXT_NewSeparator("Video"),
		fullscreenbox,
		aspectratiobox,
		textmodefullbox, NULL);

	TXT_SetHelpLabel(fullscreenbox, "Select Fullscreen Mode On / Off");
	TXT_SetHelpLabel(aspectratiobox, "Select Aspect Ratio Correction On / Off");
	TXT_SetHelpLabel(textmodefullbox, "Select Text Mode Fullscreen On / Off");

	TXT_AddWidgets(window, TXT_NewSeparator("Audio"),
		systemmidibox, TXT_NewConditional(&sys_midi, 1,
			TXT_NewHorizBox(TXT_NewStrut(4, 0), TXT_NewLabel("Windows Multimedia MIDI Device: "), winmmmpudevicebox,
				TXT_NewLabel(" (Default = 0)"), NULL)),
		TXT_NewConditional(&sys_midi, 1,
			TXT_NewHorizBox(TXT_NewStrut(4, 0), coredlssynthbox, NULL)),
		TXT_NewConditional(&core_dls_synth, 0,
			TXT_NewConditional(&sys_midi, 1,
				TXT_NewHorizBox(TXT_NewStrut(4, 0), TXT_NewLabel("Core MIDI Port: "), coremidiportbox,
					TXT_NewLabel(" (Default = 0)"), NULL))),
		TXT_NewConditional(&sys_midi, 1,
			TXT_NewHorizBox(TXT_NewStrut(4, 0), TXT_NewLabel("ALSA Output Port: "), alsaclientbox,
				TXT_NewLabel(":"), alsaportbox, TXT_NewLabel(" (Default = 128:0)"), NULL)),
		TXT_NewHorizBox(TXT_NewLabel("TSF SoundFont Filename: "), sfbox, NULL), NULL);

	TXT_SetHelpLabel(systemmidibox, "Select System MIDI On / Off");
	TXT_SetHelpLabel(winmmmpudevicebox, "Enter the Windows Multimedia MIDI device");
	TXT_SetHelpLabel(coredlssynthbox, "Select macOS Core Audio DLS Synthesizer On / Off");
	TXT_SetHelpLabel(coremidiportbox, "Enter the Port Number for Core MIDI");
	TXT_SetHelpLabel(alsaclientbox, "Enter the Client Number for ALSA Client");
	TXT_SetHelpLabel(alsaportbox, "Enter the Port Number for ALSA Port");
	TXT_SetHelpLabel(sfbox, "Enter the file name for the GM compatible soundfont in SF2 format");

	TXT_AddWidgets(window, TXT_NewSeparator("Controller"),
		hapticbox,
		menunewbox, NULL);

	TXT_SetHelpLabel(hapticbox, "Select Haptic for Controller On / Off");
	TXT_SetHelpLabel(menunewbox, "Select New Joystick Menu Input On / Off");

	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);

	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}
////////////////////////////////////////////////Select ControlButtonConfigMain////////////////////////////////////////////////////
void ControlButtonConfig(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;

	window = TXT_NewWindow("Controller Configuration   ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 8);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("Keyboard                   "),
		button2 = TXT_NewButton("Mouse                      "),
		button3 = TXT_NewButton("Joystick                   "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(button1, "pressed", GetControlKeyboard, (void*)"Keyboard");
	TXT_SignalConnect(button2, "pressed", GetControlMouse, (void*)"Mouse");
	TXT_SignalConnect(button3, "pressed", GetControlJoystick, (void*)"Joystick");

	TXT_SetHelpLabel(button1, " Setup New Keyboard Layout");
	TXT_SetHelpLabel(button2, "Configure Mouse Button Layout");
	TXT_SetHelpLabel(button3, "Configure Joystick Button Layout");

	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}
////////////////////////////////////////////////Select ControlType/////////////////////////////////////////////////////////////
void Control(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;

	window = TXT_NewWindow("Select Controller Type      ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 40, 8);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("Keyboard                    "),
		button2 = TXT_NewButton("Mouse                       "),
		button3 = TXT_NewButton("Joystick                    "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(button1, "pressed", GetControl, (void*)"Keyboard");
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetControl, (void*)"Mouse");
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetControl, (void*)"Joystick");
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);

	TXT_DrawHelpLabel(" Use Keyboard for All Controls");

	TXT_SetHelpLabel(button1, " Use Keyboard for All Controls");
	TXT_SetHelpLabel(button2, " Use Mouse for Main control");
	TXT_SetHelpLabel(button3, " Use Joystick for Main control");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	if (setupflag)
	{
		TXT_SignalConnect(button1, "pressed", MusicCard, NULL);
		TXT_SignalConnect(button2, "pressed", MusicCard, NULL);
		TXT_SignalConnect(button3, "pressed", MusicCard, NULL);
		TXT_SignalConnect(close_button, "pressed", MusicCard, NULL);
		TXT_SignalConnect(close_button, "pressed", GetControl, (void*)"Keyboard");
	}

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}
////////////////////////////////////////////////Select Sound FX Card//////////////////////////////////////////////////////////
void SoundCardChannels(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_button_t* button7;
	txt_button_t* button8;

	window = TXT_NewWindow("Number of Digital Channels ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 7);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("One                        "),
		button2 = TXT_NewButton("Two                        "),
		button3 = TXT_NewButton("Three                      "),
		button4 = TXT_NewButton("Four                       "),
		button5 = TXT_NewButton("Five                       "),
		button6 = TXT_NewButton("Six                        "),
		button7 = TXT_NewButton("Seven                      "),
		button8 = TXT_NewButton("Eight                      "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", GetSoundChannels, (void*)"One");
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetSoundChannels, (void*)"Two");
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetSoundChannels, (void*)"Three");
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", GetSoundChannels, (void*)"Four");
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", GetSoundChannels, (void*)"Five");
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", GetSoundChannels, (void*)"Six");
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button7, "pressed", GetSoundChannels, (void*)"Seven");
	TXT_SignalConnect(button7, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button8, "pressed", GetSoundChannels, (void*)"Eight");
	TXT_SignalConnect(button8, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Will play one Digital sound effect at a time");
	TXT_SetHelpLabel(button2, " Will play two Digital sound effects at a time");
	TXT_SetHelpLabel(button3, " Will play three Digital sound effects at a time");
	TXT_SetHelpLabel(button4, " Will play four Digital sound effects at a time");
	TXT_SetHelpLabel(button5, " Will play five Digital sound effects at a time");
	TXT_SetHelpLabel(button6, " Will play six Digital sound effects at a time");
	TXT_SetHelpLabel(button7, " Will play seven Digital sound effects at a time");
	TXT_SetHelpLabel(button8, " Will play eight Digital sound effects at a time");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (SoundChannels)
	{
	case 1:
		TXT_SelectWidget(table, button1);
		break;
	case 2:
		TXT_SelectWidget(table, button2);
		break;
	case 3:
		TXT_SelectWidget(table, button3);
		break;
	case 4:
		TXT_SelectWidget(table, button4);
		break;
	case 5:
		TXT_SelectWidget(table, button5);
		break;
	case 6:
		TXT_SelectWidget(table, button6);
		break;
	case 7:
		TXT_SelectWidget(table, button7);
		break;
	case 8:
		TXT_SelectWidget(table, button8);
		break;
	default:
		TXT_SelectWidget(table, button4);
		break;
	}

	if (setupflag)
	{
		TXT_SignalConnect(button1, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button2, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button3, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button4, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button5, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button6, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button7, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button8, "pressed", MainMenu, NULL);
		TXT_SignalConnect(close_button, "pressed", MainMenu, NULL);
	}

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}

void SoundCardDma(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;

	window = TXT_NewWindow("Available DMA Channels     ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 6);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("0                          "),
		button2 = TXT_NewButton("1                          "),
		button3 = TXT_NewButton("3                          "),
		button4 = TXT_NewButton("5                          "),
		button5 = TXT_NewButton("6                          "),
		button6 = TXT_NewButton("7                          "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", GetSoundDma, (void*)"0");
	TXT_SignalConnect(button1, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetSoundDma, (void*)"1");
	TXT_SignalConnect(button2, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetSoundDma, (void*)"3");
	TXT_SignalConnect(button3, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", GetSoundDma, (void*)"5");
	TXT_SignalConnect(button4, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", GetSoundDma, (void*)"6");
	TXT_SignalConnect(button5, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", GetSoundDma, (void*)"7");
	TXT_SignalConnect(button6, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Sets DMA to 0");
	TXT_SetHelpLabel(button2, " Sets DMA to 1");
	TXT_SetHelpLabel(button3, " Sets DMA to 3");
	TXT_SetHelpLabel(button4, " Sets DMA to 5");
	TXT_SetHelpLabel(button5, " Sets DMA to 6");
	TXT_SetHelpLabel(button6, " Sets DMA to 7");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (SoundDma)
	{
	case 1000:
		TXT_SelectWidget(table, button1);
		break;
	case 1:
		TXT_SelectWidget(table, button2);
		break;
	case 3:
		TXT_SelectWidget(table, button3);
		break;
	case 5:
		TXT_SelectWidget(table, button4);
		break;
	case 6:
		TXT_SelectWidget(table, button5);
		break;
	case 7:
		TXT_SelectWidget(table, button6);
		break;
	default:
		TXT_SelectWidget(table, button2);
		break;
	}

	if (setupflag)
		TXT_SignalConnect(close_button, "pressed", MainMenu, NULL);

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}

void SoundCardIrq(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;

	window = TXT_NewWindow("Available IRQs             ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 8);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("2                          "),
		button2 = TXT_NewButton("5                          "),
		button3 = TXT_NewButton("7                          "),
		button4 = TXT_NewButton("10                         "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", GetSoundIrq, (void*)"2");
	TXT_SignalConnect(button1, "pressed", SoundCardDma, NULL);
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetSoundIrq, (void*)"5");
	TXT_SignalConnect(button2, "pressed", SoundCardDma, NULL);
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetSoundIrq, (void*)"7");
	TXT_SignalConnect(button3, "pressed", SoundCardDma, NULL);
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", GetSoundIrq, (void*)"10");
	TXT_SignalConnect(button4, "pressed", SoundCardDma, NULL);
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Sets IRQ to 2");
	TXT_SetHelpLabel(button2, " Sets IRQ to 5");
	TXT_SetHelpLabel(button3, " Sets IRQ to 7");
	TXT_SetHelpLabel(button4, " Sets IRQ to 10");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (SoundIrq)
	{
	case 2:
		TXT_SelectWidget(table, button1);
		break;
	case 5:
		TXT_SelectWidget(table, button2);
		break;
	case 7:
		TXT_SelectWidget(table, button3);
		break;
	case 10:
		TXT_SelectWidget(table, button4);
		break;
	default:
		TXT_SelectWidget(table, button3);
		break;
	}

	if (setupflag)
		TXT_SignalConnect(close_button, "pressed", MainMenu, NULL);

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}

void SoundCardSB(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_button_t* button7;

	GetSoundcard(0, user_data);

	window = TXT_NewWindow("Available PORTs            ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 6);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("210                        "),
		button2 = TXT_NewButton("220                        "),
		button3 = TXT_NewButton("230                        "),
		button4 = TXT_NewButton("240                        "),
		button5 = TXT_NewButton("250                        "),
		button6 = TXT_NewButton("260                        "),
		button7 = TXT_NewButton("280                        "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", GetSoundBasePort, (void*)"210");
	TXT_SignalConnect(button1, "pressed", SoundCardIrq, NULL);
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetSoundBasePort, (void*)"220");
	TXT_SignalConnect(button2, "pressed", SoundCardIrq, NULL);
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetSoundBasePort, (void*)"230");
	TXT_SignalConnect(button3, "pressed", SoundCardIrq, NULL);
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", GetSoundBasePort, (void*)"240");
	TXT_SignalConnect(button4, "pressed", SoundCardIrq, NULL);
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", GetSoundBasePort, (void*)"250");
	TXT_SignalConnect(button5, "pressed", SoundCardIrq, NULL);
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", GetSoundBasePort, (void*)"260");
	TXT_SignalConnect(button6, "pressed", SoundCardIrq, NULL);
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button7, "pressed", GetSoundBasePort, (void*)"280");
	TXT_SignalConnect(button7, "pressed", SoundCardIrq, NULL);
	TXT_SignalConnect(button7, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Sets PORT address to 210");
	TXT_SetHelpLabel(button2, " Sets PORT address to 220");
	TXT_SetHelpLabel(button3, " Sets PORT address to 230");
	TXT_SetHelpLabel(button4, " Sets PORT address to 240");
	TXT_SetHelpLabel(button5, " Sets PORT address to 250");
	TXT_SetHelpLabel(button6, " Sets PORT address to 260");
	TXT_SetHelpLabel(button7, " Sets PORT address to 280");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (SoundBasePort)
	{
	case 210:
		TXT_SelectWidget(table, button1);
		break;
	case 220:
		TXT_SelectWidget(table, button2);
		break;
	case 230:
		TXT_SelectWidget(table, button3);
		break;
	case 240:
		TXT_SelectWidget(table, button4);
		break;
	case 250:
		TXT_SelectWidget(table, button5);
		break;
	case 260:
		TXT_SelectWidget(table, button6);
		break;
	case 280:
		TXT_SelectWidget(table, button7);
		break;
	default:
		TXT_SelectWidget(table, button2);
		break;
	}

	if (setupflag)
		TXT_SignalConnect(close_button, "pressed", MainMenu, NULL);

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}

void SoundCardGM(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_button_t* button7;
	txt_button_t* button8;
	txt_button_t* button9;
	txt_button_t* button10;
	txt_button_t* button11;
	txt_button_t* button12;

	GetSoundcard(0, user_data);

	window = TXT_NewWindow("Available MIDI Ports       ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 3);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("220                        "),
		button2 = TXT_NewButton("230                        "),
		button3 = TXT_NewButton("240                        "),
		button4 = TXT_NewButton("250                        "),
		button5 = TXT_NewButton("300                        "),
		button6 = TXT_NewButton("320                        "),
		button7 = TXT_NewButton("330                        "),
		button8 = TXT_NewButton("332                        "),
		button9 = TXT_NewButton("334                        "),
		button10 = TXT_NewButton("336                        "),
		button11 = TXT_NewButton("340                        "),
		button12 = TXT_NewButton("360                        "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", GetSoundGMPort, (void*)"220");
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetSoundGMPort, (void*)"230");
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetSoundGMPort, (void*)"240");
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", GetSoundGMPort, (void*)"250");
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", GetSoundGMPort, (void*)"300");
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", GetSoundGMPort, (void*)"320");
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button7, "pressed", GetSoundGMPort, (void*)"330");
	TXT_SignalConnect(button7, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button8, "pressed", GetSoundGMPort, (void*)"332");
	TXT_SignalConnect(button8, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button9, "pressed", GetSoundGMPort, (void*)"334");
	TXT_SignalConnect(button9, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button10, "pressed", GetSoundGMPort, (void*)"336");
	TXT_SignalConnect(button10, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button11, "pressed", GetSoundGMPort, (void*)"340");
	TXT_SignalConnect(button11, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button12, "pressed", GetSoundGMPort, (void*)"360");
	TXT_SignalConnect(button12, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Sets Midi port to 220");
	TXT_SetHelpLabel(button2, " Sets Midi port to 230");
	TXT_SetHelpLabel(button3, " Sets Midi port to 240");
	TXT_SetHelpLabel(button4, " Sets Midi port to 250");
	TXT_SetHelpLabel(button5, " Sets Midi port to 300");
	TXT_SetHelpLabel(button6, " Sets Midi port to 320");
	TXT_SetHelpLabel(button7, " Sets Midi port to 330");
	TXT_SetHelpLabel(button8, " Sets Midi port to 332");
	TXT_SetHelpLabel(button9, " Sets Midi port to 334");
	TXT_SetHelpLabel(button10, " Sets Midi port to 336");
	TXT_SetHelpLabel(button11, " Sets Midi port to 340");
	TXT_SetHelpLabel(button12, " Sets Midi port to 360");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (SoundMidiPort)
	{
	case 220:
		TXT_SelectWidget(table, button1);
		break;
	case 230:
		TXT_SelectWidget(table, button2);
		break;
	case 240:
		TXT_SelectWidget(table, button3);
		break;
	case 250:
		TXT_SelectWidget(table, button4);
		break;
	case 300:
		TXT_SelectWidget(table, button5);
		break;
	case 320:
		TXT_SelectWidget(table, button6);
		break;
	case 330:
		TXT_SelectWidget(table, button7);
		break;
	case 332:
		TXT_SelectWidget(table, button8);
		break;
	case 334:
		TXT_SelectWidget(table, button9);
		break;
	case 336:
		TXT_SelectWidget(table, button10);
		break;
	case 340:
		TXT_SelectWidget(table, button11);
		break;
	case 360:
		TXT_SelectWidget(table, button12);
		break;
	default:
		TXT_SelectWidget(table, button7);
		break;
	}

	if (setupflag)
	{
		TXT_SignalConnect(button1, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button2, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button3, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button4, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button5, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button6, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button7, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button8, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button9, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button10, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button11, "pressed", MainMenu, NULL);
		TXT_SignalConnect(button12, "pressed", MainMenu, NULL);
		TXT_SignalConnect(close_button, "pressed", MainMenu, NULL);
	}

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}

void SoundCardProUl(TXT_UNCAST_ARG(widget), void* user_data)
{
	GetSoundcard(0, user_data);
}

void SoundCardAdPCNo(TXT_UNCAST_ARG(widget), void* user_data)
{
	GetSoundcard(0, user_data);
	if (setupflag)
		MainMenu(0, 0);
}

void FXCard(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_button_t* button7;
	txt_button_t* button8;

	window = TXT_NewWindow("Select Sound FX Device   ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 5);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("General Midi             "),
		button2 = TXT_NewButton("Sound Canvas             "),
		button3 = TXT_NewButton("Sound Blaster            "),
		button4 = TXT_NewButton("Pro Audio Spectrum       "),
		button5 = TXT_NewButton("UltraSound               "),
		button6 = TXT_NewButton("Adlib                    "),
		button7 = TXT_NewButton("PC Speaker               "),
		button8 = TXT_NewButton("NONE                     "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", SoundCardGM, (void*)"General Midi");
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", SoundCardGM, (void*)"Sound Canvas");
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", SoundCardSB, (void*)"Sound Blaster");
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", SoundCardProUl, (void*)"Pro Audio Spectrum");
	TXT_SignalConnect(button4, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", SoundCardProUl, (void*)"UltraSound");
	TXT_SignalConnect(button5, "pressed", SoundCardChannels, NULL);
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", SoundCardAdPCNo, (void*)"Adlib");
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button7, "pressed", SoundCardAdPCNo, (void*)"PC Speaker");
	TXT_SignalConnect(button7, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button8, "pressed", SoundCardAdPCNo, (void*)"NONE");
	TXT_SignalConnect(button8, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Select for General Midi Sound FX - MPU-401 General Midi");
	TXT_SetHelpLabel(button2, " Select for Roland Sound Canvas Sound FX ( Uses Build in FX on card )");
	TXT_SetHelpLabel(button3, " Digital Sound FX  - Sound Blaster, Pro, 16 and AWE32");
	TXT_SetHelpLabel(button4, " Digital Sound FX  - Pro Audio Spectrum");
	TXT_SetHelpLabel(button5, " Gravis UltraSound Digital Sound FX");
	TXT_SetHelpLabel(button6, " Sound FX ( Adlib and 100% compatibles )");
	TXT_SetHelpLabel(button7, " PC speaker Sound FX");
	TXT_SetHelpLabel(button8, " Select for No Sound FX");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (SoundCardType)
	{
	case 1:
		TXT_SelectWidget(table, button7);
		break;
	case 2:
		TXT_SelectWidget(table, button6);
		break;
	case 3:
		TXT_SelectWidget(table, button5);
		break;
	case 4:
		TXT_SelectWidget(table, button4);
		break;
	case 5:
		TXT_SelectWidget(table, button3);
		break;
	case 7:
		TXT_SelectWidget(table, button2);
		break;
	case 8:
		TXT_SelectWidget(table, button1);
		break;
	case 1000:
		TXT_SelectWidget(table, button8);
		break;
	default:
		TXT_SelectWidget(table, button3);
		break;
	}

	if (setupflag)
		TXT_SignalConnect(close_button, "pressed", MainMenu, NULL);

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}
///////////////////////////////////////Select Music Card////////////////////////////////////////////////////////////
void MusicCardGMSCWBSB32(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_button_t* button7;
	txt_button_t* button8;
	txt_button_t* button9;
	txt_button_t* button10;
	txt_button_t* button11;
	txt_button_t* button12;

	GetMusiccard(0, user_data);

	window = TXT_NewWindow("Available MIDI Ports       ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 3);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("220                        "),
		button2 = TXT_NewButton("230                        "),
		button3 = TXT_NewButton("240                        "),
		button4 = TXT_NewButton("250                        "),
		button5 = TXT_NewButton("300                        "),
		button6 = TXT_NewButton("320                        "),
		button7 = TXT_NewButton("330                        "),
		button8 = TXT_NewButton("332                        "),
		button9 = TXT_NewButton("334                        "),
		button10 = TXT_NewButton("336                        "),
		button11 = TXT_NewButton("340                        "),
		button12 = TXT_NewButton("360                        "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", GetMusicGMPort, (void*)"220");
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetMusicGMPort, (void*)"230");
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetMusicGMPort, (void*)"240");
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", GetMusicGMPort, (void*)"250");
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", GetMusicGMPort, (void*)"300");
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", GetMusicGMPort, (void*)"320");
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button7, "pressed", GetMusicGMPort, (void*)"330");
	TXT_SignalConnect(button7, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button8, "pressed", GetMusicGMPort, (void*)"332");
	TXT_SignalConnect(button8, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button9, "pressed", GetMusicGMPort, (void*)"334");
	TXT_SignalConnect(button9, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button10, "pressed", GetMusicGMPort, (void*)"336");
	TXT_SignalConnect(button10, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button11, "pressed", GetMusicGMPort, (void*)"340");
	TXT_SignalConnect(button11, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button12, "pressed", GetMusicGMPort, (void*)"360");
	TXT_SignalConnect(button12, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Sets Midi port to 220");
	TXT_SetHelpLabel(button2, " Sets Midi port to 230");
	TXT_SetHelpLabel(button3, " Sets Midi port to 240");
	TXT_SetHelpLabel(button4, " Sets Midi port to 250");
	TXT_SetHelpLabel(button5, " Sets Midi port to 300");
	TXT_SetHelpLabel(button6, " Sets Midi port to 320");
	TXT_SetHelpLabel(button7, " Sets Midi port to 330");
	TXT_SetHelpLabel(button8, " Sets Midi port to 332");
	TXT_SetHelpLabel(button9, " Sets Midi port to 334");
	TXT_SetHelpLabel(button10, " Sets Midi port to 336");
	TXT_SetHelpLabel(button11, " Sets Midi port to 340");
	TXT_SetHelpLabel(button12, " Sets Midi port to 360");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (MidiPort)
	{
	case 220:
		TXT_SelectWidget(table, button1);
		break;
	case 230:
		TXT_SelectWidget(table, button2);
		break;
	case 240:
		TXT_SelectWidget(table, button3);
		break;
	case 250:
		TXT_SelectWidget(table, button4);
		break;
	case 300:
		TXT_SelectWidget(table, button5);
		break;
	case 320:
		TXT_SelectWidget(table, button6);
		break;
	case 330:
		TXT_SelectWidget(table, button7);
		break;
	case 332:
		TXT_SelectWidget(table, button8);
		break;
	case 334:
		TXT_SelectWidget(table, button9);
		break;
	case 336:
		TXT_SelectWidget(table, button10);
		break;
	case 340:
		TXT_SelectWidget(table, button11);
		break;
	case 360:
		TXT_SelectWidget(table, button12);
		break;
	default:
		TXT_SelectWidget(table, button7);
		break;
	}

	if (setupflag)
	{
		TXT_SignalConnect(button1, "pressed", FXCard, NULL);
		TXT_SignalConnect(button2, "pressed", FXCard, NULL);
		TXT_SignalConnect(button3, "pressed", FXCard, NULL);
		TXT_SignalConnect(button4, "pressed", FXCard, NULL);
		TXT_SignalConnect(button5, "pressed", FXCard, NULL);
		TXT_SignalConnect(button6, "pressed", FXCard, NULL);
		TXT_SignalConnect(button7, "pressed", FXCard, NULL);
		TXT_SignalConnect(button8, "pressed", FXCard, NULL);
		TXT_SignalConnect(button9, "pressed", FXCard, NULL);
		TXT_SignalConnect(button10, "pressed", FXCard, NULL);
		TXT_SignalConnect(button11, "pressed", FXCard, NULL);
		TXT_SignalConnect(button12, "pressed", FXCard, NULL);
		TXT_SignalConnect(close_button, "pressed", FXCard, NULL);
	}

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}

void MusicCardSB(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_button_t* button7;

	GetMusiccard(0, user_data);

	window = TXT_NewWindow("Available PORTs            ");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 6);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("210                        "),
		button2 = TXT_NewButton("220                        "),
		button3 = TXT_NewButton("230                        "),
		button4 = TXT_NewButton("240                        "),
		button5 = TXT_NewButton("250                        "),
		button6 = TXT_NewButton("260                        "),
		button7 = TXT_NewButton("280                        "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button1, "pressed", GetMusicBasePort, (void*)"210");
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", GetMusicBasePort, (void*)"220");
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", GetMusicBasePort, (void*)"230");
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", GetMusicBasePort, (void*)"240");
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", GetMusicBasePort, (void*)"250");
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", GetMusicBasePort, (void*)"260");
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button7, "pressed", GetMusicBasePort, (void*)"280");
	TXT_SignalConnect(button7, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, " Sets PORT address to 210");
	TXT_SetHelpLabel(button2, " Sets PORT address to 220");
	TXT_SetHelpLabel(button3, " Sets PORT address to 230");
	TXT_SetHelpLabel(button4, " Sets PORT address to 240");
	TXT_SetHelpLabel(button5, " Sets PORT address to 250");
	TXT_SetHelpLabel(button6, " Sets PORT address to 260");
	TXT_SetHelpLabel(button7, " Sets PORT address to 280");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (BasePort)
	{
	case 210:
		TXT_SelectWidget(table, button1);
		break;
	case 220:
		TXT_SelectWidget(table, button2);
		break;
	case 230:
		TXT_SelectWidget(table, button3);
		break;
	case 240:
		TXT_SelectWidget(table, button4);
		break;
	case 250:
		TXT_SelectWidget(table, button5);
		break;
	case 260:
		TXT_SelectWidget(table, button6);
		break;
	case 280:
		TXT_SelectWidget(table, button7);
		break;
	default:
		TXT_SelectWidget(table, button2);
		break;
	}

	if (setupflag)
	{
		TXT_SignalConnect(button1, "pressed", FXCard, NULL);
		TXT_SignalConnect(button2, "pressed", FXCard, NULL);
		TXT_SignalConnect(button3, "pressed", FXCard, NULL);
		TXT_SignalConnect(button4, "pressed", FXCard, NULL);
		TXT_SignalConnect(button5, "pressed", FXCard, NULL);
		TXT_SignalConnect(button6, "pressed", FXCard, NULL);
		TXT_SignalConnect(button7, "pressed", FXCard, NULL);
		TXT_SignalConnect(close_button, "pressed", FXCard, NULL);
	}

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}

void MusicCardProUltraADNO(TXT_UNCAST_ARG(widget), void* user_data)
{
	GetMusiccard(0, user_data);
	if (setupflag)
		FXCard(0, 0);
}

void MusicCard(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_window_t* window;
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_button_t* button7;
	txt_button_t* button8;
	txt_button_t* button9;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;
	txt_table_t* table;

	window = TXT_NewWindow("Select Music Playback Device");
	TXT_SetWindowPosition(window, TXT_HORIZ_CENTER, TXT_VERT_TOP, 40, 5);
	table = TXT_NewTable(1);
	TXT_AddWidgets(table,
		button1 = TXT_NewButton("General Midi                "),
		button2 = TXT_NewButton("Sound Canvas                "),
		button3 = TXT_NewButton("WaveBlaster                 "),
		button4 = TXT_NewButton("SB AWE 32                   "),
		button5 = TXT_NewButton("Sound Blaster               "),
		button6 = TXT_NewButton("Pro Audio Spectrum          "),
		button7 = TXT_NewButton("UltraSound                  "),
		button8 = TXT_NewButton("Adlib                       "),
		button9 = TXT_NewButton("NONE                        "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, window);
	TXT_SignalConnect(button1, "pressed", MusicCardGMSCWBSB32, (void*)"General Midi");
	TXT_SignalConnect(button1, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button2, "pressed", MusicCardGMSCWBSB32, (void*)"Roland Sound Canvas");
	TXT_SignalConnect(button2, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button3, "pressed", MusicCardGMSCWBSB32, (void*)"WaveBlaster");
	TXT_SignalConnect(button3, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button4, "pressed", MusicCardGMSCWBSB32, (void*)"Sound Blaster AWE 32");
	TXT_SignalConnect(button4, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button5, "pressed", MusicCardSB, (void*)"Sound Blaster");
	TXT_SignalConnect(button5, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button6, "pressed", MusicCardProUltraADNO, (void*)"Pro Audio Spectrum");
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button7, "pressed", MusicCardProUltraADNO, (void*)"UltraSound");
	TXT_SignalConnect(button7, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button8, "pressed", MusicCardProUltraADNO, (void*)"Adlib");
	TXT_SignalConnect(button8, "pressed", ClosePwnBox, window);
	TXT_SignalConnect(button9, "pressed", MusicCardProUltraADNO, (void*)"NONE");
	TXT_SignalConnect(button9, "pressed", ClosePwnBox, window);

	TXT_SetHelpLabel(button1, "MPU-401 General MIDI");
	TXT_SetHelpLabel(button2, "Roland Sound Canvas");
	TXT_SetHelpLabel(button3, " WaveBlaster");
	TXT_SetHelpLabel(button4, " AWE 32 Sound Blaster");
	TXT_SetHelpLabel(button5, "Sound Blaster, Sound Blaster 16, Sound Blaster AWE32 and Sound Blaster Pro");
	TXT_SetHelpLabel(button6, " Pro Audio Spectrum");
	TXT_SetHelpLabel(button7, "Gravis UltraSound");
	TXT_SetHelpLabel(button8, "Adlib and 100% compatibles");
	TXT_SetHelpLabel(button9, " Do not use any Sound");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	switch (CardType)
	{
	case 2:
		TXT_SelectWidget(table, button8);
		break;
	case 3:
		TXT_SelectWidget(table, button7);
		break;
	case 4:
		TXT_SelectWidget(table, button6);
		break;
	case 5:
		TXT_SelectWidget(table, button5);
		break;
	case 6:
		TXT_SelectWidget(table, button3);
		break;
	case 7:
		TXT_SelectWidget(table, button2);
		break;
	case 8:
		TXT_SelectWidget(table, button1);
		break;
	case 9:
		TXT_SelectWidget(table, button4);
		break;
	case 1000:
		TXT_SelectWidget(table, button9);
		break;
	default:
		TXT_SelectWidget(table, button5);
		break;
	}

	if (setupflag)
		TXT_SignalConnect(close_button, "pressed", FXCard, NULL);

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(window, table);
	TXT_SetWindowAction(window, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(window, TXT_HORIZ_RIGHT, accept_button);
}
/////////////////////////////////////////////////////////Main Menu/////////////////////////////////////////////////////////
void MainMenu(TXT_UNCAST_ARG(widget), void* user_data)
{
	txt_button_t* button1;
	txt_button_t* button2;
	txt_button_t* button3;
	txt_button_t* button4;
	txt_button_t* button5;
	txt_button_t* button6;
	txt_table_t* table;
	txt_window_action_t* close_button;
	txt_window_action_t* accept_button;

	mainwindow = TXT_NewWindow("Main Menu                  ");
	TXT_SetWindowPosition(mainwindow, TXT_HORIZ_CENTER, TXT_VERT_TOP, 39, 10);
	table = TXT_NewTable(1);

	TXT_AddWidgets(table,
		button1 = TXT_NewButton("Select Music Card          "),
		button2 = TXT_NewButton("Select Sound FX Card       "),
		button3 = TXT_NewButton("Select Controller Type     "),
		button4 = TXT_NewButton("Controller Config          "),
		button5 = TXT_NewButton("Additional Features        "),
		button6 = TXT_NewButton("Save Settings              "),
		NULL);
	accept_button = TXT_NewWindowAction(KEY_ENTER, "Accept");
	close_button = TXT_NewWindowAction(KEY_ESCAPE, "Abort");

	TXT_SignalConnect(button1, "pressed", MusicCard, NULL);
	TXT_SignalConnect(button2, "pressed", FXCard, NULL);
	TXT_SignalConnect(button3, "pressed", Control, NULL);
	TXT_SignalConnect(button4, "pressed", ControlButtonConfig, NULL);
	TXT_SignalConnect(button5, "pressed", AdditionalFeatures, NULL);
	TXT_SignalConnect(button6, "pressed", SaveSettings, NULL);
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, mainwindow);
	TXT_SignalConnect(button6, "pressed", ClosePwnBox, infowindow);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, mainwindow);
	TXT_SignalConnect(close_button, "pressed", ClosePwnBox, infowindow);
	TXT_SignalConnect(accept_button, "pressed", WindowSelect, mainwindow);

	TXT_DrawHelpLabel(" Select Sound Card that will play Music");

	TXT_SetHelpLabel(button1, " Select Sound Card that will play Music");
	TXT_SetHelpLabel(button2, " Select Sound Card that will play Sound FX");
	TXT_SetHelpLabel(button3, " Select Controller Type ( Mouse / Joystick / Keyboard )");
	TXT_SetHelpLabel(button4, " Configure Keyboard / Mouse / Joystick Layout");
	TXT_SetHelpLabel(button5, " Configure Additional Features Video / Audio / Input");
	TXT_SetHelpLabel(button6, " Save Current Settings as Defaults");
	TXT_SetHelpLabel(close_button, " Press ESC to Abort");
	TXT_SetHelpLabel(accept_button, " Press ENTER to Accept");

	if (setupflag)
		TXT_SelectWidget(table, button6);

	TXT_SetWidgetFocus(mainwindow, 1);
	TXT_AddWidget(mainwindow, table);

	TXT_SetWindowAction(mainwindow, TXT_HORIZ_LEFT, close_button);
	TXT_SetWindowAction(mainwindow, TXT_HORIZ_RIGHT, accept_button);

	setupflag = 0;
}

int main(int argc, char* argv[])
{
	RAP_DataPath();

	if (access(RAP_GetSetupPath(), 0))                     //Check setup.ini is in folder
	{
		setupflag = 1;
		writesetupflag = 1;
	}

	INI_InitPreference(RAP_GetSetupPath());
	GetSetupSettings();

	TXT_Fullscreen(txt_fullscreen);

	if (!TXT_Init())
	{
		fprintf(stderr, "Failed to initialise GUI\n");
		exit(-1);
	}

	TXT_SetColor(TXT_COLOR_BLUE, 0x04, 0x14, 0x40);

	TXT_SetDesktopTitle("Raptor Setup ver 1.2                              (c) Cygnus Studios Inc. 1994");
	TXT_SetWindowTitle("Raptor Setup");

	if (!setupflag)
	{
		InfoWindow(0, 0);
		MainMenu(0, 0);
	}

	if (setupflag)
	{
		InfoWindow(0, 0);
		Control(0, 0);
	}

	TXT_GUIMainLoop();

	TXT_Shutdown();

	return 0;
}


