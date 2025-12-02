#include "globals.h"

#define PACKETLEN 40
/* global vars */
struct normalPacket g_packet;
long int lastLoopTime = 0

void setup() {
    Serial.begin(9600);

    while (!Serial) {
        ; // TODO: remove this from prod code, this forces Serial connection to boot
    }

#ifdef CORE_CM7
    bootM4();
#endif
    RPC.begin();
    Serial.println("initializing");

    Serial.println(sizeof(struct normalPacket));
    int retVal = initComms();
    while(0 != retVal) {
        //TODO: discuss this with team, as this will block 'proper' boot until connected to the GS.
        switch(retVal) {
            case 1:
                Serial.println("Ethernet Error");
                break;
            case 2:
                Serial.print("TCP Error, cannot connect from ");
                Serial.println(Ethernet.localIP());
                break;
            case 3:
                Serial.println("Memory Error");
                break;
        }
        retVal = initComms();
    }
    Serial.println("initialized");
}

void loop () {
    g_packet.data.timestamp = millis();
    sendTelemetry(NORMAL_PACKET, (void *) g_packet, sizeof(struct normalPacket));
    while (RPC.available()) {
        Serial.write(RPC.read());
    }
    //constant time loop
    while(millis() - lastLoopTime < 100) {
        ; // TODO: change this
    }
    lastLoopTime = millis();
}
