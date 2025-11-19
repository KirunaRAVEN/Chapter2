/*
 * Contains all code for communication between the Portenta and the ground station.
 * TODO: buffer output
 * TODO: reverse client/server relationship?
*/
#ifndef COMMS_H
#define COMMS_H

#include <Ethernet.h>
#include <PortentaEthernet.h>
#include <SPI.h>
#include "globals.h"


/* Network details */
const byte TARGETIP[] = {192,168,0,1};
const byte MAC[] = {0x00, 0x20, 0x91, 0xDA, 0xBE, 0xEF};
const byte IP[] = {192,168,0,2};
const int PORT = 4000;


/* Functions */
EthernetClient initComms();

int sendTelemetry(EthernetClient client, void * buf, size_t size);

#endif /* COMMS_H */
