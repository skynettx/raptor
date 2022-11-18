#ifdef __linux__
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <alsa/asoundlib.h>
#include "SDL.h"

#include "common.h"
#include "musapi.h"
#include "fx.h"

static snd_seq_t* seq;
static int client;
static snd_seq_event_t ev;

/***************************************************************************
fatal() - Prints an error message to stderr, and dies
 ***************************************************************************/
static void 
fatal(
	const char* msg, 
	...
)
{
	va_list ap;

	va_start(ap, msg);
	vfprintf(stderr, msg, ap);
	va_end(ap);
	fputc('\n', stderr);
	exit(EXIT_FAILURE);
}

/***************************************************************************
check_snd() - Error handling for ALSA functions
 ***************************************************************************/
static void 
check_snd(
	const char* operation, 
	int err
)
{
	if (err < 0)
		fatal("Cannot %s - %s", operation, snd_strerror(err));
}

/***************************************************************************
ALSA_Init() - 
 ***************************************************************************/
int 
ALSA_Init(
	int option
) 
{
	int err;
	
    // ALSA seq
	err = snd_seq_open(&seq, "default", SND_SEQ_OPEN_OUTPUT, 0);
	
	if (err) 
	{
		printf("Error opening ALSA seq: %s\n", snd_strerror(err));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Raptor", "Error opening ALSA seq", NULL);
		exit(err);
	}
	
	err = snd_seq_set_client_name(seq, "Raptor");
	check_snd("set client name", err);
	client = snd_seq_client_id(seq);
	check_snd("get client id", client);
	
	int port = snd_seq_create_simple_port(seq, "Raptor",
		SND_SEQ_PORT_CAP_READ | SND_SEQ_PORT_CAP_SUBS_READ | SND_SEQ_PORT_CAP_SYNC_READ,
		SND_SEQ_PORT_TYPE_APPLICATION | SND_SEQ_PORT_TYPE_MIDI_GENERIC);
	
	if (port < 0) 
	{
		printf("Error opening ALSA port: %s\n", snd_strerror(port));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Raptor", "Error opening ALSA port", NULL);
		snd_seq_close(seq);
		exit(port);
	}
	
	err = snd_seq_connect_to(seq, port, alsaclient, alsaport);
	
	if (err) 
	{
		printf("Couldn't connect to synth: %s\n", snd_strerror(err));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Raptor", "Couldn't connect to synth", NULL);
		snd_seq_close(seq);
		exit(err);
	}
	
	err = snd_seq_set_output_buffer_size(seq, 512);
	
	if (err) 
	{
		printf("Couldn't set buffer size: %s\n", snd_strerror(err));
		SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
			"Raptor", "Couldn't set buffer size", NULL);
		snd_seq_close(seq);
		exit(err);
	}
	
	printf("%d:%d connected to %u:%u\n", client, 0, alsaclient, alsaport);
	
	snd_seq_ev_clear(&ev);
	snd_seq_ev_set_source(&ev, 0);
	snd_seq_ev_set_subs(&ev);
	snd_seq_ev_set_direct(&ev);

	return 1;
}

/***************************************************************************
ALSA_DeInit() -
 ***************************************************************************/
void 
ALSA_DeInit(
	void
) 
{
	snd_seq_close(seq);
}

/***************************************************************************
MPU_MapChannel() -
 ***************************************************************************/
static unsigned int 
MPU_MapChannel(
	unsigned chan
)
{
	if (chan < 9)
		return chan;
	
	if (chan == 15)
		return 9;
	
	return chan + 1;
}

/***************************************************************************
KeyOffEvent() -
 ***************************************************************************/
static void 
KeyOffEvent(
	unsigned int chan, 
	unsigned int key
)
{
	int err;
	
	snd_seq_ev_set_noteoff(&ev, MPU_MapChannel(chan), key, 0);
	err = snd_seq_event_output_direct(seq, &ev);
	check_snd("KeyOffEvent", err);
}

/***************************************************************************
KeyOnEvent() -
 ***************************************************************************/
static void 
KeyOnEvent(
	int chan, 
	unsigned int key, 
	unsigned int volume
) 
{
	int err;
	
	snd_seq_ev_set_noteon(&ev, MPU_MapChannel(chan), key, volume);
	err = snd_seq_event_output_direct(seq, &ev);
	check_snd("KeyOnEvent", err);
}

/***************************************************************************
ProgramEvent() -
 ***************************************************************************/
static void 
ProgramEvent(
	unsigned int chan, 
	unsigned int param
) 
{
	int err;
	
	snd_seq_ev_set_pgmchange(&ev, MPU_MapChannel(chan), param);
	err = snd_seq_event_output_direct(seq, &ev);
	check_snd("ProgramEvent", err);
}

/***************************************************************************
PitchBendEvent() -
 ***************************************************************************/
static void 
PitchBendEvent(
	unsigned int chan, 
	int bend
) 
{
	int err;
	
	snd_seq_ev_set_pitchbend(&ev, MPU_MapChannel(chan), bend);
	err = snd_seq_event_output_direct(seq, &ev);
	check_snd("PitchBendEvent", err);
}

/***************************************************************************
AllNotesOffEvent() -
 ***************************************************************************/
static void 
AllNotesOffEvent(
	unsigned int chan, 
	unsigned int param
) 
{
	int err;
	
	snd_seq_ev_set_pgmchange(&ev, chan, param);
	err = snd_seq_event_output_direct(seq, &ev);
	check_snd("AllNotesOffEvent", err);
}

/***************************************************************************
ControllerEvent() -
 ***************************************************************************/
static void 
ControllerEvent(
	unsigned int chan, 
	unsigned int controller, 
	unsigned int param
)
{
    static int event_map[] = {
	  0, 0, 1, 7, 10, 11, 91, 93, 64, 67, 120, 123, -1, -1, 121, -1
	};
	int err;
	
	snd_seq_ev_set_controller(&ev, MPU_MapChannel(chan), event_map[controller], param);
	err = snd_seq_event_output_direct(seq, &ev);
	check_snd("ControllerEvent", err);
}

musdevice_t mus_device_alsa = {
	ALSA_Init,
	ALSA_DeInit,
	NULL,

    KeyOffEvent,
    KeyOnEvent,
    ControllerEvent,
    PitchBendEvent,
	ProgramEvent,
	AllNotesOffEvent,
};
#endif
