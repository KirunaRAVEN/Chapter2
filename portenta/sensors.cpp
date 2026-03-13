#include "globals.h"

static const float pressureConversionFactor = MAX_PRESSURE / ((1 << SENSOR_RESOLUTION) - 1);
static const float tempConversionFactor = ((V_REF / ((1 << SENSOR_RESOLUTION) - 1))-0.25)*100; // clear this up

int readAllSensors() {
    /*
    sample all the sensors
    */

    g_packet.data.timestamp = millis();

    g_packet.data.N2OFeedingPressure1 = analogRead(OXIDIZER1_PRESSURE)*pressureConversionFactor;
    g_packet.data.N2OFeedingPressure2 = analogRead(OXIDIZER2_PRESSURE)*pressureConversionFactor;
    g_packet.data.linePressure = analogRead(LINE_PRESSURE)*pressureConversionFactor;
    g_packet.data.chamberPressure = analogRead(CHAMBER_PRESSURE)*pressureConversionFactor;
    g_packet.data.N2FeedingPressure = analogRead(NITROGEN_PRESSURE)*pressureConversionFactor;

    // gives the temp in celsius if i am able to read.
    g_packet.data.bottleTemperature1 = analogRead(OXIDIZER1_TEMP)*tempConversionFactor+25;
    g_packet.data.bottleTemperature2 = analogRead(OXIDIZER2_TEMP)*tempConversionFactor+25;
    g_packet.data.engineTemperature = analogRead(CHAMBER_TEMP);

    return 0;
}

float fastRead() {
    /*
    read only chamberPressure
    */
    return analogRead(CHAMBER_PRESSURE);
}
