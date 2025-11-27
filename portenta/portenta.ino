#include "globals.h"

#define PACKETLEN 40
/* global vars */
char g_packet[PACKETLEN];


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
    for(int i = 0; i<PACKETLEN; i++) {
        g_packet[i] = 'A';
    }
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
    sendTelemetry((void *) g_packet, PACKETLEN*sizeof(char));
    while (RPC.available()) {
        Serial.write(RPC.read());
    }
}
