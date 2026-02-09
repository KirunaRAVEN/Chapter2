#include "globals.h"

int readAllSensors() {
    /*
    sample all the sensors
    */

    g_packet.data.timestamp = millis();

    g_packet.data.N2OFeedingPressure1 = Breakout.analogRead(g_analogPins[OXIDIZER1_PRESSURE]);
    g_packet.data.N2OFeedingPressure2 = Breakout.analogRead(g_analogPins[OXIDIZER2_PRESSURE]);
    g_packet.data.linePressure = Breakout.analogRead(g_analogPins[LINE_PRESSURE]);
    g_packet.data.chamberPressure = Breakout.analogRead(g_analogPins[CHAMBER_PRESSURE]);
    g_packet.data.N2FeedingPressure = Breakout.analogRead(g_analogPins[NITROGEN_PRESSURE]);

    g_packet.data.bottleTemperature1 = Breakout.analogRead(g_analogPins[OXIDIZER1_TEMP]);
    g_packet.data.bottleTemperature2 = Breakout.analogRead(g_analogPins[OXIDIZER2_TEMP]);
    g_packet.data.engineTemperature = Breakout.analogRead(g_analogPins[CHAMBER_TEMP]);

    return 0;
}

float fastRead() {
    /*
    read only chamberPressure
    */
    return Breakout.analogRead(g_analogPins[CHAMBER_PRESSURE]);
}
