/*
Verification that everything works as it should.
*/

#ifndef VERIFICATION_H
#define VERIFICATION_H
#include "globals.h"

#define ACTUATOR_SETTLE_TIME 250
#define VERIFICATION_END_COUNT 10


enum verificationState {
    TEST_START,
    OFF_STATE_BUTTON,
    OFF_STATE_TEST,
    HEAT_ON_BUTTON,
    HEAT_ON_TEST,
    HEAT_RELEASE,
    VALVE_ON_BUTTON,
    VALVE_ON_TEST,
    VALVE_RELEASE,
    IGN_ON_BUTTON,
    IGN_ON_TEST,
    IGN_RELEASE,
    TEST_END
};

int stepVerification();

#endif /* VERIFICATION_H */
