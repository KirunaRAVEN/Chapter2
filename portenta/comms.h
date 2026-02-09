/*
 * Contains all code for communication between the Portenta and the ground station.
 * TODO: reverse client/server relationship?
 */

/*
 * Note from real-world testing on actual hardware:
 * Maximum rate seems to be about 1 mbps @ 1.1k packets/s if we max out the MSS (536 bytes/packet)
 */
#ifndef COMMS_H
#define COMMS_H

#include <Ethernet.h>
#include <SPI.h>
#include "globals.h"


/* Network details */
#define TARGETIP {192,168,0,1}
#define MAC      {0x00, 0x20, 0x91, 0xDA, 0xBE, 0xEF}
#define IP       {192,168,0,2}
#define PORT     4000

#define BUFFERSIZE 536
#define BUFFERMEMORYSIZE (BUFFERSIZE+80)
enum packettypes {NORMAL_PACKET=1, FAST_PACKET=2};
#define MESSAGEBUFFERSIZE 16

/* Functions */
int initComms();
int sendTelemetry(uint8_t type, void *buf, size_t size);
int getNextMessage();
int addMessage(uint8_t message);

/* Global variables */

#endif /* COMMS_H */
