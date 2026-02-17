#include "globals.h"

/* global vars */
struct normalPacket g_packet;
ControlBoxRX g_controlBox;
breakoutPin g_outPins[] = {CAN1_TX, CAMERA_D0N, CAMERA_D1N, CAMERA_D2N, GPIO_4, GPIO_5, PWM0, PWM1, PWM2, PWM3, SPI1_CS};
breakoutPin g_inPins[] = {GPIO_0, PCIE_CKN, PCIE_TXP};
breakoutPin g_analogPins[] = {ANALOG_A0, ANALOG_A1, ANALOG_A2, ANALOG_A3, ANALOG_A4, ANALOG_A5, ANALOG_A6, ANALOG_A7};

/* non-global "global" vars */
long int lastLoopTime = 0;
long int fastLoopTime = 0;

UART UART0Breakout = UART(UART0_TX, UART0_RX);
Servo dumpValve;
#ifdef DEBUG
long int activeTime = 0;
int loops = 0;
long int lastLoopMicros = 0;
#endif


void setup() {
#ifdef DEBUG
    Serial.begin(9600);
    while (!Serial) {
        ;
    }
    Serial.println("initializing");
    Serial.println(sizeof(g_packet));
#endif


    for(auto i: g_outPins){
        Breakout.pinMode(i, OUTPUT);
        Breakout.digitalWrite(i, RELAY_OFF);
    }
    Breakout.digitalWrite(g_outPins[TEST_LED_SIGNAL], LOW);


    for(auto i: g_inPins) {
        Breakout.pinMode(i, INPUT);
    }

    dumpValve.attach(PWM8);

    g_packet.state.mode = INIT;
    g_controlBox.begin(&UART0Breakout);

    initComms(); // blocking
#ifdef DEBUG
    Serial.println("initialized");
#endif
}


void loop () {
    g_packet.data.timestamp = millis();
    g_packet.state.message = getNextMessage();

    int retVal = g_controlBox.receiveMessage();
#ifdef DEBUG
    if(millis() % 1000 < MAIN_LOOP_PERIOD) {
        Serial.print(byte(g_controlBox.getMessage().allButtons), BIN);
        Serial.print(", ");
        Serial.println(retVal);
    }
#endif

    /* TODO: break out into a wrapper */
    /* (and be made more readable) */
    if(0 == retVal) {
        if(SEQUENCE != g_packet.state.mode) {
            Breakout.digitalWrite(g_outPins[OXIDIZER1_RELAY], g_controlBox.getMessage().oxidizerButton ? LOW : HIGH);
            Breakout.digitalWrite(g_outPins[OXIDIZER2_RELAY], g_controlBox.getMessage().oxidizerButton ? LOW : HIGH);
            g_packet.state.N2OValveButton = g_controlBox.getMessage().oxidizerButton;
            Breakout.digitalWrite(g_outPins[NITROGEN_RELAY], g_controlBox.getMessage().nitrogenButton ? LOW : HIGH);
            g_packet.state.N2ValveButton = g_controlBox.getMessage().nitrogenButton;
            Breakout.digitalWrite(g_outPins[HEATING1_RELAY], g_controlBox.getMessage().heating1Switch ? LOW : HIGH);
            g_packet.state.heatingBlanketButton1 = g_controlBox.getMessage().heating1Switch;
            Breakout.digitalWrite(g_outPins[HEATING2_RELAY], g_controlBox.getMessage().heating2Switch ? LOW : HIGH);
            g_packet.state.heatingBlanketButton2 = g_controlBox.getMessage().heating2Switch;

            dumpValve.write((g_controlBox.getMessage().dumpButton ? DUMP_OPEN : DUMP_CLOSE));
            g_packet.state.dumpValveButton = g_controlBox.getMessage().dumpButton;
        }
        Breakout.digitalWrite(g_outPins[IGNITION_ARM], g_controlBox.getMessage().ignitionButton ? LOW : HIGH);
        g_packet.state.ignitionButton = g_controlBox.getMessage().ignitionButton;
    }
    readAllSensors();

    switch(g_packet.state.mode) {
        case INIT:
            if(true) { // == Breakout.digitalRead(g_inPins[TEST_MODE_BUTTON])) {
                g_packet.state.mode = TEST;
                Breakout.digitalWrite(g_outPins[TEST_LED_SIGNAL], HIGH);
            } else {
                g_packet.state.mode = WAIT;
            }
            break;
        case TEST:
            Breakout.digitalWrite(g_outPins[LIGHT_SIGNAL], RELAY_ON);
            if(0 == stepVerification()) {
                Breakout.digitalWrite(g_outPins[TEST_LED_SIGNAL], LOW);
                g_packet.state.mode = WAIT;
            }
            break;
        case WAIT:
            Breakout.digitalWrite(g_outPins[LIGHT_SIGNAL], RELAY_OFF);
            if(1 == g_controlBox.getMessage().ignitionButton) {
                g_packet.state.mode = SEQUENCE;
            }
            break;
        case SEQUENCE:
            Breakout.digitalWrite(g_outPins[LIGHT_SIGNAL], RELAY_ON);
            Breakout.digitalWrite(g_outPins[SIREN_SIGNAL], RELAY_ON);
            if(0 == stepSequence()) {
                g_packet.state.mode = SHUTDOWN;
            }
            break;
        case SAFE:
            Breakout.digitalWrite(g_outPins[LIGHT_SIGNAL], RELAY_ON);
            g_packet.state.subState = FINISHED;
            break;
        case SHUTDOWN:
            Breakout.digitalWrite(g_outPins[LIGHT_SIGNAL], RELAY_OFF);
            Breakout.digitalWrite(g_outPins[SIREN_SIGNAL], RELAY_OFF);
            Breakout.digitalWrite(g_outPins[HIGH_SPEED_SIGNAL], LOW);
            break;
        default:
            g_packet.state.mode = SAFE;
    }

    sendTelemetry(NORMAL_PACKET, (void *) &g_packet, sizeof(struct normalPacket));

    flushTelemetry();

#ifdef DEBUG
    activeTime += (micros() - lastLoopMicros);
    loops -=- 1; // >:3
    if(millis() % 1000 < MAIN_LOOP_PERIOD) {
        Serial.print("average execution time last second: ");
        Serial.print((float) activeTime/loops);
        Serial.print(" us, with a period of ");
        Serial.print(MAIN_LOOP_PERIOD);
        Serial.print(" ms. (");
        Serial.print(loops);
        Serial.print(" loops, ");
        Serial.print(activeTime);
        Serial.println(" us.)");
        activeTime = loops = 0;
    }
#endif

    //constant time loop
    while(millis() - lastLoopTime < MAIN_LOOP_PERIOD) {
        if(SEQUENCE == g_packet.state.mode) {
            float sample = fastRead();
            sendTelemetry(FAST_PACKET, (void *) &sample, sizeof(float));
            while(micros() - fastLoopTime < FAST_LOOP_PERIOD) {
                ;
            }
            fastLoopTime = micros();
        }
    }
    lastLoopTime = millis();

#ifdef DEBUG
    lastLoopMicros = micros();
#endif
}
