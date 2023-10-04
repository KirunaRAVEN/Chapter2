/* Filename:      Mode.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 *
 * Purpose:       Stores the current mode and substate variables in a portected
 *                object. These values can then be fetched from here by other objects.
 */

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>

#include "Globals.h"
#include "TestInOut.h"

static mode_t currentMode;
static substate_t currentSubstate;
static bool currentWarning;

static SemaphoreHandle_t modeMutex;
static SemaphoreHandle_t substateMutex;
static SemaphoreHandle_t warningMutex;

//In test bench version V1 the indicator LEDs use the servo connector
const int16_t forcedSequenceIndicatorPin = 7;
const int16_t testModeIndicatorPin = 6;


void setMode(mode_t newMode){
  if (xSemaphoreTake(modeMutex, 10) == pdTRUE){ 
    currentMode = newMode;
    xSemaphoreGive(modeMutex);
  }
}

void getMode(mode_t *mode){
  if (xSemaphoreTake(modeMutex, 10) == pdTRUE){ 
    *mode = currentMode;
    xSemaphoreGive(modeMutex);
  }
}

void setSubstate(substate_t newSubstate){
  if (xSemaphoreTake(substateMutex, 10) == pdTRUE){ 
    currentSubstate = newSubstate;
    xSemaphoreGive(substateMutex);
  }
}

void getSubstate(substate_t *substate){
  if (xSemaphoreTake(substateMutex, 10) == pdTRUE){ 
    *substate = currentSubstate;
    xSemaphoreGive(substateMutex);
  }
}

void setWarning(bool newWarning){
  if (xSemaphoreTake(warningMutex, 10) == pdTRUE){
    currentWarning = newWarning;
    xSemaphoreGive(warningMutex);
  }
}

void getWarning(bool *warning){
  if (xSemaphoreTake(warningMutex, 10) == pdTRUE){ 
    *warning = currentWarning;
    xSemaphoreGive(warningMutex);
  }
}

void setForcedIndicator(bool state){
  digitalWrite(forcedSequenceIndicatorPin, state);
}

void setTestModeIndicator(bool state){
  digitalWrite(testModeIndicatorPin, state);
}

void initMode(){
  currentMode = startMode;
  currentSubstate = startSubstate;

  modeMutex = xSemaphoreCreateMutex();
  substateMutex = xSemaphoreCreateMutex();
  warningMutex = xSemaphoreCreateMutex();

  pinMode(testModeIndicatorPin, OUTPUT);
  digitalWrite(testModeIndicatorPin, LOW);

  pinMode(forcedSequenceIndicatorPin, OUTPUT);
  digitalWrite(forcedSequenceIndicatorPin, LOW);

  //Check if system is started in TEST mode or normal WAIT mode
  testInput_t startTestInput;
  readTestInput(&startTestInput);

  if (startTestInput.startTest == HIGH){
    setMode(TEST);
    setTestModeIndicator(true);
  }
  else{
    setMode(WAIT);
  }
}
