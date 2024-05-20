/* Filename:      Sensing.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       This object handles the measurements of the different sensors,
 *                storing them to the LatestValues object and checking them for
 *                faults using the FaultDetection object.
 *                Contains the FreeRTOS task called senseLoop.
 */

#include <Arduino.h>

#include "Sensors.h"
#include "Sensing.h"
#include "Pressure.h"
#include "LoadCell.h"
#include "Temperature.h"
#include "InfraRed.h"
#include "Globals.h"
#include "ControlSensing.h"

//When were the slower sensors measured last time
static uint32_t lastSlowTime = 0; 


void initSensing(){
  //Nothing to initalize here
}

void senseLoop(values_t* values, substate_t currentSubstate){

  // These values are saved in every MODE and SUBSTATE
  values->linePressure = readPressure5V(LINE_PRESSURE);                   //Line pressure 
  values->combustionPressure = readPressure5V(CHAMBER_PRESSURE);          //Chamber pressure 
  values->loadCell = readLoad();  //Load cell for thrust

  // Non essential values are skipped during the firing
  if (currentSubstate == ALL_OFF || currentSubstate >= PURGING){
    
    values->N2FeedingPressure = readPressure5V(FEEDING_PRESSURE_N2);             //N2 Feeding pressure 
    values->N2OFeedingPressure = readPressure5V(FEEDING_PRESSURE_OXIDIZER);       //Oxidizer Feeding Pressure 

    if (millis() - lastSlowTime > 1000/tempSensorRate){
      values->slowUpdated = true;

      values->bottleTemperature = readTMP36();                     //Bottle/Heating blanket temperature
      values->notConnectedTemperature = 0;//readTemp(NOT_CONNECTED_1);   //Not connected
      values->nozzleTemperature = readTemp(NOZZLE_TC);             //Nozzle temperature
      values->pipingTemperature = readTemp(PIPING_TC);            //Piping temperature

      values->IR = readIR();   //Plume temperature
      
      lastSlowTime = millis();
    }

    //Read control signals
    values->dumpValveButton = readDumpValveButton();           //Dump Valve button status (inverted afterwards due to normally open valve)
    values->heatingBlanketButton = readHeatingButton();        //Heating button status
    values->ignitionButton = readIgnitionButton();             //Ignition button status
    values->n2FeedingButton = readN2FeedingValveButton();      //N2 Feeding button status
    values->oxidizerValveButton = readOxidizerValveButton();   //Main oxidizer button status
    
    //Called from Countdown loop in V1.31    
    //sendToCheck(values);
    //setLatest(values);

    //Save timestamp
    uint32_t newTimestamp = micros();
    if (newTimestamp < values->lastTimestamp){
      values->timeOverflowOffset = values->timeOverflowOffset + 4294967295;
    }
    
    values->timestamp = values->timeOverflowOffset + newTimestamp;  //Arduino time in us
    values->lastTimestamp = newTimestamp;
    }
}
