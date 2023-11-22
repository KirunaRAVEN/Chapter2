/* Filename:      LatestValues.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 *
 * Purpose:       Stores the latest set of sensors measurements in a protected
 *                object. These values are then fetched by the countdown object.
 */

#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include "Globals.h"

static values_t latestValues;

static SemaphoreHandle_t latestValueMutex;

void initLatestValues(){
  latestValues.pressure0 = 0;
  latestValues.pressure1 = 0;
  latestValues.pressure2 = 0;
  latestValues.loadCell0 = 0;
  latestValues.temperature0 = 0;
  latestValues.temperature1 = 0;
  latestValues.temperature2 = 0;
  latestValues.temperature3 = 0;
  latestValues.IR = 0;
  latestValues.timestamp = 0;

  latestValues.dumpValveButton = true; // normally open, so no voltage opens
  latestValues.heatingBlanketButton = false;
  latestValues.ignitionButton = false;
  latestValues.mainValveButton = false;
  latestValues.feedingButton = false;

  latestValueMutex = xSemaphoreCreateMutex();
}

setLatest(values_t values){
  if (xSemaphoreTake(latestValueMutex, 10) == pdTRUE){
    latestValues = values;
    xSemaphoreGive(latestValueMutex);
  }
}

void getLatest(values_t* values){
  if (xSemaphoreTake(latestValueMutex, 10) == pdTRUE){
    *values = latestValues;
    xSemaphoreGive(latestValueMutex);
  }
}