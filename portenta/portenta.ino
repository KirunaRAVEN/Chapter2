#include "globals.h"

/* global vars */
struct normalPacket g_packet;
ControlBoxRX g_controlBox;
int g_outRelays[] = {SIREN_SIGNAL, LIGHT_SIGNAL, HEATING1_RELAY, HEATING2_RELAY, IGNITION_ARM, IGNITION_RELAY, NITROGEN_RELAY, OXIDIZER1_RELAY, OXIDIZER2_RELAY};
int g_outPins[] = {TEST_LED_SIGNAL, HIGH_SPEED_SIGNAL, ERROR_LED_SIGNAL};
int g_inPins[] = {TEST_MODE_BUTTON};
int g_analogPins[] = {OXIDIZER1_TEMP, OXIDIZER1_PRESSURE, OXIDIZER2_TEMP, OXIDIZER1_PRESSURE, NITROGEN_PRESSURE, LINE_PRESSURE, PLUME_TEMP, CHAMBER_PRESSURE};

/* non-global "global" vars */
long int lastLoopTime = 0;
long int fastLoopTime = 0;

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


    for(auto i: g_outRelays){
        pinMode(i, OUTPUT);
        digitalWrite(i, RELAY_OFF);
    }

    for(auto i: g_outPins){
        pinMode(i, OUTPUT);
        digitalWrite(i, LOW);
    }


    for(auto i: g_inPins) {
        pinMode(i, INPUT_PULLUP);
    }

    dumpValve.attach(DUMP_PIN);

    g_packet.state.mode = INIT;
    g_controlBox.begin(&Serial2);

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
            digitalWrite(OXIDIZER1_RELAY, g_controlBox.getMessage().oxidizerButton ? LOW : HIGH);
            digitalWrite(OXIDIZER2_RELAY, g_controlBox.getMessage().oxidizerButton ? LOW : HIGH);
            g_packet.state.N2OValveButton = g_controlBox.getMessage().oxidizerButton;
            digitalWrite(NITROGEN_RELAY, g_controlBox.getMessage().nitrogenButton ? LOW : HIGH);
            g_packet.state.N2ValveButton = g_controlBox.getMessage().nitrogenButton;
            digitalWrite(HEATING1_RELAY, g_controlBox.getMessage().heating1Switch ? LOW : HIGH);
            g_packet.state.heatingBlanketButton1 = g_controlBox.getMessage().heating1Switch;
            digitalWrite(HEATING2_RELAY, g_controlBox.getMessage().heating2Switch ? LOW : HIGH);
            g_packet.state.heatingBlanketButton2 = g_controlBox.getMessage().heating2Switch;

            dumpValve.write((g_controlBox.getMessage().dumpButton ? DUMP_OPEN : DUMP_CLOSE));
            g_packet.state.dumpValveButton = g_controlBox.getMessage().dumpButton;
        }
        digitalWrite(IGNITION_ARM, g_controlBox.getMessage().ignitionButton ? LOW : HIGH);
        g_packet.state.ignitionButton = g_controlBox.getMessage().ignitionButton;
    }
    readAllSensors();

    /* switch on the light if pressure in the system */
    if (g_packet.data.linePressure >= 2.0) {
        digitalWrite(LIGHT_SIGNAL, RELAY_ON);
    } else {
        digitalWrite(LIGHT_SIGNAL, RELAY_OFF);

    }

    switch(g_packet.state.mode) {
        case INIT:
            if(LOW == digitalRead(TEST_MODE_BUTTON)) {
                g_packet.state.mode = TEST;
                digitalWrite(TEST_LED_SIGNAL, HIGH);
            } else {
                g_packet.state.mode = WAIT;
            }
            break;
        case TEST:
            digitalWrite(LIGHT_SIGNAL, RELAY_ON);
            if(0 == stepVerification()) {
                digitalWrite(TEST_LED_SIGNAL, LOW);
                g_packet.state.mode = WAIT;
                digitalWrite(LIGHT_SIGNAL, RELAY_OFF);
            }
            break;
        case WAIT:
            if(1 == g_controlBox.getMessage().ignitionButton) {
                g_packet.state.mode = SEQUENCE;
            }
            break;
        case SEQUENCE:
            digitalWrite(LIGHT_SIGNAL, RELAY_ON);
            digitalWrite(SIREN_SIGNAL, RELAY_ON);
            if(0 == stepSequence()) {
                g_packet.state.mode = SHUTDOWN;
                digitalWrite(LIGHT_SIGNAL, RELAY_OFF);
                digitalWrite(SIREN_SIGNAL, RELAY_OFF);
            }
            break;
        case SAFE:
            digitalWrite(LIGHT_SIGNAL, RELAY_ON);
            g_packet.state.subState = FINISHED;
            break;
        case SHUTDOWN:
            digitalWrite(HIGH_SPEED_SIGNAL, LOW);
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
