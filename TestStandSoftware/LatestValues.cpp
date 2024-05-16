/* Filename:      LatestValues.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       Stores the latest set of sensors measurements in a protected
 *                object. These values are then fetched by the countdown object.
 */

//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>

#include "Globals.h"

static values_t latestValues;

//static SemaphoreHandle_t latestValueMutex;

void initLatestValues(){
  latestValues.N2FeedingPressure = 0;      //N2 Feeding pressure 
  latestValues.linePressure = 0;      //Oxidizer line pressure 
  latestValues.combustionPressure = 0;      //Combustion chamber pressure 
  latestValues.N2OFeedingPressure = 0;      //Oxidizer Feeding pressure 
  latestValues.loadCell = 0;       //Back of the engine
  latestValues.bottleTemperature = 0;   //Bottle temperature - Switched to TMP36 output, uses different pin
  latestValues.notConnectedTemperature = 0;   //Injector temperature - Usually outputs NaN, not used in live_grapher_V3.py
  latestValues.nozzleTemperature = 0;   //Nozzle temperature
  latestValues.pipingTemperature = 0;   //Piping temperature
  latestValues.IR = 0;             //Plume Temperature
  latestValues.timestamp = 0;      //When was this set of values collected

  latestValues.dumpValveButton = true;        //Dump Valve button status. Initialized true, since new nominal state is dump valve open (inverted afterwards due to normally open valve)
  latestValues.heatingBlanketButton = false;  //Heating button status
  latestValues.ignitionButton = false;        //Ignition button status
  latestValues.n2FeedingButton = false;       //N2 Feeding valve status
  latestValues.oxidizerValveButton = false;   //Main oxidizer valve status

  //latestValueMutex = xSemaphoreCreateMutex();
}

void setLatest(values_t values){
  //if (xSemaphoreTake(latestValueMutex, 10) == pdTRUE){
    latestValues = values;
    //xSemaphoreGive(latestValueMutex);
  //}
}

void getLatest(values_t* values){
  //if (xSemaphoreTake(latestValueMutex, 10) == pdTRUE){
    *values = latestValues;
    //xSemaphoreGive(latestValueMutex);
  //}
}