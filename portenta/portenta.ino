#include <Ethernet.h>
#include <PortentaEthernet.h>
#include <SPI.h>
#include "globals.h"

#define PACKETLEN 10

EthernetClient g_client;
char g_packet[PACKETLEN];

void setup() {
    Serial.begin(9600);

    while (!Serial) {
        ; // TODO: remove this from prod code, this forces Serial connection to boot
    }

    g_client = initComms();
    for(int i = 0; i<PACKETLEN; i++) {
        g_packet[i] = 'A';
    }
}

void loop () {
    sendTelemetry(g_client, (void *) g_packet, PACKETLEN*sizeof(char));
}
