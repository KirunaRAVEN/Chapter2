#include <Ethernet.h>
#include <PortentaEthernet.h>
#include "comms.h"
#include "globals.h"

EthernetClient initComms() {
    /*
    Initialize communications between the test bench and the portenta
    */
    EthernetClient client;
    if(0 == Ethernet.begin(MAC, IP)) {
        return client; // TODO: return error state
    }

    if(0 == client.connect(TARGETIP, PORT)) {
        return client;
    }
    return client;
}

int sendTelemetry(EthernetClient client, void * buf, size_t size) {
    /*
    Send a single packet of telemetry to the ground station. unbuffered at the moment.
    */
    client.write((uint8_t *) buf, size);
}
