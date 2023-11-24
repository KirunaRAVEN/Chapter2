/* Filename:      InfraRed.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
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
  float sensorValue = analogRead(INFRARED_INPUT_PIN);
  float temperature = calibrationADC * (sensorValue / maxADC) * (maxIR - minIR) + minIR;
  return temperature;
}
