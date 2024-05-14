/* Filename:      InfraRed.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.45 (11.05.2024)
 *
 * Purpose:       Responsible for the device interface of reading the 
 *                infrared sensor used to measure the plume temperature.
 */

#include <Arduino.h>
#include <stdint.h>
#include "Globals.h"

#include "InfraRed.h"


void initIR(){
  //Nothing to initialize currently
}

float readIR(){
  /* Measurement to value explanation:
   * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
   * sensorValue = measured voltage on the pin, within 0...1023
   * maxADC = 1023, since we are using 10-bit analog to digital converter
   * maxIR = maximum temperature in C measured by the IR sensor
   * minIR = minimum temperature in C measured by the IR sensor
   * calibratedValue * (maxIR - minIR) + minIR = mapping to the actual temp value
   * Measurement is repeated IrAverageCount times each loop to reduce noise
   */

  float sensorValueSum = 0;
  for (int i = 0; i < IrAverageCount; i++){
    sensorValueSum += analogRead(INFRARED_INPUT_PIN);
  }

  float temperature = calibrationADC * (sensorValueSum / (IrAverageCount * maxADC)) * (maxIR - minIR) + minIR;
  return temperature;
}
