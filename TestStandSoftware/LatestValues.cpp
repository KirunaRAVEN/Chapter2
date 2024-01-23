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
  latestValues.pressure0 = 0;      //Feeding line
  latestValues.pressure1 = 0;      //Oxidizer line
  latestValues.pressure2 = 0;      //Combustion chamber
  latestValues.pressure3 = 0;      // --- TO CHANGE
  latestValues.loadCell = 0;       //Back of the engine
  latestValues.temperature0 = 0;   //Bottle temperature - Switched to TMP36 output, uses different pin
  latestValues.temperature1 = 0;   //Injector temperature - Usually outputs NaN, not used in live_grapher_V3.py
  latestValues.temperature2 = 0;   //Nozzle temperature
  latestValues.temperature3 = 0;   //Ambient temperature
  latestValues.IR = 0;             //Plume Temperature
  latestValues.timestamp = 0;      //When was this set of values collected

  latestValues.dumpValveButton = false; // normally open, so no voltage opens
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