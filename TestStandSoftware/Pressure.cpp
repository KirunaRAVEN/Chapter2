/* Filename:      Pressure.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 *
 * Purpose:       Responsible for the device interface of reading the 
 *                pressure sensors used to measure the various pressures.
 */

#include <Arduino.h>
#include <stdint.h>

#include "Globals.h"

//This pin list is kept to access both pin number and calibration list data with the same index given to readPressure5V()
static const int16_t pressurePins5V[pressureCount5V] = {PRESSURE_INPUT_PIN0, PRESSURE_INPUT_PIN1, PRESSURE_INPUT_PIN2};

void initPressure(){
  //Nothing to initialize currently
}

float readPressure5V(uint16_t sensorNum){
  float pressureVoltage = analogRead(pressurePins5V[sensorNum]);
  pressureVoltage = calibrationADC * refADC * (pressureVoltage / maxADC);
  return pressureCalibration_K[sensorNum] * pressureVoltage + pressureCalibration_B[sensorNum];
}

