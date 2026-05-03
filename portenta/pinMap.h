#ifndef PINMAP_H
#define PINMAP_H

#include <Arduino.h>

/** OUTPUT PINS **/
/* RELAYS */
#define    PIN_LIGHT_SIGNAL        22
#define    PIN_SIREN_SIGNAL        25
#define    PIN_HEATING2_RELAY      24
#define    PIN_HEATING1_RELAY      23
#define    PIN_OXIDIZER2_RELAY     26
#define    PIN_OXIDIZER1_RELAY     27
#define    PIN_NITROGEN_RELAY      28
#define    PIN_IGNITION_ARM        29
#define    PIN_IGNITION_RELAY      47
#define    PIN_DUMP_RELAY          11

/* NORMAL OUTPUT PINS */
#define    PIN_TEST_LED_SIGNAL     35
#define    PIN_ERROR_LED_SIGNAL    33
#define    PIN_HIGH_SPEED_SIGNAL   32

/* OTHER GPIO */
#define    PIN_DUMP_PIN            11
#define    PIN_TEST_MODE_BUTTON    37
#define    PIN_LOADCELL_DATA       44
#define    PIN_LOADCELL_CLOCK      45

// #define    PIN_SPI_MISO            50
// #define    PIN_SPI_CLK             52
#define    PIN_SPI_CS_TC_CHAMBER   46
#define    PIN_SPI_CS_TC_PIPING    48

/** ANALOG INPUT PINS **/
#define    PIN_ADC_FUCKED          A0
#define    PIN_OXIDIZER1_PRESSURE  A1
#define    PIN_OXIDIZER2_TEMP      A2
#define    PIN_OXIDIZER2_PRESSURE  A3
#define    PIN_OXIDIZER1_TEMP      A4
#define    PIN_LINE_PRESSURE       A5
#define    PIN_CHAMBER_PRESSURE    A7
#define    PIN_NITROGEN_PRESSURE   A9
#define    PIN_PLUME_TEMP          A10
#define    PIN_PRECHAMBER_PRESSURE A11

#endif /* PINMAP_H */
