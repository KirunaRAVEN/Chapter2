#include "verification.h"

static int testState = TEST_START;
static int testStateChangeTime;

int _testActuator(bool passed, uint8_t message) {
    /*
    test a single actuator, with all the bells and whisles around it
    */
    addMessage(message);
    if(passed) {
        addMessage(MSG_PASS);
        return 0;
    } else {
        addMessage(MSG_FAIL);
        return 1;
    }
}

int stepVerification() {
    /*
    Main verification function. Returns 0 once all tests have passed, and 1 otherwise.
    */
    union ControlBoxStateMessage buttonStates = g_controlBox.getMessage();
    switch(testState) {
        case TEST_START:
            addMessage(MSG_TEST_SEQUENCE_START);
            testState = OFF_STATE_BUTTON;
            break;
        case OFF_STATE_BUTTON:
            if(0 == buttonStates.allButtons) {
                addMessage(MSG_NO_BUTTONS);
                testState = OFF_STATE_TEST;
                testStateChangeTime = millis();
            } else {
                if(millis() % 1000 < MAIN_LOOP_PERIOD) {
                    addMessage(MSG_RELEASE_OX);
                }
            }
            break;
        case OFF_STATE_TEST:
            //TODO: Change this once we have control sensing
            if(millis() - testStateChangeTime > ACTUATOR_SETTLE_TIME) {
                int testsFailed = 0;
                testsFailed += _testActuator((false == buttonStates.ignitionButton), MSG_IGN_24_OFF);
                testsFailed += _testActuator((false == buttonStates.ignitionButton), MSG_IGN_GND_OFF);
                testsFailed += _testActuator((false == buttonStates.ignitionButton), MSG_IGN_SW_OFF);
                testsFailed += _testActuator((false == (buttonStates.heatingBlanket1Switch + buttonStates.heatingBlanket2Switch)), MSG_HEAT_OFF);
                testsFailed += _testActuator((false == buttonStates.oxidizerValveButton), MSG_OX_OFF);

                if(0 == testsFailed) {
                    testState = HEAT_ON_BUTTON;
                    addMessage(MSG_HEAT_ON_START);
                }
            }
            break;

        case HEAT_ON_BUTTON:
            if(2 == (buttonStates.heatingBlanket1Switch + buttonStates.heatingBlanket2Switch)) {
                addMessage(MSG_HEAT_BUTTON);
                testStateChangeTime = millis();
                testState = HEAT_ON_TEST;
            }
            break;
        case HEAT_ON_TEST:
            //TODO: update once we have control sensing.
            if(millis() - testStateChangeTime > ACTUATOR_SETTLE_TIME) {
                int testsFailed = _testActuator((2 == (buttonStates.heatingBlanket1Switch + buttonStates.heatingBlanket2Switch)), MSG_HEAT_ON_RESULT);

                if(0 == testsFailed) {
                    addMessage(MSG_HEAT_RELEASE);
                    testState = HEAT_RELEASE;
                }
            }
            break;
        case HEAT_RELEASE:
            if(false == (buttonStates.heatingBlanket1Switch + buttonStates.heatingBlanket2Switch)) {
                addMessage(MSG_OX_ON_START);
                testState = VALVE_ON_BUTTON;
            }
            break;

        case VALVE_ON_BUTTON:
            if(true == buttonStates.oxidizerValveButton) {
                addMessage(MSG_OX_BUTTON);
                testStateChangeTime = millis();
                testState = VALVE_ON_TEST;
            }
            break;
        case VALVE_ON_TEST:
            // TODO: update once we have control sensing
            if(millis() - testStateChangeTime > ACTUATOR_SETTLE_TIME) {
                int testsFailed = _testActuator((true == buttonStates.oxidizerValveButton), MSG_OX_ON_RESULT);

                if(0 == testsFailed) {
                    addMessage(MSG_OX_RELEASE);
                    testState = VALVE_RELEASE;
                }
            }
            break;
        case VALVE_RELEASE:
            if(false == buttonStates.oxidizerValveButton) {
                addMessage(MSG_IGN_ON_START);
                testState = IGN_ON_BUTTON;
            }
            break;

        case IGN_ON_BUTTON:
            if(true == buttonStates.ignitionButton) {
                addMessage(MSG_IGN_BUTTON);
                testStateChangeTime = millis();
                testState = IGN_ON_TEST;
            }
            break;
        case IGN_ON_TEST:
            // TODO: update once we have control sensing
            if(millis() - testStateChangeTime > ACTUATOR_SETTLE_TIME) {
                int testsFailed = 0;
                testsFailed += _testActuator((true == buttonStates.ignitionButton), MSG_IGN_ON_24_RESULT);
                testsFailed += _testActuator((true == buttonStates.ignitionButton), MSG_IGN_ON_GND_RESULT);
                testsFailed += _testActuator((true == buttonStates.ignitionButton), MSG_IGN_ON_SW_RESULT);

                if(0 == testsFailed) {
                    addMessage(MSG_IGN_ON_RELEASE);
                    testState = IGN_RELEASE;
                }
            }
            break;
        case IGN_RELEASE:
            if(false == buttonStates.ignitionButton) {
                addMessage(MSG_TEST_PASSED);
                testState = TEST_END;
            }
            break;

        case TEST_END:
            addMessage(MSG_TEST_ENDING);
            return 0;

    }
    return 1;
}
