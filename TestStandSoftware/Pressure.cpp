/* Filename:      Pressure.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Responsible for the device interface of reading the 
 *                pressure sensors used to measure the various pressures.
 */

#include <Arduino.h>
#include <stdint.h>

#include "Globals.h"

//This pin list is kept to access both pin number and calibration list data with the same index given to readPressure5V()
static const int16_t pressurePins[pressureCount5V + pressureCount20mA] = {PRESSURE_INPUT_PIN0, PRESSURE_INPUT_PIN1, PRESSURE_INPUT_PIN2, PRESSURE_INPUT_PIN3, PRESSURE_INPUT_PIN4};

void initPressure(){
  //Nothing to initialize currently
}

int readPressure5V(uint16_t sensorNum){

  return analogRead(pressurePins[sensorNum]);
  
  /* Measurement to value explanation:
   * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
   * refADV = expected ADC voltage of 5.00V
   * pressureVoltage = measured voltage on the pin, within 0...1023
   * maxADC = 1023, since we are using 10-bit analog to digital converter
   * pressureCalibration_K[sensorNum] = mapping from voltage to bar for this sensor. Includes calibration values
   * pressureCalibration_B[sensorNum] = mapping from voltage to bar for this sensor. Includes calibration values
   * Bars = K * Voltage + B
   */
  
  /*
  float pressureVoltage = analogRead(pressurePins[sensorNum]);
  pressureVoltage = calibrationADC * refADC * (pressureVoltage / maxADC);
  return pressureCalibration_K[sensorNum] * pressureVoltage + pressureCalibration_B[sensorNum];
  */
}


//UNUSED
//float readPressure20mA(uint16_t sensorNum){

  /* Measurement to value explanation:
   * Multiple readings may be taken and averaged out
   * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
   * Conversion to mV with 1000 multiplier
   * refADV = expected ADC voltage of 5.00V
   * pressureVoltage = measured voltage on the pin, within 0...1023
   * maxADC = 1023, since we are using 10-bit analog to digital converter
   * maxcombustionPressure0mA = Maximum pressure the sensor measures
   * pressureResistance = How many ohms does the sensor output current go through
   * minPressureCurrent = Minimum output current from the sensor
   * maxPressureCurrent = Maximum output current from the sensor
   * Reading is calibrated linearly with: Bars = K * pressureReading + B
   */
  /*
  int32_t sum = 0;
  for(uint16_t i = 0; i < pressureAverageCount20mA; i++){
    sum += analogRead(pressurePins[sensorNum]);
  }
  float pressureVoltage = sum/(float) pressureAverageCount20mA;  //Change to float to get a floating point number output
  //return pressureVoltage;
  pressureVoltage = calibrationADC * 1000 * refADC * (pressureVoltage / maxADC); //Conversion to mV with 1000 multiplier
  float pressureReading = maxcombustionPressure0mA * (pressureVoltage / pressureResistance - minPressureCurrent) / (maxPressureCurrent - minPressureCurrent);

  return pressureReading * pressureLine_K20mA + pressureLine_B20mA;
}
*/