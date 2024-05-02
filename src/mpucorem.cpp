#ifdef __APPLE__
#include <CoreMIDI/MIDIServices.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>

#include "musapi.h"
#include "fx.h"

MIDIPortRef m_port;
MIDIClientRef m_client;
MIDIEndpointRef m_endpoint;
MIDIPacket *m_pCurPacket;

int
COREM_Init(
    int subsystem
)
{
    // Get the MIDIEndPoint
    m_endpoint = 0;
    ItemCount numDests = MIDIGetNumberOfDestinations();
    ItemCount destId = numDests;
    m_endpoint = MIDIGetDestination(core_midi_port);
    
    // Create a MIDI client and port
    MIDIClientCreate(CFSTR("MyClient"), nullptr, nullptr, &m_client);
    
    if (!m_client)
    {
        printf("MIDI:COREMIDI: No client created.");
        return false;
    }
    
    MIDIOutputPortCreate(m_client, CFSTR("MyOutPort"), &m_port);
    
    if (!m_port)
    {
        printf("MIDI:COREMIDI: No port created.");
        return false;
    }
    
    return true;
}

void
COREM_DeInit(
    void
)
{
    // Dispose the port
    MIDIPortDispose(m_port);
    
    // Dispose the client
    MIDIClientDispose(m_client);
}

static unsigned int
MPU_MapChannel(
    unsigned chan
)
{
    if (chan < 9)
        return chan;
    
    if (chan == 15)
        return 9;
    
    return chan +1;
}

static void
KeyOffEvent(
    unsigned int chan,
    unsigned int key
)
{
    // Acquire a MIDIPacketList
    Byte packetBuf[128];
    MIDIPacketList* packetList = (MIDIPacketList*)packetBuf;
    m_pCurPacket = MIDIPacketListInit(packetList);
    
    Byte chanin = 0x80 | MPU_MapChannel(chan);
    Byte keyin = key;
    Byte noteoff[3] = {chanin, keyin, 0};
    
    // Add msg to the MIDIPacketList
    MIDIPacketListAdd(packetList,
                      (ByteCount)sizeof(packetBuf),
                      m_pCurPacket,
                      (MIDITimeStamp)0,
                      3,
                      noteoff);
    
    // Send the MIDIPacketList
    MIDISend(m_port, m_endpoint, packetList);
}

static void
KeyOnEvent(
    int chan,
    unsigned int key,
    unsigned int volume
)
{
    // Acquire a MIDIPacketList
    Byte packetBuf[128];
    MIDIPacketList* packetList = (MIDIPacketList*)packetBuf;
    m_pCurPacket = MIDIPacketListInit(packetList);
    
    Byte chanin = 0x90 | MPU_MapChannel(chan);
    Byte keyin = key;
    Byte volumein = volume;
    Byte noteon[3] = {chanin, keyin, volumein};
    
    // Add msg to the MIDIPacketList
    MIDIPacketListAdd(packetList,
                      (ByteCount)sizeof(packetBuf),
                      m_pCurPacket,
                      (MIDITimeStamp)1,
                      3,
                      noteon);
    
    // Send the MIDIPacketList
    MIDISend(m_port, m_endpoint, packetList);
}

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
    
    if (event_map[controller] == -1)
           return;
    
    // Acquire a MIDIPacketList
    Byte packetBuf[128];
    MIDIPacketList* packetList = (MIDIPacketList*)packetBuf;
    m_pCurPacket = MIDIPacketListInit(packetList);
    
    Byte chanin = 0xB0 | MPU_MapChannel(chan);
    Byte controllerin = event_map[controller];
    Byte paramin = param;
    Byte controlev[3] = {chanin, controllerin, paramin};
    
    // Add msg to the MIDIPacketList
    MIDIPacketListAdd(packetList,
                      (ByteCount)sizeof(packetBuf),
                      m_pCurPacket,
                      (MIDITimeStamp)1,
                      3,
                      controlev);
    
    // Send the MIDIPacketList
    MIDISend(m_port, m_endpoint, packetList);
}

static void
PitchBendEvent(
    unsigned int chan,
    int bend
)
{
    int lsb;
    int msb;
    
    bend <<= 6;
    
    lsb = bend;
    msb = bend;
    
    lsb &= 0x7F;
    msb >>= 7;
    
    // Acquire a MIDIPacketList
    Byte packetBuf[128];
    MIDIPacketList* packetList = (MIDIPacketList*)packetBuf;
    m_pCurPacket = MIDIPacketListInit(packetList);
    
    Byte chanin = 0xE0 | MPU_MapChannel(chan);
    Byte lsbin = lsb;
    Byte msbin = msb;
    Byte pitchbend[3] = {chanin, lsbin, msbin};
    
    // Add msg to the MIDIPacketList
    MIDIPacketListAdd(packetList,
                      (ByteCount)sizeof(packetBuf),
                      m_pCurPacket,
                      (MIDITimeStamp)0,
                      3,
                      pitchbend);
    
    // Send the MIDIPacketList
    MIDISend(m_port, m_endpoint, packetList);
}

static void
ProgramEvent(
    unsigned int chan,
    unsigned int param
)
{
    // Acquire a MIDIPacketList
    Byte packetBuf[128];
    MIDIPacketList* packetList = (MIDIPacketList*)packetBuf;
    m_pCurPacket = MIDIPacketListInit(packetList);
    
    Byte chanin = 0xC0 | MPU_MapChannel(chan);
    Byte paramin = param;
    Byte progev[3] = {chanin, paramin, 0};
    
    // Add msg to the MIDIPacketList
    MIDIPacketListAdd(packetList,
                      (ByteCount)sizeof(packetBuf),
                      m_pCurPacket,
                      (MIDITimeStamp)0,
                      3,
                      progev);
    
    // Send the MIDIPacketList
    MIDISend(m_port, m_endpoint, packetList);
}

static void
AllNotesOffEvent(
    unsigned int chan,
    unsigned int param
)
{
    // Acquire a MIDIPacketList
    Byte packetBuf[128];
    MIDIPacketList* packetList = (MIDIPacketList*)packetBuf;
    m_pCurPacket = MIDIPacketListInit(packetList);
    
    Byte chanin = 0x7B | chan;
    Byte paramin = param;
    Byte allnotesoff[3] = {chanin, paramin, 0};
    
    // Add msg to the MIDIPacketList
    MIDIPacketListAdd(packetList,
                      (ByteCount)sizeof(packetBuf),
                      m_pCurPacket,
                      (MIDITimeStamp)0,
                      3,
                      allnotesoff);
    
    // Send the MIDIPacketList
    MIDISend(m_port, m_endpoint, packetList);
}

musdevice_t mus_device_corem = {
    COREM_Init,
    COREM_DeInit,
    NULL,

    KeyOffEvent,
    KeyOnEvent,
    ControllerEvent,
    PitchBendEvent,
    ProgramEvent,
    AllNotesOffEvent
};
#endif
