#include "comms.h"


// static uint8_t *pPacketBuffer;
// static size_t packetBufferPtr;
// static EthernetClient client;
// static uint8_t *pMessageBuffer;
// static size_t messageBufferReadPtr;
// static size_t messageBufferWritePtr;

int EthernetCommunication::begin() {
    /*
    Initialize communications between the test bench and the portenta.
    returns 0 on success and will retry on any failure
    BLOCKING.
    */

    // yes, i know this is ugly. I haven't found any other way to make it work
    byte mac[] = MAC;
    byte ip[] = IP;
    byte targetIP[] = TARGETIP;

    Ethernet.begin(mac, ip);

    while(false == client.connect(targetIP, PORT)) {
        Serial.println("[!] TCP error");
        delay(0.1);
    }
    while(NULL == (pPacketBuffer = (uint8_t *) malloc(BUFFERMEMORYSIZE*sizeof(uint8_t)))) {
        Serial.println("[!] Memory error");
        delay(0.1);
    }
    if(NULL == (pMessageBuffer = (uint8_t *) malloc(MESSAGEBUFFERSIZE*sizeof(uint8_t)))) {
        Serial.println("[!] Memory error");
        delay(0.1);
    }
    return 0;
}

int EthernetCommunication::send(uint8_t type, void *buf, size_t size) {
    /*
    Send a single packet of telemetry to the ground station.
    Output is currently buffered, so one call does not necessarily send packets.
    returns 0 on sent packet, and 1 if no packet was sent
    */
    pPacketBuffer[packetBufferPtr] = type;
    memcpy((void *) (pPacketBuffer+packetBufferPtr+1), buf, size);
    packetBufferPtr += size + 1;
    if(BUFFERSIZE <= packetBufferPtr) {
        flush();
    }
    return 1;
}

int EthernetCommunication::flush() {
    /*
    Flush the telemetry buffer (force a send)
    */
    client.write(pPacketBuffer, packetBufferPtr);
    packetBufferPtr = 0;
    return 0;
}

int EthernetCommunication::getNextMessage() {
    /*
    get the oldest message in the queue
    */
    if(messageBufferWritePtr == messageBufferReadPtr) {
        return 0;
    }
    uint8_t retval = pMessageBuffer[messageBufferReadPtr];
    messageBufferReadPtr = (messageBufferReadPtr+1) % MESSAGEBUFFERSIZE;
    return retval;
}

int EthernetCommunication::addMessage(uint8_t message) {
    /*
    Add a message to the message queue
    */
    pMessageBuffer[messageBufferWritePtr] = message;
    messageBufferWritePtr = (messageBufferWritePtr+1) % MESSAGEBUFFERSIZE;
#ifdef DEBUG
    Serial.println(message);
#endif
    return 0;
}

