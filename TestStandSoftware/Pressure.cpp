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
  /* Measurement to value explanation:
   * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
   * refADV = expected ADC voltage of 5.00V
   * pressureVoltage = measured voltage on the pin, within 0...1023
   * maxADC = 1023, since we are using 10-bit analog to digital converter
   * pressureCalibration_K[sensorNum] = mapping from voltage to bar for this sensor. Includes calibration values
   * pressureCalibration_B[sensorNum] = mapping from voltage to bar for this sensor. Includes calibration values
   * Bars = K * Voltage + B
   */
float readPressure5V(uint16_t sensorNum){
  float pressureVoltage = analogRead(pressurePins5V[sensorNum]);
  pressureVoltage = calibrationADC * refADC * (pressureVoltage / maxADC);
  return pressureCalibration_K[sensorNum] * pressureVoltage + pressureCalibration_B[sensorNum];
}

