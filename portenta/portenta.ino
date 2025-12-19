#include "globals.h"

/* global vars */
struct normalPacket g_packet;
ControlBoxRX g_controlBox;
breakoutPin g_outPins[] = {CAN1_TX, CAMERA_D0N, CAMERA_D1N, CAMERA_D2N, GPIO_4, GPIO_5, PWM0, PWM1, PWM2, PWM9, SPI1_CS};
breakoutPin g_inPins[] = {GPIO_0, PCIE_CKN, PCIE_TXP};

/* non-global "global" vars */
long int lastLoopTime = 0;
UART UART0Breakout = UART(UART0_TX, UART0_RX);
#ifdef DEBUG 1
long int activeTime = 0;
int loops = 0;
#endif


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

    for(auto i: g_outPins){
        Breakout.pinMode(i, OUTPUT);
        Breakout.digitalWrite(i, RELAY_OFF);
    }

    for(auto i: g_inPins) {
        Breakout.pinMode(i, INPUT);
    }

    g_packet.state.mode = INIT;
    g_controlBox.begin(&UART0Breakout);

    Serial.println("initialized");
}



void loop () {
    g_packet.data.timestamp = millis();
    g_packet.state.message = getNextMessage();

    int retVal = g_controlBox.receiveMessage();
#ifdef DEBUG 1
    if(millis() % 1000 < MAIN_LOOP_PERIOD) {
        Serial.print(byte(g_controlBox.getMessage().allButtons), BIN);
        Serial.print(", ");
        Serial.println(retVal);
    }
#endif

    Breakout.digitalWrite(g_outPins[OXIDIZER_RELAY], g_controlBox.getMessage().oxidizerButton ? LOW : HIGH);
    Breakout.digitalWrite(g_outPins[NITROGEN_RELAY], g_controlBox.getMessage().nitrogenButton ? LOW : HIGH);
    Breakout.digitalWrite(g_outPins[HEATING1_RELAY], g_controlBox.getMessage().heating1Switch ? LOW : HIGH);
    Breakout.digitalWrite(g_outPins[HEATING2_RELAY], g_controlBox.getMessage().heating2Switch ? LOW : HIGH);
    Breakout.digitalWrite(g_outPins[IGNITION_ARM], g_controlBox.getMessage().ignitionButton ? LOW : HIGH);

    switch(g_packet.state.mode) {
        case INIT:
            if(true == Breakout.digitalRead(g_inPins[TEST_MODE_BUTTON])) {
                g_packet.state.mode = TEST;
                Breakout.digitalWrite(g_outPins[TEST_LED_SIGNAL], HIGH);
            } else {
                g_packet.state.mode = WAIT;
            }
            break;
        case TEST:
            if(0 == stepVerification()) {
                Breakout.digitalWrite(g_outPins[TEST_LED_SIGNAL], LOW);
                g_packet.state.mode = WAIT;
            }
            break;
        case WAIT:
            if(1 == g_controlBox.getMessage().ignitionButton) {
                g_packet.state.mode = SEQUENCE;
            }
            break;
        case SEQUENCE:
            if(0 == stepSequence()) {
                g_packet.state.mode = SHUTDOWN;
            }
            break;
        case SAFE:
            g_packet.state.subState = FINISHED;
            break;
        case SHUTDOWN:
            Breakout.digitalWrite(g_outPins[HIGH_SPEED_SIGNAL], LOW);
            break;
        default:
            g_packet.state.mode = SAFE;
    }

    sendTelemetry(NORMAL_PACKET, (void *) &g_packet, sizeof(struct normalPacket));

    //constant time loop
#ifdef DEBUG 1
    activeTime += (millis() - lastLoopTime);
    loops -=- 1; // >:3
    if(millis() % 1000 < MAIN_LOOP_PERIOD) {
        Serial.print("average execution time last second: ");
        Serial.print(activeTime/loops);
        Serial.print(" ms, with a period of ");
        Serial.print(MAIN_LOOP_PERIOD);
        Serial.println(" ms.");
        activeTime = loops = 0;
    }
#endif
    while(millis() - lastLoopTime < MAIN_LOOP_PERIOD) {
        ; // TODO: change this
    }
    lastLoopTime = millis();
}
