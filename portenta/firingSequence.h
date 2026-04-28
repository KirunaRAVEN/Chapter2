#ifndef SEQUENCE_H
#define SEQUENCE_H
#include "globals.h"

struct SequenceContext
{
    uint8_t stepIndex = 0;
    uint32_t startTime = 0;
    bool running = false;
};

struct TimedStep
{
    uint32_t triggerTimeMs;
    void (*action)();
    uint8_t nextState;
};

static inline uint32_t elapsedTime();

static void ignitionOn();
static void valveOn();
static void ignitionOff();
static void valveOff();
static void purgeOn();
static void cameraTrigger();
static void purgeOff();

void resetSequence();
static void startSequence();


int stepSequence();

#endif /* SEQUENCE_H */
