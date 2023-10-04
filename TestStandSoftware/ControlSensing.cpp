/* Filename:      ControlSensing.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 *
 * Purpose:       Responsible for reading the three external control signals
 *                passed to the Arduino Shield.
 */

 #include <Arduino.h>
 #include <stdint.h>

 #include "ControlSensing.h"

static const int16_t ventingPin = 10;
static const int16_t heatSensePin = 9;
static const int16_t igniSensePin = 8;

void initControlSensing(){
  pinMode(ventingPin, INPUT);
  pinMode(heatSensePin, INPUT);
  pinMode(igniSensePin, INPUT);
}

bool readVenting(){
  return digitalRead(ventingPin);
}

bool readIgnition(){
  return digitalRead(igniSensePin);
}

bool readHeating(){
  return digitalRead(heatSensePin);
}
