#ifndef PINMAP_H
#define PINMAP_H

#include <Arduino.h>

enum ControlBoxButton { // TODO
    IGNITION_KEY,
    OXIDIZER_VALVE_BUTTON,
    N2_FEEDING_BUTTON,
    DUMP_VALVE_BUTTON,
    HEATING_BLANKET_1_SWITCH,
    HEATING_BLANKET_2_SWITCH,
    EMERGENCY_BUTTON,
    BUTTON_COUNT
};

/** OUTPUT PINS **/
/* RELAYS */
#define    LIGHT_SIGNAL        22
#define    SIREN_SIGNAL        25
#define    HEATING2_RELAY      24
#define    HEATING1_RELAY      23
#define    OXIDIZER2_RELAY     26
#define    OXIDIZER1_RELAY     27
#define    NITROGEN_RELAY      28
#define    IGNITION_ARM        29
#define    IGNITION_RELAY      47

/* NORMAL OUTPUT PINS */
#define    TEST_LED_SIGNAL     35
#define    ERROR_LED_SIGNAL    33
#define    HIGH_SPEED_SIGNAL   32

/* OTHER GPIO */
#define    DUMP_PIN            31
#define    TEST_MODE_BUTTON    37
#define    LOADCELL_DATA       44
#define    LOADCELL_CLOCK      45

/** ANALOG INPUT PINS **/
#define    ADC_FUCKED          A0
#define    OXIDIZER1_PRESSURE  A1
#define    OXIDIZER2_TEMP      A2
#define    OXIDIZER2_PRESSURE  A3
#define    OXIDIZER1_TEMP      A4
#define    LINE_PRESSURE       A5
#define    CHAMBER_PRESSURE    A7
#define    NITROGEN_PRESSURE   A9
#define    PLUME_TEMP          A10
#define    PRECHAMBER_PRESSURE A11

#define RELAY_ON LOW
#define RELAY_OFF HIGH

#endif /* PINMAP_H */
