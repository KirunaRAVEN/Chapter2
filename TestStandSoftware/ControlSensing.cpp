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

void initControlSensing(){
  pinMode(DUMP_VALVE_BUTTON_PIN, INPUT);
  pinMode(HEATING_SENSE_PIN, INPUT);
  pinMode(IGNITION_SENSE_PIN, INPUT);
  pinMode(FEEDING_VALVE_BUTTON_PIN, INPUT);
  pinMode(MAIN_VALVE_BUTTON_PIN, INPUT);
}

bool readVenting(){
  return digitalRead(DUMP_VALVE_BUTTON_PIN);
}

bool readIgnition(){
  return digitalRead(IGNITION_SENSE_PIN);
}

bool readHeating(){
  return digitalRead(HEATING_SENSE_PIN);
}

bool readBottleValve(){
  return digitalRead(FEEDING_VALVE_BUTTON_PIN);
}

bool readPrechamberValve(){
  return digitalRead(MAIN_VALVE_BUTTON_PIN);
}