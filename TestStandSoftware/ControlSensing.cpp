/* Filename:      ControlSensing.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.3 (10.03.2024)
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

bool readDumpValveButton(){
  return digitalRead(DUMP_VALVE_BUTTON_PIN);
}

bool readIgnitionButton(){
  return digitalRead(IGNITION_SENSE_PIN);
}

bool readHeatingButton(){
  return digitalRead(HEATING_SENSE_PIN);
}

bool readN2FeedingValveButton(){
  return digitalRead(FEEDING_VALVE_BUTTON_PIN);
}

bool readOxidizerValveButton(){
  return digitalRead(MAIN_VALVE_BUTTON_PIN);
}