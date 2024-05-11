/* Filename:      TestInOut.cpp
 * Author:        Eemeli Mykrä
 * Date:          17.04.2023
 * Version:       V1.45 (11.05.2024)
 *
 * Purpose:       Responsible for setting up and reading the pins used to start
 *                the software in test mode and test the various actuators.
 */
 
#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
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

//Trying out a more sensible way of writing the code and not using pin number lists of miniscule length.
//static const int16_t testInputPins[testInputCount] = {IGN_SW_RELAY_TEST_PIN, MAIN_VALVE_TEST_PIN};
//static const int16_t testInputPullups[testPullupCount] = {AUTO_TEST_START_PIN, FORCED_SEQUENECE_PIN, SW_RESET_PIN};
//static const int16_t testOutputPins[testOutputCount] = {IGN_GND_RELAY_TEST_DRIVE_PIN};
//static const int16_t testAnalogPins[testAnalogCount] = {IGN_GND_RELAY_TEST_MEASURE_PIN};

//static SemaphoreHandle_t testPinInMutex;
//static SemaphoreHandle_t testPinOutMutex;

void initTestInOut(){
  pinMode(AUTO_TEST_START_PIN, INPUT_PULLUP);
  pinMode(FORCED_SEQUENECE_PIN, INPUT_PULLUP);
  pinMode(SW_RESET_PIN, INPUT_PULLUP);

  pinMode(IGN_SW_RELAY_TEST_PIN, INPUT);
  pinMode(MAIN_VALVE_TEST_PIN, INPUT);

  pinMode(IGN_GND_RELAY_TEST_DRIVE_PIN, OUTPUT);
  
  //testPinInMutex = xSemaphoreCreateMutex();
  //testPinOutMutex = xSemaphoreCreateMutex();
}

void readTestInput(testInput_t* testInput){
  //f (xSemaphoreTake(testPinInMutex, 10) == pdTRUE){
    //Pullup pins have inverted input, Button pressed -> LOW, Not pressed -> HIGH
    testInput->startTest = !digitalRead(AUTO_TEST_START_PIN);   //Inverted input
    testInput->forced = !digitalRead(FORCED_SEQUENECE_PIN);     //Inverted input
    testInput->resetSW = !digitalRead(SW_RESET_PIN);            //Inverted input

    //Analog to digital calibration is not included here due to the 
    //error being way less than the margins for this specific value
    testInput->IGN_GND_IN = analogRead(IGN_GND_RELAY_TEST_MEASURE_PIN);
    
    testInput->MAIN_VALVE_IN = digitalRead(MAIN_VALVE_TEST_PIN);
    testInput->IGN_SW_IN = digitalRead(IGN_SW_RELAY_TEST_PIN);
  
  //  xSemaphoreGive(testPinInMutex);
  //}
}

//Allowing any pin to be changed by this instead of pins in a list is slightly sus
//Either keep this and trust the process, go back to a list of pins for this or include a check.
void activateOutputPin(uint16_t pinNumber, bool pinState){
  //if (xSemaphoreTake(testPinOutMutex, 10) == pdTRUE){
    digitalWrite(pinNumber, pinState);
  //  xSemaphoreGive(testPinOutMutex);
  //}
}
