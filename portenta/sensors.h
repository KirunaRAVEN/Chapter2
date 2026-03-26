/*
Sensors library
*/
#ifndef SENSORS_H
#define SENSORS_H

#include "globals.h"

#define SENSOR_RESOLUTION 10 // ADC bits
#define V_REF 5.0 // voltage reference
#define MAX_PRESSURE 100.0 //the maximum measurable pressure

void initLoadcell();

int readAllSensors();

float fastRead();

#endif /* SENSORS_H */
