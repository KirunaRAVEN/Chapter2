/* Filename:      Sensing.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.45 (11.05.2024)
 *
 * Purpose:       This object handles the measurements of the different sensors,
 *                storing them to the LatestValues object and checking them for
 *                faults using the FaultDetection object.
 *                Contains the FreeRTOS task called senseLoop.
 */

//#include <Arduino_FreeRTOS.h>
#include <Arduino.h>

#include "Sensors.h"
#include "Sensing.h"
#include "Pressure.h"
#include "LoadCell.h"
#include "Temperature.h"
#include "InfraRed.h"
//#include "LatestValues.h"
#include "Globals.h"
#include "ControlSensing.h"

//values_t values;


void initSensing(){

  /*
    xTaskCreate(
    senseLoop           // Name of the task function
    ,  "SenseLoop"      // A name just for humans
    ,  taskMemoryBytes  // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL             // Poiter to passed variable
    ,  HIGH_PRIORITY    // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL);           // Handle to the created task
    */
}

void senseLoop(values_t* values){
  //values_t values;

  //TickType_t lastSensingWakeTime = xTaskGetTickCount();
  //bool valveState;
  //bool ignitionState;
  //while (true){
    values->N2FeedingPressure = readPressure5V(FEEDING_PRESSURE_N2);             //N2 Feeding pressure 
    values->linePressure = readPressure5V(LINE_PRESSURE);                   //Line pressure 
    values->combustionPressure = readPressure5V(CHAMBER_PRESSURE);                //Chamber pressure 
    values->N2OFeedingPressure = readPressure5V(FEEDING_PRESSURE_OXIDIZER);       //Oxidizer Feeding Pressure 

    values->loadCell = readLoad();  //Load cell for thrust

    values->bottleTemperature = readTMP36();                     //Bottle/Heating blanket temperature
    values->notConnectedTemperature = 0;//readTemp(NOT_CONNECTED_1);   //Not connected
    values->nozzleTemperature = readTemp(NOZZLE_TC);             //Nozzle temperature
    values->pipingTemperature = readTemp(AMBIENT_TC);            //Piping temperature

    values->IR = readIR();   //Plume temperature
    
    //Read control signals
    values->dumpValveButton = readDumpValveButton();           //Dump Valve button status (inverted afterwards due to normally open valve)
    values->heatingBlanketButton = readHeatingButton();        //Heating button status
    values->ignitionButton = readIgnitionButton();             //Ignition button status
    values->n2FeedingButton = readN2FeedingValveButton();      //N2 Feeding button status
    values->oxidizerValveButton = readOxidizerValveButton();   //Main oxidizer button status

    //Save timestamp
    values->timestamp = millis();            //Arduino time in ms

    //Called from Countdown loop in V1.31    
    //sendToCheck(values);

    //setLatest(values);

    //Serial.println(values.N2FeedingPressure);

    //Testing with no delay
    //xTaskDelayUntil(&lastSensingWakeTime, samplingTickDelay);
  //}
}
