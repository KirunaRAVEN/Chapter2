/* Filename:      LoadCell.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.53 (12.06.2024)
 *
 * Purpose:       Responsible for reading the load cell used to measure the 
 *                thrust of the engine.
 */

#include <Arduino.h>
#include <stdint.h>

#include "LoadCell.h"
#include "Globals.h"

void initLoad(){
  //Nothing to initialize currently
}

int readLoad(){
  
  return analogRead(LOADCELL_INPUT_PIN);

  /*
  float sum = 0;
  float sensorValue = 0;
  */

  /* Measurement to value explanation:
   * For loop = measure multiple values and take average, load cells are noisy by default.
   * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
   * sensorValue = measured voltage on the pin, within 0...1023
   * maxADC = 1023, since we are using 10-bit analog to digital converter
   * refADV = expected ADC voltage of 5.00V
   * LoadCellLine_K = mapping from voltage to Newtons. Includes calibration values
   * LoadCellLine_B = mapping from voltage to Newtons. Includes calibration values
   * Newtons = K * Voltage + B
   */

  /*
  for(uint16_t i = 0; i<loadCellAverageCount; i++){
    sensorValue = analogRead(LOADCELL_INPUT_PIN);
    sensorValue = (sensorValue / maxADC) * refADC * calibrationADC;
    sum += sensorValue;  
  }
  sensorValue = sum/loadCellAverageCount;
  return loadCellLine_K * sensorValue + loadCellLine_B;
  */
}
