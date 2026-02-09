#include "sequence.h"

static long int countdownStartTime = 0;

int stepSequence() {
    /*
    coroutine for ignition sequence. Returns 0 once finished, and 1 otherwise.
    */
    switch(g_packet.state.subState) {
        case ALL_OFF: // hold until button is pressed for long enough
            if(0 == countdownStartTime) {
                countdownStartTime = millis();
            }
            if(0 == g_controlBox.getMessage().ignitionButton) {
                countdownStartTime = 0;
                g_packet.state.mode = WAIT;
            }
            if(millis() - countdownStartTime > IGNITION_SAFE_TIME) {
                if(1 == g_controlBox.getMessage().allButtons) {
                    countdownStartTime = millis();
                    g_packet.state.subState = IGNIT_ON;
                    Breakout.digitalWrite(g_outPins[IGNITION_RELAY], RELAY_ON);
                    g_packet.state.ignitionEngagedActive = 1;
                }
            }
            break;

        case IGNIT_ON:
            if(millis() - countdownStartTime > VALVE_ON_TIME) {
                g_packet.state.subState = VALVE_ON;
                Breakout.digitalWrite(g_outPins[OXIDIZER1_RELAY], RELAY_ON);
                Breakout.digitalWrite(g_outPins[OXIDIZER2_RELAY], RELAY_ON);
                g_packet.state.valveActive = 1;
            }
            break;

        case VALVE_ON:
            if(millis() - countdownStartTime > IGNITION_OFF_TIME) {
                g_packet.state.subState = IGNIT_OFF;
                Breakout.digitalWrite(g_outPins[OXIDIZER1_RELAY], RELAY_OFF);
                Breakout.digitalWrite(g_outPins[OXIDIZER2_RELAY], RELAY_OFF);
                g_packet.state.valveActive = 0;
            }
            break;

        case IGNIT_OFF:
            if(millis() - countdownStartTime > VALVE_OFF_TIME) {
                g_packet.state.subState = VALVE_OFF;
                Breakout.digitalWrite(g_outPins[IGNITION_RELAY], RELAY_OFF);
                g_packet.state.ignitionEngagedActive = 0;
            }
            break;

        case VALVE_OFF:
            if(millis() - countdownStartTime > OXIDIZER_EMPTY_TIME) {
                g_packet.state.subState = PURGING;
                Breakout.digitalWrite(g_outPins[NITROGEN_RELAY], RELAY_ON);
            }
            break;

        case PURGING:
            if(millis() - countdownStartTime > CAMERA_TRIGGER_TIME) {
                Breakout.digitalWrite(g_outPins[HIGH_SPEED_SIGNAL], HIGH);
            }

            if(millis() - countdownStartTime > OXIDIZER_EMPTY_TIME) {
                g_packet.state.subState = FINISHED;
                Breakout.digitalWrite(g_outPins[NITROGEN_RELAY], RELAY_OFF);
            }
            break;

        case FINISHED:
            return 0;
    }
    return 1;
}
