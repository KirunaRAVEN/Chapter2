/* Filename:      Mode.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.51 (21.05.2024)
 *
 * Purpose:       Stores the current mode and substate variables in a portected
 *                object. These values can then be fetched from here by other objects.
 */

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
#include <stdint.h>

#include "Globals.h"
#include "TestInOut.h"

static mode_t currentMode;
static substate_t currentSubstate;
static bool currentWarning;

void setMode(mode_t newMode){
    currentMode = newMode;
}

void getMode(mode_t *mode){
    *mode = currentMode;
}

void setSubstate(substate_t newSubstate){
    currentSubstate = newSubstate;
}

void getSubstate(substate_t *substate){
    *substate = currentSubstate;
}

void setWarning(bool newWarning){
    currentWarning = newWarning;
}

void getWarning(bool *warning){
    *warning = currentWarning;
}

void setRepeatIndicator(bool state){
  //digitalWrite(REPEAT_SEQUENCE_LED_PIN, state);
  if (state == true){PORTH |=  (1 << REPEAT_SEQUENCE_LED_PIN_PORTH);}
  else              {PORTH &= ~(1 << REPEAT_SEQUENCE_LED_PIN_PORTH);}
}

void setTestModeIndicator(bool state){
  //digitalWrite(TEST_MODE_LED_PIN, state);
  if (state == true){PORTH |=  (1 << TEST_MODE_LED_PIN_PORTH);}
  else              {PORTH &= ~(1 << TEST_MODE_LED_PIN_PORTH);}
}


//Init function at the bottom due to the need to call functions defined before in this file.
void initMode(){
  currentMode = startMode;
  currentSubstate = startSubstate;

  pinMode(TEST_MODE_LED_PIN, OUTPUT);
  digitalWrite(TEST_MODE_LED_PIN, LOW);

  pinMode(REPEAT_SEQUENCE_LED_PIN, OUTPUT);
  digitalWrite(REPEAT_SEQUENCE_LED_PIN, LOW);

  //Check if system is started in TEST mode or normal WAIT mode
  testInput_t startTestInput;
  //Read all of the inputs
  readTestInput(&startTestInput, true);

  if (startTestInput.startTest == HIGH){
    setMode(TEST);
    setTestModeIndicator(true);
  }
  else{
    setMode(WAIT);
  }
}
