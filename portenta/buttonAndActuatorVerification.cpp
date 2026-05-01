#include "buttonAndActuatorVerification.h"

static VerificationContext g_ctx;

static int testActuator(bool passed, uint8_t message) {
    addMessage(message);

    if(passed) {
        addMessage(MSG_PASS);
        return 0;
    }

    addMessage(MSG_FAIL);
    return 1;
}

// -------------------------------------------------------
// Button verification conditions
static bool noButtonPressed(const Buttons& b) {
    return b.allButtons == 0;
}
static bool heatOn(const Buttons& b) {
    return (b.heating1Switch + b.heating2Switch) == 2;
}
static bool heatOff(const Buttons& b) {
    return (b.heating1Switch + b.heating2Switch) == 0;
}
static bool oxidizerPressed(const Buttons& b) {
    return b.oxidizerButton == true;
}
static bool oxidizerReleased(const Buttons& b) {
    return b.oxidizerButton == false;
}
static bool ignitionPressed(const Buttons& b) {
    return b.ignitionButton == true;
}
static bool ignitionReleased(const Buttons& b) {
    return b.ignitionButton == false;
}
// -------------------------------------------------------


// -------------------------------------------------------
// Actuator verification functions
static int verifyAllOff(const Buttons& b) {
    int failed = 0;

    failed += testActuator(!b.ignitionButton, MSG_IGN_24_OFF);
    failed += testActuator(!b.ignitionButton, MSG_IGN_GND_OFF);
    failed += testActuator(!b.ignitionButton, MSG_IGN_SW_OFF);
    failed += testActuator(heatOff(b), MSG_HEAT_OFF);
    failed += testActuator(!b.oxidizerButton, MSG_OX_OFF);

    return failed;
}
static int verifyHeatOn(const Buttons& b) {
    return testActuator(heatOn(b), MSG_HEAT_ON_RESULT);
}
static int verifyOxidizerOn(const Buttons& b) {
    return testActuator(b.oxidizerButton, MSG_OX_ON_RESULT);
}

static int verifyIgnitionOn(const Buttons& b) {
    int failed = 0;

    failed += testActuator(b.ignitionButton, MSG_IGN_ON_24_RESULT);
    failed += testActuator(b.ignitionButton, MSG_IGN_ON_GND_RESULT);
    failed += testActuator(b.ignitionButton, MSG_IGN_ON_SW_RESULT);

    return failed;
}
// -------------------------------------------------------


// -------------------------------------------------------
// Sequence table
// Follows the following sequence:
//      1. Message showing what to do
//      2. Verify that the buttons have been pressed
//      3. Wait for the actuators to settle
//      4. Verify that the actuators did the thing
//      5. Message showing what to do
//      6. Verify that the buttons have been released
static const TestStep steps[] = {
        // 0 - ensure everything is off
        {
            MSG_NO_BUTTONS,
            noButtonPressed,
            ACTUATOR_SETTLE_TIME,
            verifyAllOff,
            0,
            nullptr
        },
        // 1 - Heat test
        {
            MSG_HEAT_ON_START,
            heatOn,
            ACTUATOR_SETTLE_TIME,
            verifyHeatOn,
            MSG_HEAT_RELEASE,
            heatOff
        },
        // 2 - Oxidizer test
        {
            MSG_OX_ON_START,
            oxidizerPressed,
            ACTUATOR_SETTLE_TIME,
            verifyOxidizerOn,
            MSG_OX_RELEASE,
            oxidizerReleased
        },
        // 3 - Ignition test
        {
            MSG_IGN_ON_START,
            ignitionPressed,
            ACTUATOR_SETTLE_TIME,
            verifyIgnitionOn,
            MSG_IGN_ON_RELEASE,
            ignitionReleased
        }
};

static size_t STEP_COUNT = sizeof(steps)/sizeof(steps[0]);
// -------------------------------------------------------

static void nextStep() {
    g_ctx.stepIndex++;

    if(g_ctx.stepIndex >= STEP_COUNT) {
        addMessage(MSG_TEST_PASSED);
        g_ctx.phase = VerificationContext::Phase::Complete;
    }
}

void resetVerification() {
    g_ctx = {};
}

// -------------------------------------------------------
// Main step verification code
int stepVerification() {
    Buttons buttons = g_controlBox.getMessage();

    static bool started = false;
    if(!started) {
        addMessage(MSG_TEST_SEQUENCE_START);
        started = true;
    }

    if(g_ctx.phase == VerificationContext::Phase::Complete) {
        addMessage(MSG_TEST_ENDING);
        return 0;
    }

    const TestStep& step = steps[g_ctx.stepIndex];

    switch (g_ctx.phase) {
        case VerificationContext::Phase::EnterStep:
            addMessage(step.startMessage);
            g_ctx.phase = VerificationContext::Phase::WaitForAction;
            break;

        case VerificationContext::Phase::WaitForAction:
            if(step.actionCondition(buttons)) {
                g_ctx.stateChangeTime = millis();
                g_ctx.phase = VerificationContext::Phase::Settling;
            }
            break;

        case VerificationContext::Phase::Settling:
            if(millis() - g_ctx.stateChangeTime >= step.settleMs) {
                g_ctx.phase = VerificationContext::Phase::Verify;
            }
            break;

        case VerificationContext::Phase::Verify:
            if(step.verifyFn(buttons) == 0) {
                if(step.releaseCondition != nullptr) {
                    addMessage(step.releaseMessage);
                    g_ctx.phase = VerificationContext::Phase::WaitForRealese;
                } else {
                    nextStep();
                }
            }
        case VerificationContext::Phase::WaitForRealese:
            if(step.releaseCondition(buttons)) {
                nextStep();
            }
        case VerificationContext::Phase::Complete:
        default:
            break;
    }
}
// -------------------------------------------------------