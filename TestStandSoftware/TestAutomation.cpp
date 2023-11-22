/* Filename:      TestAutomation.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 *
 * Purpose:       Parent object the decomposes into <<device>> objects
 *                responsible for handling the hardware side of the test.
 *                Also includes objects handling the countdown sequence 
 *                and timing of the test. 
 */
 
#include <stdint.h>

#include "Globals.h"
#include "Mode.h"
#include "Sensors.h"
#include "SerialComms.h"
#include "TestAutomation.h"
#include "TestInOut.h"
#include "Buzzer.h"

void initTestAutomation(){
  //Nothing to initialize currently
}

void setNewMode(mode_t newMode){
  setMode(newMode);
}

void getCurrentMode(mode_t* mode){
  getMode(mode);
}

void setNewSubstate(substate_t newSubstate){
  setSubstate(newSubstate);
}

void getCurrentSubstate(substate_t* substate){
  getSubstate(substate);
}

void forwardGetLatestValues(values_t* values){
  getValuesFromSensors(values);
}

void sendValuesToSerial(values_t values, statusValues_t statusValues){
  writeValues(values, statusValues);
}

void sendMessageToSerial(char* message){
  saveMessage(message);
  //writeMessage(message);
}

/*
void sendIntMessageToSerial(int16_t integer){
  writeIntMessage(integer);
}
*/

void getTestInput(testInput_t* testInput){
  readTestInput(testInput);
}

void setTestOutput(uint16_t pinIndex, bool pinState){
  activateOutputPin(pinIndex, pinState);
}

void setNewBuzzerState(bool newState){
  setBuzzer(newState);
}

void setNewForcedIndicator(bool newState){
  setForcedIndicator(newState);
}

void callBuzzerUpdate(){
  updateBuzzer();
}
