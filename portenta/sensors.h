/*
Sensors library
*/
#ifndef SENSORS_H
#define SENSORS_H

#define SENSOR_RESOLUTION 16 // ADC bits
#define V_REF 5.0 // voltage reference
#define MAX_PRESSURE 100.0 //the maximum measurable pressure

int readAllSensors();

float fastRead();

#endif /* SENSORS_H */
