/* Filename:      Sensing.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 *
 * Purpose:       This object handles the measurements of the different sensors,
 *                storing them to the LatestValues object and checking them for
 *                faults using the FaultDetection object.
 *                Contains the FreeRTOS task called senseLoop.
 */

#include <Arduino_FreeRTOS.h>
#include <Arduino.h>
#include <MemoryFree.h>

#include "Sensors.h"
#include "Sensing.h"
#include "Pressure.h"
#include "LoadCell.h"
#include "Temperature.h"
#include "InfraRed.h"
#include "LatestValues.h"
#include "Globals.h"
#include "ControlSensing.h"

void initSensing(){

    xTaskCreate(
    senseLoop        // Name of the task function
    ,  "SenseLoop"   // A name just for humans
    ,  512           // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL          // Poiter to passed variable
    ,  2             // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL);        // Handle to the created task
}

void senseLoop(){
  values_t values;
  TickType_t lastSensingWakeTime = xTaskGetTickCount();
  //bool valveState;
  //bool ignitionState;
  while (true){
    values.pressure0 = readPressure5V(0);     //Bottle pressure aka. Pre valve oxidizer line
    values.pressure1 = readPressure20mA(0);   //Oxidizer line pressure aka after valve
    values.pressure2 = readPressure20mA(1);   //Chamber pressure

    values.loadCell0 = readLoad(0);           //Load cell for thrust

    values.temperature0 = readTMP36();  //Bottle/Heating blanket temperature
    values.temperature1 = readTemp(1);  //Not connected
    values.temperature2 = readTemp(2);  //Nozzle temperature
    values.temperature3 = readTemp(3);  //Ambient temperature

    values.IR = readIR(0);  //Plume temperature
    
    //Read control signals
    values.venting = readVenting();     //Manual vent button status
    values.heating = readHeating();     //Heating button status
    values.ignition = readIgnition();   //Ignition button status

    //Save timestamp
    values.timestamp = millis();        //Arduino time in ms
    
    sendToCheck(values);
    setLatest(values);

    xTaskDelayUntil(&lastSensingWakeTime, samplingTickDelay);
  }
}
