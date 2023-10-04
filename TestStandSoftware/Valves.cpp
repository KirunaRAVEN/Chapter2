/* Filename:      Valves.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 *
 * Purpose:       Responsible for controlling the different valves used to 
 *                control the flow of the oxidizer.
 */
 
#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>
#include <stdint.h>

#include "Globals.h"

static const uint16_t valvePins[valveCount] = {2, 3, 4};
static SemaphoreHandle_t valveSemaphore;

void initValves(){
  //Set up pressure sensor
  for (uint16_t i = 0; i<valveCount; i++){
    pinMode(valvePins[i], OUTPUT);
    digitalWrite(valvePins[i], LOW);
  }
  valveSemaphore = xSemaphoreCreateMutex();
  
}

void setValve(uint16_t valveNum, bool state){
  if (xSemaphoreTake(valveSemaphore, 10) == pdTRUE){
    digitalWrite(valvePins[valveNum], state);
    xSemaphoreGive(valveSemaphore);
  }
}

void getValve(uint16_t valveNum, bool* valveState){
  if (xSemaphoreTake(valveSemaphore, 10) == pdTRUE){
    *valveState = digitalRead(valvePins[valveNum]); 
    xSemaphoreGive(valveSemaphore);
  }
}
