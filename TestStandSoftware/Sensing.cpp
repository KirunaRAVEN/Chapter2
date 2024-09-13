/* Filename:      Sensing.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.55 (13.09.2024)
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
static uint32_t lastMediumTime = 0;
 
uint32_t newTime;

bool updateSlow;
bool updateMedium;


void initSensing(){
  //Nothing to initalize here
}

void senseLoop(values_t* values, mode_t currentMode){

  //Save timestamp
  uint32_t newTimestamp = micros();

  //Account for 32-bit counter overflow
  if (newTimestamp < values->checkTimestamp){
    values->timeOverflowOffset = values->timeOverflowOffset + 4294967295;
  }

  values->checkTimestamp = newTimestamp;
  newTimestamp += values->timeOverflowOffset;  //Arduino time in us

  //Calculate time since last values
  uint32_t sampleTimeDiff = newTimestamp - values->lastTimestamp;

  //Serial.print(usPerSample);
  //Serial.print(" ");
  //Serial.println(sampleTimeDiff);
  
  //If not enough time has passed, wait and update timestamp
  if (sampleTimeDiff < usPerSample){
    delayMicroseconds(usPerSample - sampleTimeDiff);
    newTimestamp += (usPerSample - sampleTimeDiff);
    //Serial.print("delayed: ");
    //Serial.println(usPerSample - sampleTimeDiff);
  }

  //Serial.print((uint32_t) (newTimestamp - values->lastTimestamp));
  //Serial.print(" ");
  //Serial.print(sampleTimeDiff);
  //Serial.print(" ");
  //Serial.println(micros());
  
  //Update old and new timestamps
  values->lastTimestamp = newTimestamp;
  values->timestamp = newTimestamp;

  // These values are saved in every MODE and SUBSTATE
  values->combustionPressure = readPressure5V(CHAMBER_PRESSURE);          //Chamber pressure 
  values->loadCell = readLoad();  //Load cell for thrust

  newTime = millis();
  updateMedium = newTime - lastMediumTime > 1000/mediumSensorRate;

  // Non essential values are sent at a reduced rate during the firing
  if (currentMode != SEQUENCE || updateMedium){
    values->mediumUpdated = true;
    lastMediumTime = newTime;

    values->linePressure = readPressure5V(LINE_PRESSURE);                        //Line pressure 
    values->N2FeedingPressure = readPressure5V(FEEDING_PRESSURE_N2);             //N2 Feeding pressure 
    values->N2OFeedingPressure = readPressure5V(FEEDING_PRESSURE_OXIDIZER);      //Oxidizer Feeding Pressure 

    updateSlow = newTime - lastSlowTime > 1000/slowSensorRate;

    if (updateSlow == true) {
      values->slowUpdated = true;
      lastSlowTime = newTime;

      values->bottleTemperature = readTMP36();                     //Bottle/Heating blanket temperature
      values->notConnectedTemperature = 0;//readTemp(NOT_CONNECTED_1);   //Not connected
      values->nozzleTemperature = readTemp(NOZZLE_TC);             //Nozzle temperature
      values->pipingTemperature = readTemp(PIPING_TC);            //Piping temperature

      values->IR = readIR();   //Plume temperature
    }

    //Read control signals
    values->dumpValveButton = readDumpValveButton();           //Dump Valve button status (inverted afterwards due to normally open valve)
    values->heatingBlanketButton = readHeatingButton();        //Heating button status
    values->ignitionButton = readIgnitionButton();             //Ignition button status
    values->n2FeedingButton = readN2FeedingValveButton();      //N2 Feeding button status
    values->oxidizerValveButton = readOxidizerValveButton();   //Main oxidizer button status
    
    }
}
