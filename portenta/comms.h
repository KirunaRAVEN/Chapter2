/*
 * Contains all code for communication between the Portenta and the ground station.
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
class EthernetCommunication{
public:
  EthernetCommunication();
  int begin();
  int send(uint8_t type, void *buf, size_t size);
  int flush();
  int getNextMessage();
  int addMessage(uint8_t message);
private:
  static uint8_t *pPacketBuffer;
  static size_t packetBufferPtr;
  static EthernetClient client;
  static uint8_t *pMessageBuffer;
  static size_t messageBufferReadPtr;
  static size_t messageBufferWritePtr;
};

#endif /* COMMS_H */
