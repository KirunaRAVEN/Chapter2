#include "comms.h"


uint8_t *pPacketBuffer;
size_t bufferPtr;
EthernetClient g_client;

int initComms() {
    /*
    Initialize communications between the test bench and the portenta.
    returns 0 on success and 1 on any failure
    */

    // yes, i know this is ugly. I haven't found any other way to make it work
    byte mac[] = MAC;
    byte ip[] = IP;
    byte targetIP[] = TARGETIP;

    if(0 == Ethernet.begin(mac, ip)) {
        return 1;
    }
    if(false == g_client.connect(targetIP, PORT)) {
        return 2;
    }
    if(NULL == (pPacketBuffer = (uint8_t *) malloc(BUFFERMEMORYSIZE*sizeof(uint8_t)))) {
        return 3;
    }
    return 0;
}

int sendTelemetry(uint8_t type, void *buf, size_t size) {
    /*
    Send a single packet of telemetry to the ground station.
    Output is currently buffered, so one call does not necessarily send packets.
    returns 0 on sent packet, and 1 if no packet was sent
    */
    pPacketBuffer[bufferPtr] = type;
    memcpy((void *) (pPacketBuffer+bufferPtr+1), buf, size);
    bufferPtr += size + 1;
    if(BUFFERSIZE <= bufferPtr) {
        g_client.write(pPacketBuffer, bufferPtr);
        bufferPtr = 0;
        return 0;
    }
    return 1;
}
