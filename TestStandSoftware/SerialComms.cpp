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

cppQueue	msgBuffer(sizeof(uint16_t), msgBufferSize, FIFO, true);

void initSerial(){

  Serial.begin(serialBaud);
  //Serial.println(serialBaud);
  while (!Serial){}
  //serialMutex = xSemaphoreCreateMutex();
  //messageMutex = xSemaphoreCreateMutex();

  Serial.print("\n r\n");
  
  //Print the headers for the CSV-file
  /*
  Serial.print("Dataline marker");            //Dataline index 1
  Serial.print(",");
  Serial.print("Arduino time");               //Arduino time in us. Dataline index 2
  Serial.print(",");
  Serial.print("N2 Feeding Pressure");        //N2 Feeding pressure . Dataline index 3
  Serial.print(",");
  Serial.print("Line Pressure");               //Line pressure . Dataline index 4 
  Serial.print(",");
  Serial.print("Chamber Pressure");           //Chamber pressure . Dataline index 5
  Serial.print(",");
  Serial.print("Oxidizer Feeding Pressure");  //Oxidizer Feeding pressure . Dataline index 6
  Serial.print(",");
  Serial.print("Load Cell");                  //Load cell for thrust. Dataline index 7
  Serial.print(",");
  Serial.print("Bottle Temperature");         //Bottle/Heating blanket temperature. Dataline index 8
  Serial.print(",");
  Serial.print("NC");                         //Not connected. Dataline index 9
  Serial.print(",");
  Serial.print("Nozzle Temperature");         //Nozzle temperature. Dataline index 10
  Serial.print(",");
  Serial.print("Piping temperature");        //Piping temperature. Dataline index 11
  Serial.print(",");
  Serial.print("Plume Temperature");          //Plume temperature. Dataline index 12
  Serial.print(",");

  Serial.print("Dump Button");        //Dump valve button status. Dataline index 13
  Serial.print(",");
  Serial.print("Heating Button");     //Heating button status. Dataline index 14
  Serial.print(",");
  Serial.print("Ignition Button");    //Ignition button status. Dataline index 15
  Serial.print(",");
  Serial.print("N2 Feeding Button");  //N2 Feeding button status. Dataline index 16
  Serial.print(",");
  Serial.print("Oxidizer Button");    //Oxidizer valve button status. Dataline index 17
  Serial.print(",");

  Serial.print("Ignition Relay");     //Ignition SW state. Dataline index 18
  Serial.print(",");
  Serial.print("Oxidizer Valve");     //Valve SW state. Dataline index 19
  Serial.print(",");
  Serial.print("SW Mode");            //Current SW mode. Dataline index 20
  Serial.print(",");
  Serial.print("SW Substate");        //Current SW substate. Dataline index 21
  Serial.print(",");
  Serial.print("Message");            //Message field. Dataline index 22
  Serial.print("\n");
  */
}

void writeValues(values_t values, statusValues_t statusValues){

  uint16_t msgIndex = 0;
  if (!msgBuffer.isEmpty()){
    msgBuffer.pop(&msgIndex);
  }

  // First 32bit data - sent always
  long combinedValue1 = values.N2FeedingPressure;
  combinedValue1 = combinedValue1 << (10) | values.linePressure;
  combinedValue1 = combinedValue1 << (10) | values.combustionPressure;
  combinedValue1 = combinedValue1 << (1) |  values.dumpValveButton;
  combinedValue1 = combinedValue1 << (1) | values.heatingBlanketButton;

  //Second 32bit data - sent always
  long combinedValue2 = values.N2OFeedingPressure;
  combinedValue2 = combinedValue2 << (10) | values.loadCell;
  combinedValue2 = combinedValue2 << (1) | values.ignitionButton;
  combinedValue2 = combinedValue2 << (1) | values.n2FeedingButton;
  combinedValue2 = combinedValue2 << (1) | values.oxidizerValveButton;
  combinedValue2 = combinedValue2 << (1) | statusValues.ignitionEngagedActive;
  combinedValue2 = combinedValue2 << (1) | statusValues.valveActive;
  combinedValue2 = combinedValue2 << (3) | statusValues.mode;
  combinedValue2 = combinedValue2 << (3) | statusValues.subState;

  //Third 32bit data - sent at most every 100ms
  long combinedValue3 = values.nozzleTemperature;
  combinedValue3 = combinedValue3 << (14) | values.pipingTemperature;
  combinedValue3 = combinedValue3 << (3) | msgIndex & 7; //first part of the message bits

  //Fourth 32bit data - sent at most every 100ms
  long combinedValue4 = values.bottleTemperature;
  combinedValue4 = combinedValue4 << (3) | (msgIndex >> 3) & 7; //second part of the message bits
  combinedValue4 = combinedValue4 << (10) | values.IR; //For unkown reasons this didn't work with having IR as the first value
  
  uint32_t sentTimeValue = (uint32_t) (values.timestamp >> 3);

  Serial.print(sentTimeValue);     //Arduino time in us. Dataline index 2
  Serial.print(",");
  Serial.print(combinedValue1);
  Serial.print(",");
  Serial.print(combinedValue2);

  if (values.slowUpdated == true){
    Serial.print(",");
    Serial.print(combinedValue3);
    Serial.print(",");
    Serial.print(combinedValue4);

    values.slowUpdated = false;
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