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
static const int16_t bottleValvePin = 32;
static const int16_t prechamberValvePin = 34; // assigned to unused pin for future-proofing

void initControlSensing(){
  pinMode(ventingPin, INPUT);
  pinMode(heatSensePin, INPUT);
  pinMode(igniSensePin, INPUT);
  pinMode(bottleValvePin, INPUT);
  pinMode(prechamberValvePin, INPUT);
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

bool readBottleValve(){
  return digitalRead(bottleValvePin);
}

bool readPrechamberValve(){
  return digitalRead(prechamberValvePin);
}