/* Filename:      Countdown.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       This object handles the countdown sequence. It controls the 
 *                mode and substate of the system based on timing or sensor
 *                readings to move through the automated sequence.
 *                Contains the FreeRTOS task called countdownLoop.
 */

#include <Arduino.h>

#include "Countdown.h"
#include "TestAutomation.h"
#include "Valves.h"
#include "Ignition.h"
#include "Heating.h"
#include "Globals.h"
#include "Verification.h"

void(* resetFunc) (void) = 0;

void initCountdown(){
  // Nothing to initialize there currently
  pinMode(CAMERA_TRIGGER_PIN, OUTPUT);

  //Trigger the camera in case of a reset
  digitalWrite(CAMERA_TRIGGER_PIN, HIGH);
  delay(500); //Keep pin high for 500ms
  digitalWrite(CAMERA_TRIGGER_PIN, LOW);


}

void countdownLoop(){
  values_t values;

  //Initialize the timing values
  values.timestamp = 0;
  values.msTimestamp = 0;
  values.checkTimestamp = 0;
  values.timeOverflowOffset = 0;

  //Initialize the sending of slower values
  values.slowUpdated = false;

  //Initialize the values after startup
  values.N2OFeedingPressure2 = 0;     //Second Oxidizer Feeding pressure 
  values.linePressure = 0;            //Oxidizer line pressure 
  values.combustionPressure = 0;      //Combustion chamber pressure 
  values.N2OFeedingPressure = 0;      //Oxidizer Feeding pressure 
  values.loadCell = 0;                //Back of the engine
  values.bottleTemperature = 0;       //Bottle temperature - Switched to TMP36 output, uses different pin
  values.notConnectedTemperature = 0; //Injector temperature - Usually outputs NaN, not used in live_grapher_V3.py
  values.nozzleTemperature = 0;       //Nozzle temperature
  values.pipingTemperature = 0;       //Piping temperature
  values.IR = 0;                      //Plume Temperature

  values.dumpValveButton = true;        //Dump Valve button status. Initialized true, since new nominal state is dump valve open (inverted afterwards due to normally open valve)
  values.heatingBlanketButton = false;  //Heating button status
  values.ignitionButton = false;        //Ignition button status
  values.n2FeedingButton = false;       //N2 Feeding valve status
  values.oxidizerValveButton = false;   //Main oxidizer valve status


  mode_t currentMode;
  substate_t currentSubstate;
  testInput_t testInput;
  bool ignitionValveStateFlag;

  statusValues_t statusValues;
  bool valveState;
  bool ignitionState;

  bool lastDump = true;

  uint32_t currentTime = 0;

  bool verificationDone = true;

  uint32_t countdownStartTime = 0;
  uint32_t ignitionPressTime = 0;

  while (true){

    getCurrentMode(&currentMode);
    getCurrentSubstate(&currentSubstate);

    // Read test pins, read all of them only outside SEQUENCE
    // If more sampling rate is needed, this could be fully skipped
    // in certain substates >ALL_OFF and <PURGING
    getTestInput(&testInput, currentMode != SEQUENCE);

    //Perform and fetch latest measurements
    forwardGetLatestValues(&values, currentMode);

    //Check latest values for anomalies
    sendToCheck(values);

    // The dump valve is within the main loop as it must always be accessible.
    // As of V1.31 the dump is always operable
    if (lastDump != values.dumpValveButton){
      setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
      lastDump = values.dumpValveButton;
    }

    // Limit the amount of things done in sequence mode to make the burst mode sampling faster
    if (currentMode != SEQUENCE){

      if (testInput.resetSW){
        //Ability to reset the Arduino through software
        resetFunc();
      }

      callBuzzerUpdate();

      setValve(pin_names_t::OXIDIZER_VALVE_PIN, values.oxidizerValveButton);
      setValve(pin_names_t::N2FEEDING_VALVE_PIN, values.n2FeedingButton);

      //The ability to reset the sequence afterwards for repeated testing
      //This updates the LED state
      setNewRepeatIndicator(testInput.repeat);

    }


    //MODE switch case
    switch(currentMode){
      case INIT:
        //Shouldn't ever be here. WAIT/TEST is entered before starting FreeRTOS tasks.
        break;

      case TEST:  
        //Continue with verification and check if was completed succesfully
        verificationDone = runVerificationStep(values, testInput);
        if (verificationDone == true){
          setNewMode(WAIT);
        }
        break;

      case WAIT:
        // The WAIT mode includes heating. The HEATING mode was removed. 
        if (values.ignitionButton == true){
          ignitionPressTime = millis();
          
          setNewMode(SEQUENCE);
          //setNewBaudRate(serialBaudFast);
        }

        // If the system returns to WAIT mode for any reason, the flag is reset to its default value.
        ignitionValveStateFlag = false; 

        break;

      case SEQUENCE:
        /* Nested switch - case for the substate. Substate is only active
         * in the SEQUENCE mode. Substates are responsible for engaging and
         * disengaging the ignition relay and opening and closing the main oxidizer
         * valve based on set timing found in the environmental Globals object.
         */
        currentTime = millis();

        switch (currentSubstate){
          case ALL_OFF:
              
            //All actuators off, wait for button to be held for ignitionSafeTime (ms)
            if (values.ignitionButton == false){
              //setNewBaudRate(serialBaudNormal);
              setNewMode(WAIT);
              
            }else{
              //We might not want to have a hard pressure limit. Minimum firing 
              //pressure currently set to 0 bar. Fully removed since ~V1.5
              if ((currentTime - ignitionPressTime > ignitionSafeTime) && !(values.dumpValveButton || values.n2FeedingButton || values.oxidizerValveButton)){
                countdownStartTime = currentTime;
                setNewSubstate(IGNIT_ON);
                setIgnition(true);
                
              }else if (ignitionValveStateFlag == false){
                if (values.dumpValveButton == true){
                  ignitionValveStateFlag = true;
                  sendMessageToSerial(MSG_DUMP_WARNING);
                }
                if (values.n2FeedingButton == true){
                  ignitionValveStateFlag = true;
                  sendMessageToSerial(MSG_N2_FEED_WARNING);
                }
                if (values.oxidizerValveButton == true){
                  ignitionValveStateFlag = true;
                  sendMessageToSerial(MSG_OX_FEED_WARNING);
                }
              }
            }

            break;

          case IGNIT_ON:
              //Ignition has been turned on, wait until oxidiser valve needs to be opened
              
              if (currentTime - countdownStartTime > valveOnTime){
                setNewSubstate(VALVE_ON);
                setValve(pin_names_t::OXIDIZER_VALVE_PIN, true);
              }
            break;

          case VALVE_ON:
              //Valve has been opened, wait until ignition can be turned off
              if (currentTime - countdownStartTime > ignitionOffTime){
                setNewSubstate(IGNIT_OFF);
                setIgnition(false);
              }
            break;

          case IGNIT_OFF:
              //Ignition has been turned off, wait until oxidiser valve needs to be closed
              if (currentTime - countdownStartTime > valveOffTime){
                setNewSubstate(VALVE_OFF);
                setValve(pin_names_t::OXIDIZER_VALVE_PIN, false);
              }
            break;

          case VALVE_OFF:
              //Oxidiser valve has been closed, wait until piping clears from oxidiser
              if (currentTime - countdownStartTime > oxidiserEmptyTime){
                setNewSubstate(PURGING);
                setValve(pin_names_t::N2FEEDING_VALVE_PIN, true);
              }
            break;

          case PURGING:
              //Trigger the high speed camera cameraTriggerTime ms after ignition
              if (currentTime - countdownStartTime > cameraTriggerTime){
                digitalWrite(CAMERA_TRIGGER_PIN, HIGH);
              }

              //Purging in progress, wait until purging finishes
              if (currentTime - countdownStartTime > purgingTime){
                setNewSubstate(FINISHED);
                setValve(pin_names_t::N2FEEDING_VALVE_PIN, false);
              }
            break;

          case FINISHED:
            //Sequence is finished
            //setNewBaudRate(serialBaudNormal);
            setNewMode(SHUTDOWN);
            break;
        }
        break;


      case SAFE:
        /* This mode is entered if the FaultDetection object detects values
         * outside safe limits. 
         */
        
        //Set substate to the final one
        setNewSubstate(FINISHED);

        //Turn off ignition
        setIgnition(false);

        break;

        
      case SHUTDOWN:
        //Testfire over

        //Turn of camera trigger
        digitalWrite(CAMERA_TRIGGER_PIN, LOW);

        // If repeatSequence is pressed, revert to pre firing state
        if (testInput.repeat == true){
          setNewSubstate(ALL_OFF);
          setNewMode(WAIT);
        }
        
        break;
    }

    //Get the states using the voltage measurement from TestInOut.
    statusValues.valveActive = !testInput.MAIN_VALVE_VOLTAGE_IN;   //Iverted input
    statusValues.ignitionEngagedActive = testInput.IGN_VOLTAGE_IN;
    //Update mode and substate stored in statusValues
    statusValues.mode = currentMode;
    statusValues.subState = currentSubstate;

    //Send out the data through Serial
    sendValuesToSerial(&values, statusValues);
    
    // Limit the sampling rate outside the SEQUENCE mode
    if (currentMode != SEQUENCE){
      delay(1000/limitedSampleRate);
    }
  }
}