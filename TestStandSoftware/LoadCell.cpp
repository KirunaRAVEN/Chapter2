/* Filename:      LoadCell.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
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

float readLoad(){
  float sum = 0;
  float sensorValue = 0;

  for(uint16_t i = 0; i<loadCellAverageCount; i++){
    sensorValue = analogRead(LOADCELL_INPUT_PIN);
    sensorValue = (sensorValue / maxADC) * refADC * calibrationADC;
    sum += sensorValue;  
  }

  sensorValue = sum/loadCellAverageCount;
  return loadCellLine_K * sensorValue + loadCellLine_B;

}
