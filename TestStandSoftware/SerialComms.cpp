/* Filename:      SerialComms.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.45 (11.05.2024)
 *
 * Purpose:       Responsible for sending the latest sensor measurements over
 *                the Arduino Serial interface to a Rock 4C+ microcomputer.
 */

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>

#include "Globals.h"
//static SemaphoreHandle_t serialMutex;
//static SemaphoreHandle_t messageMutex;

static char datalineMessage[512];  //As much as I despise dynamic memory allocation, I also don't like this
static char* msg = datalineMessage;

uint32_t lastTime = 0;
uint32_t count = 0;

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
  //if (xSemaphoreTake(serialMutex, 10) == pdTRUE){

  int msgIndex = 4;

  long comb1 = values.N2FeedingPressure;
  comb1 = comb1 << (10) | values.linePressure;
  comb1 = comb1 << (10) | values.combustionPressure;

  long comb2 = values.N2OFeedingPressure;
  comb2 = comb2 << (10) | values.loadCell;
  comb2 = comb2 << (10) | values.bottleTemperature;

  long comb3 = values.nozzleTemperature;
  comb3 = comb3 << (14) | values.pipingTemperature;

  long comb4 = values.IR;
  comb4 = comb4 << (10) | values.dumpValveButton;
  comb4 = comb4 << (1) | values.heatingBlanketButton;
  comb4 = comb4 << (1) | values.ignitionButton;
  comb4 = comb4 << (1) | values.n2FeedingButton;
  comb4 = comb4 << (1) | values.oxidizerValveButton;
  comb4 = comb4 << (1) | statusValues.ignitionEngagedActive;
  comb4 = comb4 << (1) | statusValues.valveActive;
  comb4 = comb4 << (3) | statusValues.mode;
  comb4 = comb4 << (3) | statusValues.subState;
  comb4 = comb4 << (4) | msgIndex;

    Serial.print(values.timestamp);     //Arduino time in us. Dataline index 2
    Serial.print(",");
    /*
    Serial.print(values.N2FeedingPressure);     //N2 Feeding pressure . Dataline index 3
    Serial.print(",");
    Serial.print(values.linePressure);     //Line pressure . Dataline index 4 
    Serial.print(",");
    Serial.print(values.combustionPressure);     //Chamber pressure . Dataline index 5
    */
    Serial.print(comb1);
    Serial.print(",");
    /*
    Serial.print(values.N2OFeedingPressure);     //Oxidizer Feeding pressure . Dataline index 6
    Serial.print(",");
    Serial.print(values.loadCell);      //Load cell for thrust. Dataline index 7
    Serial.print(",");
    Serial.print(values.bottleTemperature);  //Bottle/Heating blanket temperature. Dataline index 8
    */
    Serial.print(comb2);
    Serial.print(",");
    /*
    Serial.print(values.notConnectedTemperature);  //Not connected. Dataline index 9
    Serial.print(",");
    Serial.print(values.nozzleTemperature);  //Nozzle temperature. Dataline index 10
    Serial.print(",");
    Serial.print(values.pipingTemperature);  //Piping temperature. Dataline index 11
    */
    Serial.print(comb3);
    Serial.print(",");
    
    /*
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
    */
    Serial.print(comb4);
    Serial.print("\n");

    //Clear message
    //strcpy(msg, " ");
    msgIndex = 0;

  //  xSemaphoreGive(serialMutex);
  //}
    lastTime = micros();
}

void saveMessage(char* message){
    //if (xSemaphoreTake(messageMutex, 10) == pdTRUE){
      strcat(msg, message);
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