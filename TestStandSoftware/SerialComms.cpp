/* Filename:      SerialComms.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Responsible for sending the latest sensor measurements over
 *                the Arduino Serial interface to a Rock 4C+ microcomputer.
 */

#include <Arduino.h>
#include <cppQueue.h>

#include "Globals.h"

cppQueue msgBuffer(sizeof(uint16_t), msgBufferSize, FIFO, true);
uint16_t msgIndex = 0;

// Values to send are stored here
unsigned char byteBuffer[20];
unsigned char bufferLength;

//These are used to start, end and mask the message
//const uint8_t START_MARKER = 0x7E;
const uint8_t END_MARKER = 0x7F;
const uint8_t ESCAPE_BYTE = 0x7D;
const uint8_t ESCAPE_XOR = 0x20;

void initSerial(){
  Serial.begin(serialBaudNormal);
  while (!Serial){}

  //Indicate reset of the system
  //Serial.print("\n r\n");
}

void sendByteArray(uint8_t *data, uint8_t length) {
  //Serial.write(START_MARKER);
  for (uint8_t i = 0; i < length; i++) {
    //if (data[i] == START_MARKER || data[i] == END_MARKER || data[i] == ESCAPE_BYTE) {
    if (data[i] == END_MARKER || data[i] == ESCAPE_BYTE) {
      Serial.write(ESCAPE_BYTE);
      Serial.write(data[i] ^ ESCAPE_XOR);
    } else {
      Serial.write(data[i]);
    }
  }
  Serial.write(END_MARKER);
}

void writeValues(values_t* values, statusValues_t statusValues){
  msgIndex = 0;
  bufferLength = 3;
    
  // Only the most essential values are sent in the burst mode
  if (statusValues.mode == SEQUENCE && values->slowUpdated == false && values->mediumUpdated == false){
      // First 32bit data - sent always
      uint32_t combinedValue1 = values->combustionPressure;
      combinedValue1 = combinedValue1 << (10) | values->loadCell;
      combinedValue1 = combinedValue1 << (1) | statusValues.ignitionEngagedActive;
      combinedValue1 = combinedValue1 << (1) | statusValues.valveActive;
      //combinedValue1 = combinedValue1 << 2;
      
      //Add combinedValue1 to byteBuffer
      byteBuffer[0] = combinedValue1 >> 16 & 255;
      byteBuffer[1] = combinedValue1 >> 8 & 255;
      byteBuffer[2] = combinedValue1 & 255;

      //Add combinedValue1 to byteBuffer
      //byteBuffer[0] = combinedValue1 >> 24 & 255;
      //byteBuffer[1] = combinedValue1 >> 16 & 255;
      //byteBuffer[2] = combinedValue1 >> 8 & 255;
      //byteBuffer[3] = combinedValue1 & 255; 

  }else{

    bufferLength = 12;

    uint32_t sentTimeValue = (uint32_t) (values->timestamp >> 3); // Bitshift by 3 to get 8*72 minutes of runtime without 32bit overflow

    byteBuffer[0] = sentTimeValue >> 24 & 255;
    byteBuffer[1] = sentTimeValue >> 16 & 255;
    byteBuffer[2] = sentTimeValue >> 8 & 255;
    byteBuffer[3] = sentTimeValue & 255;

    // First 32bit data - sent always
    uint32_t combinedValue1 = values->N2FeedingPressure;
    combinedValue1 = combinedValue1 << (10) | values->linePressure;
    combinedValue1 = combinedValue1 << (10) | values->combustionPressure;
    combinedValue1 = combinedValue1 << (1) |  values->dumpValveButton;
    combinedValue1 = combinedValue1 << (1) | values->heatingBlanketButton;

    //Add combinedValue1 to byteBuffer
    byteBuffer[4] = combinedValue1 >> 24 & 255;
    byteBuffer[5] = combinedValue1 >> 16 & 255;
    byteBuffer[6] = combinedValue1 >> 8 & 255;
    byteBuffer[7] = combinedValue1 & 255; 

    //Second 32bit data - sent always
    uint32_t combinedValue2 = values->N2OFeedingPressure;
    combinedValue2 = combinedValue2 << (10) | values->loadCell;
    combinedValue2 = combinedValue2 << (1) | values->ignitionButton;
    combinedValue2 = combinedValue2 << (1) | values->n2FeedingButton;
    combinedValue2 = combinedValue2 << (1) | values->oxidizerValveButton;
    combinedValue2 = combinedValue2 << (1) | statusValues.ignitionEngagedActive;
    combinedValue2 = combinedValue2 << (1) | statusValues.valveActive;
    combinedValue2 = combinedValue2 << (3) | statusValues.mode;
    combinedValue2 = combinedValue2 << (3) | statusValues.subState;

    //Add combinedValue2 to byteBuffer
    byteBuffer[8] = combinedValue2 >> 24 & 255;
    byteBuffer[9] = combinedValue2 >> 16 & 255;
    byteBuffer[10] = combinedValue2 >> 8 & 255;
    byteBuffer[11] = combinedValue2 & 255;

    if (values->slowUpdated == true){
      
      if (!msgBuffer.isEmpty()){
        msgBuffer.pop(&msgIndex);
      }

      bufferLength = 20;

      //Third 32bit data - sent at most every 100ms
      uint32_t combinedValue3 = values->nozzleTemperature;
      combinedValue3 = combinedValue3 << (14) | values->pipingTemperature;
      combinedValue3 = combinedValue3 << (3) | msgIndex & 7; //first part of the message bits

      //Add combinedValue3 to byteBuffer
      byteBuffer[12] = combinedValue3 >> 24 & 255;
      byteBuffer[13] = combinedValue3 >> 16 & 255;
      byteBuffer[14] = combinedValue3 >> 8 & 255;
      byteBuffer[15] = combinedValue3 & 255;

      //Fourth 32bit data - sent at most every 100ms
      uint32_t combinedValue4 = values->bottleTemperature;
      combinedValue4 = combinedValue4 << (3) | (msgIndex >> 3) & 7; //second part of the message bits
      combinedValue4 = combinedValue4 << (10) | values->IR; //For unkown reasons this didn't work with having IR as the first value
    
      //Add combinedValue3 to byteBuffer
      byteBuffer[16] = combinedValue4 >> 24 & 255;
      byteBuffer[17] = combinedValue4 >> 16 & 255;
      byteBuffer[18] = combinedValue4 >> 8 & 255;
      byteBuffer[19] = combinedValue4 & 255;

      values->slowUpdated = false;
    }
    
    values->mediumUpdated = false;
  }

  sendByteArray(byteBuffer, bufferLength);
}

void saveMessage(uint16_t messageIndex){
  msgBuffer.push(&messageIndex);
}

void switchBaudrate(uint16_t newBaudrate){
  //Send a message notifying the receiver of the switch
  
  //while (Serial.availableForWrite() < 63){
  //  Serial.flush();
  //}
  /*
  uint8_t emptyArray[0];

  sendByteArray(emptyArray, 0);
  delay(20);

  Serial.end();
  Serial.flush();
  Serial.begin(newBaudrate);

  Serial.write(START_MARKER);
  Serial.write(0x11);
  Serial.write(END_MARKER);
  */
}