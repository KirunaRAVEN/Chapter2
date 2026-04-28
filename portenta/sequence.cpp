#include "sequence.h"

static SequenceContext g_seq;

static uint32_t elapsedTime() {
    return millis() - g_seq.startTime;
}

// -------------------------------------------------------
// Actions taken during each step during the count-up after the burn
static void ignitionOn() {
    digitalWrite(IGNITION_RELAY, RELAY_ON);
    g_packet.state.ignitionEngagedActive = 1;
}
static void ignitionOff(){
    digitalWrite(IGNITION_RELAY, RELAY_OFF);
    g_packet.state.ignitionEngagedActive = 0;
}
static void valveOn() {
    digitalWrite(OXIDIZER1_RELAY, RELAY_ON);
    digitalWrite(OXIDIZER2_RELAY, RELAY_ON);

    g_packet.state.N2OValveButton = 1;
    g_packet.state.valveActive = 1;
}
static void valveOff() {
    digitalWrite(OXIDIZER1_RELAY, RELAY_OFF);
    digitalWrite(OXIDIZER2_RELAY, RELAY_OFF);

    g_packet.state.N2OValveButton = 0;
    g_packet.state.valveActive = 0;
}
static void purgeOn() {
    digitalWrite(NITROGEN_RELAY, RELAY_ON);
}
static void purgeOff() {
    digitalWrite(NITROGEN_RELAY, RELAY_OFF);
}
static void cameraTrigger(){
    digitalWrite(HIGH_SPEED_SIGNAL, HIGH);
}
// -------------------------------------------------------

// -------------------------------------------------------
// List of all the steps during the count-up after the burn
// Follows the following sequence:
//      1. Time after burn
//      2. Action to do
//      3. sets the current state (g_packet)
static const TimedStep steps[] = {
    {
        0,
        ignitionOn,
        IGNIT_ON
    },
    {
        VALVE_ON_TIME,
        valveOn,
        VALVE_ON
    },
    {
        IGNITION_OFF_TIME,
        ignitionOff,
        IGNIT_OFF
    },
    {
        VALVE_OFF_TIME,
        valveOff,
        VALVE_OFF
    },
    {
        OXIDIZER_EMPTY_TIME,
        purgeOn,
        PURGING
    },
    {
        CAMERA_TRIGGER_TIME,
        cameraTrigger,
        PURGING
    },
    {
        PURGING_TIME,
        purgeOff,
        FINISHED
    }
};

static size_t STEP_COUNT = sizeof(steps)/sizeof(steps[0]);
// -------------------------------------------------------

// -------------------------------------------------------
// If we want to do a relaunch sequence
void resetSequence() {
    g_seq = {};

    g_packet.state.subState = ALL_OFF;
    g_packet.state.ignitionEngagedActive = 0;
    g_packet.state.N2OValveButton = 0;
    g_packet.state.valveActive = 0;
}
// -------------------------------------------------------

// -------------------------------------------------------
// Check weather or not the launch is authorized
static bool launchAuthorized() {
    static uint32_t holdStart = 0;

    auto buttons = g_controlBox.getMessage();

    if(buttons.ignitionButton == 0) {
        holdStart = millis();
        g_packet.state.mode = WAIT;
        return false;
    }

    if(millis() - holdStart >= IGNITION_SAFE_TIME && buttons.allButtons == 1) {
        holdStart = 0;
        return true;
    }

    return false;
}
// -------------------------------------------------------

// -------------------------------------------------------
// The actual step function that goes through all the steps after the burn
int stepSequence() {
    if(!g_seq.running) {
        if(launchAuthorized()){
            g_seq.running = true;
            g_seq.stepIndex = 0;
            g_seq.startTime = 0;
        }
        return 1;
    }

    while(g_seq.stepIndex < STEP_COUNT && elapsedTime() >= steps[g_seq.stepIndex].triggerTimeMs) {
        const TimedStep& step = steps[g_seq.stepIndex];

        step.action;
        g_packet.state.subState = step.nextState;
        g_seq.stepIndex++;
    }
    if(g_packet.state.subState == FINISHED)
        return 0;

    return 1;
}
// -------------------------------------------------------