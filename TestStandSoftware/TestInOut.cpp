/* Filename:      TestInOut.cpp
 * Author:        Eemeli Mykrä
 * Date:          17.04.2023
 *
 * Purpose:       Responsible for setting up and reading the pins used to start
 *                the software in test mode and test the various actuators.
 */
 
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>
#include "TestInOut.h"
#include "Globals.h"


/*
TEST INPUT SCHEMATIC:
  PIN#    Var Name    Function                        Index
         ____________________________________________________________________
        |
  ______|
  | GND |  -          Analog ground                   -
  | 35  | resetSW     !Resets the software!           2 (pullups)
  | 33  | forced      !Forced sequence!               1 (pullups)
  | 31  | VALVE_IN    Valve test input                1 (inputs)      ______________
  | 29  | IGN_OUT     IGN_GND Relay test (OUTPUT)     0 (outputs)     |ARDUINO MEGA|
  | 27  | IGN_SW_IN   IGN_SW Relay test input         0 (inputs)      ¨¨¨¨¨¨¨¨¨¨¨¨¨¨
  | A15 | IGN_GND_IN  IGN_GND Relay test input        0 (analogs)
  | 23  | startTest   !Start in TEST mode!            0 (pullups)
  ------|
        |
        |____________________________________________________________________
*/

static const int16_t testInputPins[testInputCount] = {ACTUATOR_TEST_INPUT_PIN0, ACTUATOR_TEST_INPUT_PIN1};
static const int16_t testInputPullups[testPullupCount] = {ACTUATOR_TEST_PULLUP_PIN0, ACTUATOR_TEST_PULLUP_PIN1, ACTUATOR_TEST_PULLUP_PIN2};
static const int16_t testOutputPins[testOutputCount] = {ACTUATOR_TEST_OUTPUT_PIN};
static const int16_t testAnalogPins[testAnalogCount] = {ACTUATOR_TEST_ANALOG_PIN};

static SemaphoreHandle_t testPinInMutex;
static SemaphoreHandle_t testPinOutMutex;

void initTestInOut(){
  for (uint16_t i = 0; i<testPullupCount; i++){
    pinMode(testInputPullups[i], INPUT_PULLUP);
  }
  for (uint16_t i = 0; i<testInputCount; i++){
    pinMode(testInputPins[i], INPUT);
  }
  for (uint16_t i = 0; i<testOutputCount; i++){
    pinMode(testOutputPins[i], OUTPUT);
  } 

  testPinInMutex = xSemaphoreCreateMutex();
  testPinOutMutex = xSemaphoreCreateMutex();
}

void readTestInput(testInput_t* testInput){
  if (xSemaphoreTake(testPinInMutex, 10) == pdTRUE){
    //Pullup pins have inverted input, Button pressed -> LOW, Not pressed -> HIGH
    testInput->startTest = !digitalRead(testInputPullups[0]); //Inverted input
    testInput->forced = !digitalRead(testInputPullups[1]);    //Inverted input
    testInput->resetSW = !digitalRead(testInputPullups[2]);   //Inverted input

    testInput->IGN_GND_IN = analogRead(testAnalogPins[0]);
    
    testInput->MAIN_VALVE_IN = digitalRead(testInputPins[1]);
    testInput->IGN_SW_IN = digitalRead(testInputPins[0]);
  
    xSemaphoreGive(testPinInMutex);
  }
}

void activateOutputPin(uint16_t pinIndex, bool pinState){
  if (xSemaphoreTake(testPinOutMutex, 10) == pdTRUE){
    digitalWrite(testOutputPins[pinIndex], pinState);
    xSemaphoreGive(testPinOutMutex);
  }
}
