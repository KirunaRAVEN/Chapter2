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

/* Main storage struct. 36 bytes */
struct dataPoint {
    long int timestamp;

    // Pressures
    int N2OFeedingPressure1;      //Oxidizer feeding pressure
    int N2OFeedingPressure2;      //Oxidizer feeding pressure
    int linePressure;             //Line pressure
    int chamberPressure;          //Combustion chamber pressure
    int N2FeedingPressure;        //Nitrogen feeding pressure

    // Temperatures
    int bottleTemperature1;       //Bottle temperature
    int bottleTemperature2;       //Nozzle temperature
    int plumeTemperature;         //Plume temperature
    int pipingTemperature;        //Piping (green)
    int chamberTemperature;       //Chamber temperature (green and white)

    int loadcellReading;          //Loadcell measurement
};

/* Internal software storage struct. 4 bytes due to padding
Will always get sent with the datapoint, so no need for a timestamp */
struct softwareState {
    // Software modes
    uint8_t mode;                 //What mode is the software in
    uint8_t subState;             //What substate is the software in
    uint8_t message;

    // Button states
    bool dumpValveButton : 1;       //Is dump valve button pressed (normally open)
    bool heatingBlanketButton1 : 1; //Is heating button 1 pressed
    bool heatingBlanketButton2 : 1; //Is heating button 2 pressed
    bool ignitionButton : 1;        //Is ignition button pressed
    bool N2ValveButton : 1;         //Is N2 feeding valve button pressed (normally closed)
    bool N2OValveButton : 1;        //Is the oxidizer valve button pressed (normally closed)

    // Software control states
    bool valveActive : 1;           //Is the valve opened by the software
    bool ignitionEngagedActive : 1; //Is the ignition activated by the software
};

/* Normal packet. 48 bytes */
struct normalPacket {
    struct dataPoint data;
    struct softwareState state;
};

/* Functions */
class Comms{
public:
  Comms(){};
  int begin();
  int send(uint8_t type, void *buf, size_t size);
  int flush();
  int getNextMessage();
  int addMessage(uint8_t message);
private:
  uint8_t *pPacketBuffer;
  size_t packetBufferPtr;
  EthernetClient client;
  uint8_t *pMessageBuffer;
  size_t messageBufferReadPtr;
  size_t messageBufferWritePtr;
};

#endif /* COMMS_H */
