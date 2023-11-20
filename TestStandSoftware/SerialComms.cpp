/* Filename:      SerialComms.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 *
 * Purpose:       Responsible for sending the latest sensor measurements over
 *                the Arduino Serial interface to a Rock 4C+ microcomputer.
 */

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

#include "Globals.h"
static SemaphoreHandle_t serialMutex;



void initSerial(){

  Serial.begin(serialBaud);
  //Serial.println(serialBaud);
  while (!Serial){}
  serialMutex = xSemaphoreCreateMutex();
  Serial.print(" r\n");
}

void writeValues(values_t values, statusValues_t statusValues){
  if (xSemaphoreTake(serialMutex, 10) == pdTRUE){
    Serial.print("d");
    Serial.print(" ");
    Serial.print(values.timestamp);     //Arduino time in ms
    Serial.print(" ");
    Serial.print(values.pressure0);     //Bottle pressure aka. Pre valve oxidizer line
    Serial.print(" ");
    Serial.print(values.pressure1);     //Oxidizer line pressure aka after valve
    Serial.print(" ");
    Serial.print(values.pressure2);     //Chamber pressure
    Serial.print(" ");
    Serial.print(values.loadCell0);     //Load cell for thrust
    Serial.print(" ");
    Serial.print(values.temperature0);  //Bottle/Heating blanket temperature
    Serial.print(" ");
    Serial.print(values.temperature1);  //Not connected
    Serial.print(" ");
    Serial.print(values.temperature2);  //Nozzle temperature
    Serial.print(" ");
    Serial.print(values.temperature3);  //Ambient temperature
    Serial.print(" ");
    Serial.print(values.IR);            //Plume temperature
    Serial.print(" ");

    Serial.print(values.purgingValveClosed);       //Manual vent button status
    Serial.print(" ");
    Serial.print(values.heatingBlanketOn);       //Heating button status
    Serial.print(" ");
    Serial.print(values.ignitionEngaged);      //Ignition button status
    Serial.print(" ");

    Serial.print(statusValues.ignitionEngagedActive);  //Ignition SW state
    Serial.print(" ");
    Serial.print(statusValues.valveActive);     //Valve SW state
    Serial.print(" ");
    Serial.print(statusValues.mode);            //Current SW mode
    Serial.print(" ");
    Serial.print(statusValues.subState);        //Current SW substate
    Serial.print("\n");

    xSemaphoreGive(serialMutex);
  }
}

writeMessage(char* message){
  if (xSemaphoreTake(serialMutex, 10) == pdTRUE){
    Serial.print(message);
    xSemaphoreGive(serialMutex);
  }
}

writeIntMessage(int16_t integer){
  if (xSemaphoreTake(serialMutex, 10) == pdTRUE){
    Serial.print(integer);
    xSemaphoreGive(serialMutex);
  }
}
