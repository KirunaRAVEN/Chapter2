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

static const int16_t pressurePins5V[pressureCount5V] = {A0};
static const int16_t pressurePins20mA[pressureCount20mA] = {A1, A2};

void initPressure(){
  //Nothing to initialize currently
}

float readPressure5V(uint16_t sensorNum){
  float pressureVoltage = analogRead(pressurePins5V[sensorNum]);
  pressureVoltage = calibrationADC * refADC * (pressureVoltage / maxADC);
  return pressureCalibration_K[sensorNum] * pressureVoltage + pressureCalibration_B[sensorNum];
}

float readPressure20mA(uint16_t sensorNum){
  int32_t sum = 0;
  for(uint16_t i = 0; i < 1; i++){
    sum += analogRead(pressurePins20mA[sensorNum]);
  }
  float pressureVoltage = sum/1.0;
  //return pressureVoltage;
  pressureVoltage = calibrationADC * 1000 * refADC * (pressureVoltage / maxADC); //Conversion to mV with 1000 multiplier
  float pressureReading = maxPressure20mA * (pressureVoltage / pressureResistance - minPressureCurrent) / (maxPressureCurrent - minPressureCurrent);

  return pressureReading * pressureLine_K20mA + pressureLine_B20mA;
}
