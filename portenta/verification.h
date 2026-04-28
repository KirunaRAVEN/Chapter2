/*
Verification that everything works as it should.
*/

#ifndef VERIFICATION_H
#define VERIFICATION_H
#include "globals.h"

#define ACTUATOR_SETTLE_TIME 250
#define VERIFICATION_END_COUNT 10

struct VerificationContext {
    enum class Phase {
        EnterStep,
        WaitForAction,
        Settling,
        Verify,
        WaitForRealese,
        Complete
    };

    int stepIndex = 0;
    int stateChangeTime = 0;
    Phase phase = Phase::EnterStep;
};

static int testActuator(bool passed, uint8_t message);

using Buttons = union ControlBoxStateMessage;

// Button conditions
static bool noButtonPressed(const Buttons& b);
static bool heatOn(const Buttons& b);
static bool heatOff(const Buttons& b);
static bool oxidizerPressed(const Buttons& b);
static bool oxidizerReleased(const Buttons& b);
static bool ignitionPressed(const Buttons& b);
static bool ignitionReleased(const Buttons& b);

// Actuator verification functions
static int VerifyAllOff(const Buttons& b);
static int verifyHeatOn(const Buttons& b);
static int VerifyOxidizerOn(const Buttons& b);
static int VerifyIgnitionOn(const Buttons& b);

// Step definition
struct TestStep {
    uint8_t startMessage;
    bool (*actionCondition)(const Buttons&);
    uint32_t settleMs;
    int (*verifyFn)(const Buttons&);
    uint8_t releaseMessage;
    bool (*releaseCondition)(const Buttons&);
};

static void nextStep();

void resetVerification();

int stepVerification();

#endif /* VERIFICATION_H */
