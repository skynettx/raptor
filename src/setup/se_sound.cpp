#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "textscreen.h"
#include "main.h"

int SoundCardType;
int SoundMidiPort;
int SoundBasePort;
int SoundIrq;
int SoundDma;
int SoundChannels;

void GetSoundcard(TXT_UNCAST_ARG(widget), void* user_data)
{
	if (strcmp((char*)user_data, "General Midi") == 0)
	{
		SoundCardType = 8;
		textbox_soundfx = (char*)"General Midi                ";
	}
	if (strcmp((char*)user_data, "Sound Canvas") == 0)
	{
		SoundCardType = 7;
		textbox_soundfx = (char*)"Roland Sound Canvas         ";
	}
	if (strcmp((char*)user_data, "Sound Blaster") == 0)
	{
		SoundCardType = 5;
		textbox_soundfx = (char*)"Sound Blaster               ";
	}
	if (strcmp((char*)user_data, "Pro Audio Spectrum") == 0)
	{
		SoundCardType = 4;
		textbox_soundfx = (char*)"Pro Audio Spectrum          ";
	}
	if (strcmp((char*)user_data, "UltraSound") == 0)
	{
		SoundCardType = 3;
		textbox_soundfx = (char*)"Gravis Ultra Sound          ";
	}
	if (strcmp((char*)user_data, "Adlib") == 0)
	{
		SoundCardType = 2;
		textbox_soundfx = (char*)"Adlib                       ";
	}
	if (strcmp((char*)user_data, "PC Speaker") == 0)
	{
		SoundCardType = 1;
		textbox_soundfx = (char*)"PC Speaker                  ";
	}
	if (strcmp((char*)user_data, "NONE") == 0)
	{
		SoundCardType = 1000;
		textbox_soundfx = (char*)"None                        ";
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

void GetSoundChannels(TXT_UNCAST_ARG(widget), void* user_data)
{
	if (strcmp((char*)user_data, "One") == 0)
		SoundChannels = 1;
	if (strcmp((char*)user_data, "Two") == 0)
		SoundChannels = 2;
	if (strcmp((char*)user_data, "Three") == 0)
		SoundChannels = 3;
	if (strcmp((char*)user_data, "Four") == 0)
		SoundChannels = 4;
	if (strcmp((char*)user_data, "Five") == 0)
		SoundChannels = 5;
	if (strcmp((char*)user_data, "Six") == 0)
		SoundChannels = 6;
	if (strcmp((char*)user_data, "Seven") == 0)
		SoundChannels = 7;
	if (strcmp((char*)user_data, "Eight") == 0)
		SoundChannels = 8;
}

void GetSoundDma(TXT_UNCAST_ARG(widget), void* user_data)
{
	if (strcmp((char*)user_data, "0") == 0)
		SoundDma = 1000;                   
	if (strcmp((char*)user_data, "1") == 0)
		SoundDma = 1;
	if (strcmp((char*)user_data, "3") == 0)
		SoundDma = 3;
	if (strcmp((char*)user_data, "5") == 0)
		SoundDma = 5;
	if (strcmp((char*)user_data, "6") == 0)
		SoundDma = 6;
	if (strcmp((char*)user_data, "7") == 0)
		SoundDma = 7;
}

void GetSoundIrq(TXT_UNCAST_ARG(widget), void* user_data)
{
	if (strcmp((char*)user_data, "2") == 0)
		SoundIrq = 2;
	if (strcmp((char*)user_data, "5") == 0)
		SoundIrq = 5;
	if (strcmp((char*)user_data, "7") == 0)
		SoundIrq = 7;
	if (strcmp((char*)user_data, "10") == 0)
		SoundIrq = 10;
}

void GetSoundBasePort(TXT_UNCAST_ARG(widget), void* user_data)
{
	if (strcmp((char*)user_data, "210") == 0)
		SoundBasePort = 210;
	if (strcmp((char*)user_data, "220") == 0)
		SoundBasePort = 220;
	if (strcmp((char*)user_data, "230") == 0)
		SoundBasePort = 230;
	if (strcmp((char*)user_data, "240") == 0)
		SoundBasePort = 240;
	if (strcmp((char*)user_data, "250") == 0)
		SoundBasePort = 250;
	if (strcmp((char*)user_data, "260") == 0)
		SoundBasePort = 260;
	if (strcmp((char*)user_data, "280") == 0)
		SoundBasePort = 280;
}

void GetSoundGMPort(TXT_UNCAST_ARG(widget), void* user_data)
{
	if (strcmp((char*)user_data, "220") == 0)
		SoundMidiPort = 220;
	if (strcmp((char*)user_data, "230") == 0)
		SoundMidiPort = 230;
	if (strcmp((char*)user_data, "240") == 0)
		SoundMidiPort = 240;
	if (strcmp((char*)user_data, "250") == 0)
		SoundMidiPort = 250;
	if (strcmp((char*)user_data, "300") == 0)
		SoundMidiPort = 300;
	if (strcmp((char*)user_data, "320") == 0)
		SoundMidiPort = 320;
	if (strcmp((char*)user_data, "330") == 0)
		SoundMidiPort = 330;
	if (strcmp((char*)user_data, "332") == 0)
		SoundMidiPort = 332;
	if (strcmp((char*)user_data, "334") == 0)
		SoundMidiPort = 334;
	if (strcmp((char*)user_data, "336") == 0)
		SoundMidiPort = 336;
	if (strcmp((char*)user_data, "340") == 0)
		SoundMidiPort = 340;
	if (strcmp((char*)user_data, "360") == 0)
		SoundMidiPort = 360;
}
