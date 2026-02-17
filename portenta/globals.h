#ifndef GLOBALS_H
#define GLOBALS_H
#include "pinMap.h"
#include "comms.h"
#include "controlBoxRX.h"
#include "sensors.h"
#include "verification.h"
#include "sequence.h"
#include <Servo.h>

//#define DEBUG

/* Timing */
#define MAIN_LOOP_PERIOD 100 //ms, 10 Hz
#define FAST_LOOP_PERIOD 333 //us, 3 kHz

/* Ignition sequence (times in ms) */
#define IGNITION_SAFE_TIME  (1 * 1000)
#define BURN_TIME           (4 * 1000)
#define IGNITER_BURN_LENGTH (900)
#define IGNITER_DELAY       (50)
#define VALVE_ON_TIME       (100)
#define IGNITION_OFF_TIME   (IGNITER_BURN_LENGTH)
#define VALVE_OFF_TIME      (IGNITION_OFF_TIME + BURN_TIME)
#define OXIDIZER_EMPTY_TIME (VALVE_OFF_TIME + 500)
#define CAMERA_TRIGGER_TIME (VALVE_OFF_TIME + 2000)
#define PURGING_TIME        (OXIDIZER_EMPTY_TIME + 4*1000)

/* angles for dump valve */
#define DUMP_CLOSE 0
#define DUMP_OPEN 180

/* Main storage struct. 36 bytes */
struct dataPoint {
    long int timestamp;

    // Pressures
    float N2OFeedingPressure1;      //Oxidizer feeding pressure
    float N2OFeedingPressure2;      //Oxidizer feeding pressure
    float linePressure;             //Line pressure
    float chamberPressure;          //Combustion chamber pressure
    float N2FeedingPressure;        //Nitrogen feeding pressure

    // Temperatures
    float bottleTemperature1;       //Bottle temperature
    float bottleTemperature2;       //Nozzle temperature
    float engineTemperature;        //Piping temperature
};

/* Internal software storage struct. 4 bytes due to padding
Will always get sent with the datapoint, so no need for a timestamp */
struct softwareState {
    // Software modes
    uint8_t mode;                       //What mode is the software in
    uint8_t subState;                   //What substate is the software in
    uint8_t message;

    // Button states
    bool dumpValveButton : 1;       //Is dump valve button pressed (normally open)
    bool heatingBlanketButton1 : 1; //Is heating button 1 pressed
    bool heatingBlanketButton2 : 1; //Is heating button 2 pressed
    bool ignitionButton : 1;        //Is ignition button pressed
    bool N2ValveButton : 1;         //Is N2 feeding valve button pressed (normally closed)
    bool N2OValveButton : 1;        //Is the oxidizer valve button pressed (normally closed)

    // Software control states
    bool valveActive : 1;           //Is the valve opened by the software
    bool ignitionEngagedActive : 1; //Is the ignition activated by the software

};

/* Normal packet. 48 bytes */
struct normalPacket {
    struct dataPoint data;
    struct softwareState state;
};

enum messageIndices {
    MSG_TEST_SEQUENCE_START = 1,
    MSG_NO_BUTTONS = 2,
    MSG_RELEASE_OX = 3,
    MSG_RELEASE_IGN = 4,
    MSG_RELEASE_HEAT = 5,
    MSG_PASS = 6,
    MSG_FAIL = 7,
    MSG_IGN_24_OFF = 8,
    MSG_IGN_GND_OFF = 9,
    MSG_IGN_SW_OFF = 10,
    MSG_HEAT_OFF = 11,
    MSG_OX_OFF = 12,
    MSG_HEAT_ON_START = 13,
    MSG_HEAT_BUTTON = 14,
    MSG_HEAT_ON_RESULT = 15,
    MSG_HEAT_RELEASE = 16,
    MSG_OX_ON_START = 17,
    MSG_OX_BUTTON = 18,
    MSG_OX_ON_RESULT = 19,
    MSG_OX_RELEASE = 20,
    MSG_IGN_ON_START = 21,
    MSG_IGN_BUTTON = 22,
    MSG_IGN_ON_24_RESULT = 23,
    MSG_IGN_ON_GND_RESULT = 24,
    MSG_IGN_ON_SW_RESULT = 25,
    MSG_IGN_ON_RELEASE = 26,
    MSG_TEST_FINISH = 27,
    MSG_TEST_PASSED = 28,
    MSG_TEST_FAILED = 29,
    MSG_TEST_ENDING = 30,
    MSG_DUMP_WARNING = 31,
    MSG_N2_FEED_WARNING = 32,
    MSG_OX_FEED_WARNING = 33
};


/* Enumeration for the different modes of the system */
enum modes {
  INIT,
  TEST,
  WAIT,
  SEQUENCE,
  SAFE,
  SHUTDOWN
};

/*Enumeration for the different substates of the firing sequence */
enum subStates {
  ALL_OFF,
  IGNIT_ON,
  VALVE_ON,
  IGNIT_OFF,
  VALVE_OFF,
  PURGING,
  FINISHED
};

//Used mainly for debugging and for user information
const char substateStrings[7][10] = {"ALL_OFF", "IGNIT_ON", "VALVE_ON", "IGNIT_OFF", "VALVE_OFF", "PURGING", "FINISHED"};
const char modeStrings[6][10] = {"INIT", "TEST", "WAIT", "SEQUENCE", "SAFE", "SHUTDOWN"};

/* global variables */
extern struct normalPacket g_packet;
extern ControlBoxRX g_controlBox;
extern breakoutPin g_outPins[];
extern breakoutPin g_inPins[];
extern breakoutPin g_analogPins[];
#endif /* GLOBALS_H */
