/* Filename:      Ignition.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.4 (10.03.2024)
 *
 * Purpose:       Controlling the final software controlled relay responsible
 *                for igniting the engine
 */

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
#include <stdint.h>

#include "Globals.h"

//static SemaphoreHandle_t ignitionMutex;

void initIgnition(){
  pinMode(IGNITER_CONTROL_PIN, OUTPUT);
  digitalWrite(IGNITER_CONTROL_PIN, LOW);
  //ignitionMutex = xSemaphoreCreateMutex();
}

void setIgnition(bool state){
  //if (xSemaphoreTake(ignitionMutex, 10) == pdTRUE){ 
    digitalWrite(IGNITER_CONTROL_PIN, state);
    //xSemaphoreGive(ignitionMutex);
    //}
}

void getIgnition(bool* ignitionState){
  //if (xSemaphoreTake(ignitionMutex, 10) == pdTRUE){
    *ignitionState = digitalRead(IGNITER_CONTROL_PIN);
    //xSemaphoreGive(ignitionMutex);
  //}
}