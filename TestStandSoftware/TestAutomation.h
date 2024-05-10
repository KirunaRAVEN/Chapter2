/* Filename:      TestAutomation.h
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.44 (06.05.2024)
 *
 * Purpose:       Header file for the TestAutomation <<active>> object. 
 *                Contains function definitions.
 */
 
#include <stdint.h>

#include "Globals.h"

#ifndef TESTAUTOMATION_H
#define TESTAUTOMATION_H

/* Function:      Initializes the TestAutomation parent object
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initTestAutomation(void);


/* Function:      Intermediate interface for setting a new mode in the Mode object.
 *                Uses the setMode() interface.
 *
 * IN:            mode_t enum with the desired new mode
 * OUT:           Nothing
 */
void setNewMode(mode_t newMode);


/* Function:      Intermediate interface for getting the current mode from 
 *                the Mode object. Uses the getMode() interface.
 *
 * IN:            Pointer to a mode_t enum where the current mode will be stored
 * OUT:           Nothing
 */
void getCurrentMode(mode_t* mode);


/* Function:      Intermediate interface for setting a new substate in 
 *                the Mode object. Uses the setSubstate() interface
 *
 * IN:            subState_t enum with the desired new substate
 * OUT:           Nothing
 */
void setNewSubstate(substate_t newSubstate);


/* Function:      Intermediate interface for getting the current substate from 
 *                the Mode object. Uses the getSubstate() interface
 *
 * IN:            subState_t pointer where the current substate will be stored
 * OUT:           Nothing
 */
void getCurrentSubstate(substate_t* substate);


/* Function:      Intermediate interface for getting the latest measurements using
 *                another intermediate interface in the Sensors object.
 *                Uses the getLatest() interface.
 *  
 * IN:            values_t pointer where the latest measurements will be stored
 * OUT:           
 */
void forwardGetLatestValues(values_t* values);


/* Function:      Intermediate interface for sending the latest values and the
 *                current state of the SW to the SerialComms object.
 *                Uses the writeValues() interface.
 *    
 * IN:            values_t struct with the latest sensor measurements
 *                statusValues_t struct with information about the status of the SW
 * OUT:           Nothing      
 */
void sendValuesToSerial(values_t values, statusValues_t statusValues);


/* Function:      Intermediate interface for sending text message to the 
 *                SerialComms object. Uses the writeMessage() interface.
 *
 * IN:            Pointer to a character array where the message is stored
 * OUT:           Nothing
 */
 void sendMessageToSerial(char* message);


/* Function:      Intermediate interface for sending a single integer to the 
 *                SerialComms object. Uses the writeIntMessage() interface.
 *
 * IN:            int16_t integer to be sent
 * OUT:           Nothing
 */
 void sendIntMessageToSerial(int16_t integer);


/* Function:      Intermediate interface for getting the current readout of the
 *                test intput pins from the TestInOut object.
 *                Uses the readTestInput() interface.
 *
 * IN:            testInput_t pointer where the readout will be stored
 * OUT:           Nothing
 */
void getTestInput(testInput_t* testInput);


/* Function:      Intermediate interface for setting the state of the output
 *                pins in the TestInOut object.
 *                Uses the activateOutputPin() interface.
 *
 * IN:            uint16_t integer of the index of the pin. See TestInOut.cpp
 * OUT:           
 */
void setTestOutput(uint16_t pinIndex, bool pinState);


/* Function:      Intermediate interface for setting a new state to the buzzer
 *                in the Buzzer object. Uses the setBuzzer() interface.
 *
 * IN:            Boolean that the buzzer is set to. true -> ON, false -> OFF.
 * OUT:           Nothing
 */
void setNewBuzzerState(bool newState);


/* Function:      Intermediate interface for controlling the forced sequence LED
 *                in the Mode object. Uses the setForcedIndicator() interface.
 *
 * IN:            Boolean that the LED is set to. true -> ON, false -> OFF.
 * OUT:           Nothing
 */
void setNewForcedIndicator(bool newState);


/* Function:      Intermediate interface for calling the buzzer update function
 *                in the Buzzer object. Uses the updateBuzzer() interface.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void callBuzzerUpdate(void);



/* Function:      Intermediate interface for calling the senseLoop function
 *                in the sensors --> sensing object. Uses the callSenseLoop() interface.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
//void activateMeasurement(void);

/* Function:      Intermediate interface for sending measurements to the 
 *                FaultDetection object to be checked for limit violations.
 *                Uses the checkData() interface.
 *
 * IN:            values_t structure containing the newest measurements
 * OUT:           Nothing
 */
void sendToCheck(values_t values);



#endif