/* Filename:      SerialComms.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       Responsible for sending the latest sensor measurements over
 *                the Arduino Serial interface to a Rock 4C+ microcomputer.
 */

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>

#include "Globals.h"

#include <cppQueue.h>

//static SemaphoreHandle_t serialMutex;
//static SemaphoreHandle_t messageMutex;

//static char datalineMessage[512];  //As much as I despise dynamic memory allocation, I also don't like this
//static char* msg = datalineMessage;

uint32_t lastTime = 0;
uint32_t count = 0;

cppQueue msgBuffer(sizeof(uint16_t), msgBufferSize, FIFO, true);

void initSerial(){
  Serial.begin(serialBaud);
  while (!Serial){}

  //Indicate reset of the system
  Serial.print("\n r\n");
}

void writeValues(values_t* values, statusValues_t statusValues){

  uint16_t msgIndex = 0;

  uint32_t sentTimeValue = (uint32_t) (values->timestamp >> 3); // Bitshift by 3 to get 8*72 minutes of runtime without 32bit overflow

  // First 32bit data - sent always
  long combinedValue1 = values->N2FeedingPressure;
  combinedValue1 = combinedValue1 << (10) | values->linePressure;
  combinedValue1 = combinedValue1 << (10) | values->combustionPressure;
  combinedValue1 = combinedValue1 << (1) |  values->dumpValveButton;
  combinedValue1 = combinedValue1 << (1) | values->heatingBlanketButton;

  //Second 32bit data - sent always
  long combinedValue2 = values->N2OFeedingPressure;
  combinedValue2 = combinedValue2 << (10) | values->loadCell;
  combinedValue2 = combinedValue2 << (1) | values->ignitionButton;
  combinedValue2 = combinedValue2 << (1) | values->n2FeedingButton;
  combinedValue2 = combinedValue2 << (1) | values->oxidizerValveButton;
  combinedValue2 = combinedValue2 << (1) | statusValues.ignitionEngagedActive;
  combinedValue2 = combinedValue2 << (1) | statusValues.valveActive;
  combinedValue2 = combinedValue2 << (3) | statusValues.mode;
  combinedValue2 = combinedValue2 << (3) | statusValues.subState;

  Serial.print(sentTimeValue);     //Arduino time in us. Dataline index 2
  Serial.print(",");
  Serial.print(combinedValue1);
  Serial.print(",");
  Serial.print(combinedValue2);

  if (values->slowUpdated == true){
    //Serial.println("Slow update");
    
    if (!msgBuffer.isEmpty()){
      msgBuffer.pop(&msgIndex);
    }
    
    //Third 32bit data - sent at most every 100ms
    long combinedValue3 = values->nozzleTemperature;
    combinedValue3 = combinedValue3 << (14) | values->pipingTemperature;
    combinedValue3 = combinedValue3 << (3) | msgIndex & 7; //first part of the message bits

    //Fourth 32bit data - sent at most every 100ms
    long combinedValue4 = values->bottleTemperature;
    combinedValue4 = combinedValue4 << (3) | (msgIndex >> 3) & 7; //second part of the message bits
    combinedValue4 = combinedValue4 << (10) | values->IR; //For unkown reasons this didn't work with having IR as the first value
      
    Serial.print(",");
    Serial.print(combinedValue3);
    Serial.print(",");
    Serial.print(combinedValue4);

    values->slowUpdated = false;
  }

  Serial.print("\n");
}

void saveMessage(uint16_t messageIndex){
    //if (xSemaphoreTake(messageMutex, 10) == pdTRUE){
    msgBuffer.push(&messageIndex);
      //strcat(msg, message);
    //  xSemaphoreGive(messageMutex);
    //}
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