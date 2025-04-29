/* Filename:      Valves.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Responsible for controlling the different valves used to 
 *                control the flow of the oxidizer and purge gas.
 */
 
#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
#include <stdint.h>

#include "Globals.h"

#include <Servo.h>
// valvePins:
// Arduino Pin 2 -> ConnectorPin 1 -> mainValve -> code value 0
// Arduino Pin 3 -> ConnectorPin 2 -> dumpValve -> code value 1
// Arduino Pin 4 -> ConnectorPin 3 -> feedingValve -> code value 2

//static const uint16_t valvePins[valveCount] = {OXIDIZER_VALVE_PIN, DUMP_VALVE_PIN, N2FEEDING_VALVE_PIN};
//static SemaphoreHandle_t valveSemaphore;

void initValves(){
  //Set up pressure sensor
  //If this comment was me, I don't know what it means. -E.M.

  pinMode(pin_names_t::OXIDIZER_VALVE_PIN, OUTPUT);
  digitalWrite(pin_names_t::OXIDIZER_VALVE_PIN, LOW);
  
  //pinMode(pin_names_t::DUMP_VALVE_PIN, OUTPUT);
  DumpValve.attach(pin_names_t::DUMP_VALVE_PIN);
  //analogWrite(pin_names_t::DUMP_VALVE_PIN, OPENFORPWM); // Even though normally open, nominal state is dump valve open
  DumpValve.write(120);
  dumpValveState = LOW;

  
  pinMode(pin_names_t::N2FEEDING_VALVE_PIN, OUTPUT);
  digitalWrite(pin_names_t::N2FEEDING_VALVE_PIN, LOW);
  
  //valveSemaphore = xSemaphoreCreateMutex();
}

void setValve(pin_names_t valve_pin, bool state){
  if (valve_pin != pin_names_t::DUMP_VALVE_PIN){
    digitalWrite(valve_pin, state);
  }
  else{
    if (state){
      DumpValve.write(68);
    }
    else{
      DumpValve.write(120);
    }
    dumpValveState = state;
  }
}

void getValve(pin_names_t valve_pin, bool* valveState){
  if (valve_pin != pin_names_t::DUMP_VALVE_PIN){
    *valveState = digitalRead(valve_pin);
  }
  else{
    *valveState = dumpValveState;  // Return saved state
  }
}
