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
static SemaphoreHandle_t messageMutex;

static char datalineMessage[512];  //As much as I despise dynamic memory allocation, I also don't like this
static char* msg = datalineMessage;


void initSerial(){

  Serial.begin(serialBaud);
  //Serial.println(serialBaud);
  while (!Serial){}
  serialMutex = xSemaphoreCreateMutex();
  messageMutex = xSemaphoreCreateMutex();

  Serial.print(" r\n");
}

void writeValues(values_t values, statusValues_t statusValues){
  if (xSemaphoreTake(serialMutex, 10) == pdTRUE){
    Serial.print("d");                  //Dataline index 1
    Serial.print(",");
    Serial.print(values.timestamp);     //Arduino time in ms. Dataline index 2
    Serial.print(",");
    Serial.print(values.pressure0);     //N2 Feeding pressure --- ORDER TO CHANGE. Dataline index 3
    Serial.print(",");
    Serial.print(values.pressure1);     //Line pressure --- ORDER TO CHANGE. Dataline index 4 
    Serial.print(",");
    Serial.print(values.pressure2);     //Chamber pressure --- ORDER TO CHANGE. Dataline index 5
    Serial.print(",");
    Serial.print(values.pressure3);     //N2O Feeding pressure --- ORDER TO CHANGE. Dataline index 6
    Serial.print(",");
    Serial.print(values.loadCell);      //Load cell for thrust. Dataline index 7
    Serial.print(",");
    Serial.print(values.temperature0);  //Bottle/Heating blanket temperature. Dataline index 8
    Serial.print(",");
    Serial.print(values.temperature1);  //Not connected. Dataline index 9
    Serial.print(",");
    Serial.print(values.temperature2);  //Nozzle temperature. Dataline index 10
    Serial.print(",");
    Serial.print(values.temperature3);  //Ambient temperature. Dataline index 11
    Serial.print(",");
    Serial.print(values.IR);            //Plume temperature. Dataline index 12
    Serial.print(",");

    Serial.print(values.dumpValveButton);       //Dump valve button status. Dataline index 13
    Serial.print(",");
    Serial.print(values.heatingBlanketButton);  //Heating button status. Dataline index 14
    Serial.print(",");
    Serial.print(values.ignitionButton);        //Ignition button status. Dataline index 15
    Serial.print(",");
    Serial.print(values.n2FeedingButton);       //N2 Feeding button status. Dataline index 16
    Serial.print(",");
    Serial.print(values.oxidizerValveButton);   //Oxidizer valve button status. Dataline index 17
    Serial.print(",");

    Serial.print(statusValues.ignitionEngagedActive);   //Ignition SW state. Dataline index 18
    Serial.print(",");
    Serial.print(statusValues.valveActive);             //Valve SW state. Dataline index 19
    Serial.print(",");
    Serial.print(statusValues.mode);                    //Current SW mode. Dataline index 20
    Serial.print(",");
    Serial.print(statusValues.subState);                //Current SW substate. Dataline index 21
    Serial.print(",");
    Serial.print(msg);                                  //Message field. Dataline index 22
    Serial.print("\n");

    //Clear message
    strcpy(msg, " ");

    xSemaphoreGive(serialMutex);
  }
}

void saveMessage(char* message){
    if (xSemaphoreTake(messageMutex, 10) == pdTRUE){
      strcat(msg, message);
      xSemaphoreGive(messageMutex);
    }
}

//Commented out for now to try out the message field in the main data line.
/*
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
*/