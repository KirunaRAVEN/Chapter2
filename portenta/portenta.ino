#include "globals.h"

#define PACKETLEN 40
/* global vars */
struct normalPacket g_packet;
ControlBoxRX g_controlBox;

/* non-global "global" vars */
long int lastLoopTime = 0;
UART UART0Breakout = UART(UART0_TX, UART0_RX);
breakoutPin relayPins[] = {PWM0, PWM1, PWM2, CAN1_TX, CAMERA_D0P, CAMERA_D0N, GPIO_4};


void setup() {
    Serial.begin(9600);
    while (!Serial) {
        ; // TODO: remove this from prod code, this forces Serial connection to boot
    }
    /*
#ifdef CORE_CM7
    bootM4();
#endif
    */

    Serial.println("initializing");
    Serial.println(sizeof(struct normalPacket));

    initComms(); // blocking

    for(auto i: relayPins){
        Breakout.pinMode(i, OUTPUT);
        Breakout.digitalWrite(i,HIGH);
    }

    g_packet.state.mode = INIT;
    g_controlBox.begin(&UART0Breakout);

    Breakout.digitalWrite(relayPins[RELAY1], LOW);
    delay(0.5);
    Breakout.digitalWrite(relayPins[RELAY1], HIGH);

    Serial.println("initialized");
}



void loop () {
    g_packet.data.timestamp = millis();
    g_packet.state.message = getNextMessage();

    int retVal = g_controlBox.receiveMessage();
    if(millis() % 1000 < MAIN_LOOP_PERIOD) {
        Serial.print(byte(g_controlBox.getMessage().allButtons), BIN);
        Serial.print(", ");
        Serial.println(retVal);
    }
    // TODO: actuate?
    switch(g_packet.state.mode) {
        case INIT:
            // TODO: read if we are in test mode or not
            g_packet.state.mode = TEST;
            break;
        case TEST:
            if(0 == stepVerification()) {
                g_packet.state.mode = WAIT;
            }
            break;
        case WAIT:
            Breakout.digitalWrite(relayPins[RELAY1], g_controlBox.getMessage().oxidizerValveButton ? HIGH : LOW);
            break;
        case SEQUENCE:

            break;
        case SAFE:
            g_packet.state.subState = FINISHED;
            break;
        case SHUTDOWN:
        
            break;
        default:
            g_packet.state.mode = SAFE;
    }

    sendTelemetry(NORMAL_PACKET, (void *) &g_packet, sizeof(struct normalPacket));

    //constant time loop
    while(millis() - lastLoopTime < MAIN_LOOP_PERIOD) {
        ; // TODO: change this
    }
    lastLoopTime = millis();
}
