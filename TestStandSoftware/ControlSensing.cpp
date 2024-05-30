/* Filename:      ControlSensing.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.52 (28.05.2024)
 *
 * Purpose:       Responsible for reading the five external control signals
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
  //return digitalRead(DUMP_VALVE_BUTTON_PIN);
  return PINB & (1 << DUMP_VALVE_BUTTON_PIN_PORTB);
}

bool readIgnitionButton(){
  //return digitalRead(IGNITION_SENSE_PIN);
  return PINH & (1 << IGNITION_SENSE_PIN_PORTH);
}

bool readHeatingButton(){
  //return digitalRead(HEATING_SENSE_PIN);
  return PINH & (1 << HEATING_SENSE_PIN_PORTH);
}

bool readN2FeedingValveButton(){
  //return digitalRead(FEEDING_VALVE_BUTTON_PIN);
  return PIND & (1 << FEEDING_VALVE_BUTTON_PIN_PORTD);
}

bool readOxidizerValveButton(){
  //return digitalRead(MAIN_VALVE_BUTTON_PIN);
  return PIND & (1 << MAIN_VALVE_BUTTON_PIN_PORTD);
}