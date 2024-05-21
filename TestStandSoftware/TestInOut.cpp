/* Filename:      TestInOut.cpp
 * Author:        Eemeli Mykrä
 * Date:          17.04.2023
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       Responsible for setting up and reading the pins used to start
 *                the software in test mode and test the various actuators.
 */
 
#include <Arduino.h>

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
  | 33  | repeat      !Repeat sequence!               1 (pullups)
  | 31  | VALVE_IN    Valve test input                1 (inputs)      ______________
  | 29  | IGN_OUT     IGN_GND Relay test (OUTPUT)     0 (outputs)     |ARDUINO MEGA|
  | 27  | IGN_SW_IN   IGN_SW Relay test input         0 (inputs)      ¨¨¨¨¨¨¨¨¨¨¨¨¨¨
  | A15 | IGN_GND_IN  IGN_GND Relay test input        0 (analogs)
  | 23  | startTest   !Start in TEST mode!            0 (pullups)
  ------|
        |
        |____________________________________________________________________
*/

void initTestInOut(){
  pinMode(AUTO_TEST_START_PIN, INPUT_PULLUP);
  pinMode(REPEAT_SEQUENECE_PIN, INPUT_PULLUP);
  pinMode(SW_RESET_PIN, INPUT_PULLUP);

  pinMode(IGN_SW_RELAY_TEST_PIN, INPUT);
  pinMode(MAIN_VALVE_TEST_PIN, INPUT);

  pinMode(IGN_GND_RELAY_TEST_DRIVE_PIN, OUTPUT);
  
}

void readTestInput(testInput_t* testInput, bool readAll){

    testInput->MAIN_VALVE_IN = PINC & (1 << MAIN_VALVE_TEST_PIN_PORTC); //digitalRead(MAIN_VALVE_TEST_PIN);
    testInput->IGN_SW_IN = PINA & (1 << IGN_SW_RELAY_TEST_PIN_PORTA); //digitalRead(IGN_SW_RELAY_TEST_PIN);

  if (readAll == true){
    //Pullup pins have inverted input, Button pressed -> LOW, Not pressed -> HIGH
    testInput->resetSW = !(PINC & (1 << SW_RESET_PIN_PORTC)); //digitalRead(SW_RESET_PIN);            //Inverted input
    testInput->startTest = !(PINA & (1 << AUTO_TEST_START_PIN_PORTA)); //digitalRead(AUTO_TEST_START_PIN);   //Inverted input
    testInput->repeat = !(PINC & (1 << REPEAT_SEQUENECE_PIN_PORTC)); //digitalRead(REPEAT_SEQUENECE_PIN);     //Inverted input

    //Analog to digital calibration is not included here due to the 
    //error being way less than the margins for this specific value
    testInput->IGN_GND_IN = analogRead(IGN_GND_RELAY_TEST_MEASURE_PIN);
  } 
}

//Allowing any pin to be changed by this instead of pins in a list is slightly sus
//Either keep this and trust the process, go back to a list of pins for this or include a check.
void activateOutputPin(uint16_t pinNumber, bool pinState){
    digitalWrite(pinNumber, pinState);
}
