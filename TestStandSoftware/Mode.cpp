/* Filename:      Mode.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.5 (16.05.2024)
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

//static SemaphoreHandle_t modeMutex;
//static SemaphoreHandle_t substateMutex;
//static SemaphoreHandle_t warningMutex;

//initMode() -function found at the end of the file

void setMode(mode_t newMode){
  //if (xSemaphoreTake(modeMutex, 10) == pdTRUE){ 
    currentMode = newMode;
  //  xSemaphoreGive(modeMutex);
  //}
}

void getMode(mode_t *mode){
  //if (xSemaphoreTake(modeMutex, 10) == pdTRUE){ 
    *mode = currentMode;
  //  xSemaphoreGive(modeMutex);
  //}
}

void setSubstate(substate_t newSubstate){
  //if (xSemaphoreTake(substateMutex, 10) == pdTRUE){ 
    currentSubstate = newSubstate;
  //  xSemaphoreGive(substateMutex);
  //}
}

void getSubstate(substate_t *substate){
  //if (xSemaphoreTake(substateMutex, 10) == pdTRUE){ 
    *substate = currentSubstate;
  //  xSemaphoreGive(substateMutex);
  //}
}

void setWarning(bool newWarning){
  //if (xSemaphoreTake(warningMutex, 10) == pdTRUE){
    currentWarning = newWarning;
  //  xSemaphoreGive(warningMutex);
  //}
}

void getWarning(bool *warning){
  //if (xSemaphoreTake(warningMutex, 10) == pdTRUE){ 
    *warning = currentWarning;
  //  xSemaphoreGive(warningMutex);
  //}
}

void setForcedIndicator(bool state){
  digitalWrite(FORCED_SEQUENCE_LED_PIN, state);
}

void setTestModeIndicator(bool state){
  digitalWrite(TEST_MODE_LED_PIN, state);
}


//Init function at the bottom due to the need to call functions defined before in this file.
void initMode(){
  currentMode = startMode;
  currentSubstate = startSubstate;

  //modeMutex = xSemaphoreCreateMutex();
  //substateMutex = xSemaphoreCreateMutex();
  //warningMutex = xSemaphoreCreateMutex();

  pinMode(TEST_MODE_LED_PIN, OUTPUT);
  digitalWrite(TEST_MODE_LED_PIN, LOW);

  pinMode(FORCED_SEQUENCE_LED_PIN, OUTPUT);
  digitalWrite(FORCED_SEQUENCE_LED_PIN, LOW);

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
