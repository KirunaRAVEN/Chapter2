#include "globals.h"

static const float pressureConversionFactor = MAX_PRESSURE / ((1 << SENSOR_RESOLUTION) - 1);
static const float tempConversionFactor = ((V_REF / ((1 << SENSOR_RESOLUTION) - 1))-0.25)*100; // clear this up

int readAllSensors() {
    /*
    sample all the sensors
    */

    g_packet.data.timestamp = millis();

    g_packet.data.N2OFeedingPressure1 = Breakout.analogRead(g_analogPins[OXIDIZER1_PRESSURE])*pressureConversionFactor;
    g_packet.data.N2OFeedingPressure2 = Breakout.analogRead(g_analogPins[OXIDIZER2_PRESSURE])*pressureConversionFactor;
    g_packet.data.linePressure = Breakout.analogRead(g_analogPins[LINE_PRESSURE])*pressureConversionFactor;
    g_packet.data.chamberPressure = Breakout.analogRead(g_analogPins[CHAMBER_PRESSURE])*pressureConversionFactor;
    g_packet.data.N2FeedingPressure = Breakout.analogRead(g_analogPins[NITROGEN_PRESSURE])*pressureConversionFactor;

    // gives the temp in celsius if i am able to read.
    g_packet.data.bottleTemperature1 = Breakout.analogRead(g_analogPins[OXIDIZER1_TEMP])*tempConversionFactor+25;
    g_packet.data.bottleTemperature2 = Breakout.analogRead(g_analogPins[OXIDIZER2_TEMP])*tempConversionFactor+25;
    g_packet.data.engineTemperature = Breakout.analogRead(g_analogPins[CHAMBER_TEMP]);

    return 0;
}

float fastRead() {
    /*
    read only chamberPressure
    */
    return Breakout.analogRead(g_analogPins[CHAMBER_PRESSURE]);
}
