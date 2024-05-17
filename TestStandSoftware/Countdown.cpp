/* Filename:      Countdown.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       This object handles the countdown sequence. It controls the 
 *                mode and substate of the system based on timing or sensor
 *                readings to move through the automated sequence.
 *                Contains the FreeRTOS task called countdownLoop.
 */

//#include <Arduino_FreeRTOS.h>
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
    
    /*
    xTaskCreate(
    countdownLoop        //Name of the task function
    ,  "CountdownLoop"   // A name just for humans
    ,  taskMemoryBytes   // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL              // Ppinter to passed variable
    ,  CRITICAL_PRIORITY // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL);            // Handle to the created task
    */
}

void countdownLoop(){
  //TickType_t lastCountdownWakeTime = xTaskGetTickCount();
  values_t values;

  //Initialize the timing values
  values.timestamp = 0;
  values.msTimestamp = 0;
  values.lastTimestamp = 0;
  values.timeOverflowOffset = 0;

  //Initialize the sending of slower values
  values.slowUpdated = false;

  //Initialize the values after startup
  values.N2FeedingPressure = 0;      //N2 Feeding pressure 
  values.linePressure = 0;      //Oxidizer line pressure 
  values.combustionPressure = 0;      //Combustion chamber pressure 
  values.N2OFeedingPressure = 0;      //Oxidizer Feeding pressure 
  values.loadCell = 0;       //Back of the engine
  values.bottleTemperature = 0;   //Bottle temperature - Switched to TMP36 output, uses different pin
  values.notConnectedTemperature = 0;   //Injector temperature - Usually outputs NaN, not used in live_grapher_V3.py
  values.nozzleTemperature = 0;   //Nozzle temperature
  values.pipingTemperature = 0;   //Piping temperature
  values.IR = 0;             //Plume Temperature

  values.dumpValveButton = true;        //Dump Valve button status. Initialized true, since new nominal state is dump valve open (inverted afterwards due to normally open valve)
  values.heatingBlanketButton = false;  //Heating button status
  values.ignitionButton = false;        //Ignition button status
  values.n2FeedingButton = false;       //N2 Feeding valve status
  values.oxidizerValveButton = false;   //Main oxidizer valve status


  mode_t currentMode;
  substate_t currentSubstate;
  testInput_t testInput;
  static bool ignitionValveStateFlag;

  // Add the capability to print lines to the ground station computer if needed
  static char message[64];
  static char* msg = message;

  statusValues_t statusValues;
  bool valveState;
  bool ignitionState;

  //In forced sequence the system reverts back to initial state for repeated testing.
  //Uses the old forced sequence functionality, hence the variable naming.
  bool forcedSequence = false;

  bool verificationDone = true;

  uint32_t countdownStartTime = 0;
  uint32_t ignitionPressTime = 0;
  while (true){

    //activateMeasurement();

    getCurrentMode(&currentMode);
    getCurrentSubstate(&currentSubstate);

    callBuzzerUpdate();
    
    //Read test pins
    getTestInput(&testInput);

    if (testInput.resetSW){
      //Ability to reset the Arduino through software
      resetFunc();
    }

    if (testInput.forced == true){
      //The ability to bypass the minimum tank/feeding pressure requirement.
      forcedSequence = true;
      setNewForcedIndicator(true);
    }else{
      forcedSequence = false;
      setNewForcedIndicator(false);
    }

    //Perform and fetch latest measurements
    forwardGetLatestValues(&values);

    //Check latest values for anomalies
    sendToCheck(values);

    // The dump valve is within the main loop as it must always be accessible.
    // As of V1.31 the dump is always operable
    //if (currentMode != SAFE){
    setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
    //}

    //Should we have the ability to control the valves in all modes?
    if ((currentMode != SEQUENCE) && (currentMode != SAFE)){
      setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
      setValve(pin_names_t::OXIDIZER_VALVE_PIN, values.oxidizerValveButton);
      setValve(pin_names_t::N2FEEDING_VALVE_PIN, values.n2FeedingButton);
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
          setNewMode(SEQUENCE);
          ignitionPressTime = millis();
        }

        // If the system returns to WAIT mode for any reason, the flag is reset to its default value.
        ignitionValveStateFlag = false; 

        // In WAIT mode, the operator should have the ability to open and close any controllable valve
        // Dump valve commented out as it is checked in every single loop regardless of mode
        // setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
        setValve(pin_names_t::OXIDIZER_VALVE_PIN, values.oxidizerValveButton);
        setValve(pin_names_t::N2FEEDING_VALVE_PIN, values.n2FeedingButton);
        break;

      case SEQUENCE:
        /* Nested switch - case for the substate. Substate is only active
         * in the SEQUENCE mode. Substates are responsible for engaging and
         * disengaging the ignition relay and opening and closing the main oxidizer
         * valve based on set timing found in the environmental Globals object.
         */

        switch (currentSubstate){     
          case ALL_OFF:
            
            //float realN2OPressure = calibrationADC * refADC * (values.N2OFeedingPressure / maxADC);
            //realN2OPressure = pressureCalibration_K[FEEDING_PRESSURE_OXIDIZER] * realN2OPressure + pressureCalibration_B[FEEDING_PRESSURE_OXIDIZER];

            //All actuators off, wait for button to be held for ignitionSafeTime (ms)
            if (values.ignitionButton == false){
              setNewMode(WAIT);
            }
            
            //We might not want to have a hard pressure limit. Minimum firing 
            //pressure currently set to 0 bar.
            //else if ((realN2OPressure > minimumFiringPressure) || forcedSequence == true){
            if ((millis() - ignitionPressTime > ignitionSafeTime) && values.dumpValveButton == false && values.n2FeedingButton == false && values.oxidizerValveButton == false){
              countdownStartTime = millis();
              setNewSubstate(IGNIT_ON);
              setIgnition(true);
            }
            else {
              if (ignitionValveStateFlag == false){
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
            //}
            break;

          case IGNIT_ON:
              //Ignition has been turned on, wait until oxidiser valve needs to be opened
              
              if (millis() - countdownStartTime > valveOnTime){
                setNewSubstate(VALVE_ON);
                setValve(pin_names_t::OXIDIZER_VALVE_PIN, true);
              }
            break;

          case VALVE_ON:
              //Valve has been opened, wait until ignition can be turned on
              if (millis() - countdownStartTime > ignitionOffTime){
                setNewSubstate(IGNIT_OFF);
                setIgnition(false);
              }
            break;

          case IGNIT_OFF:
              //Ignition has been turned off, wait until oxidiser valve needs to be closed
              if (millis() - countdownStartTime > valveOffTime){
                setNewSubstate(VALVE_OFF);
                setValve(pin_names_t::OXIDIZER_VALVE_PIN, false);
              }
            break;

          case VALVE_OFF:
              //Oxidiser valve has been closed, wait until piping clears from oxidiser
              if (millis() - countdownStartTime > oxidiserEmptyTime){
                setNewSubstate(PURGING);
                setValve(pin_names_t::N2FEEDING_VALVE_PIN, true);
              }
            break;

          case PURGING:
              //Purging in progress, wait until purging finishes
              if (millis() - countdownStartTime > purgingTime){
                setNewSubstate(FINISHED);
                setValve(pin_names_t::N2FEEDING_VALVE_PIN, false);
              }
            break;

          case FINISHED:
            //Sequence is finished
            setNewMode(SHUTDOWN);
            break;
        }
        break;


      case SAFE:
        /* This mode is entered if the FaultDetection object detects values
         * outside safe limits. 
         */

        //Turn off ignition
        setIgnition(false);

        //Ability to control the system in the safe mode as well.
        setValve(pin_names_t::OXIDIZER_VALVE_PIN, values.oxidizerValveButton);
        setValve(pin_names_t::N2FEEDING_VALVE_PIN, values.n2FeedingButton);

        //setValve(pin_names_t::DUMP_VALVE_PIN, false); // FALSE because it is normally open
        //setValve(pin_names_t::N2FEEDING_VALVE_PIN, false); // FALSE because it is normally closed
        // In safe mode, the dump value is hard-coded to open if SAFE MODE is entered.

        // TODO: Do we also want the OXIDIZER_VALVE_PIN open here as well?
        break;

        
      case SHUTDOWN:
        //Testfire over
        // Dump valve commented out as it is checked in every single loop regardless of mode
        // setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
        if (forcedSequence == true){
          setNewSubstate(ALL_OFF);
          setNewMode(WAIT);
        }
        break;
    }


    //Get the valve state using the voltage measurement from TestInOut.
    statusValues.valveActive = !testInput.MAIN_VALVE_IN;   //Iverted input

    getIgnition(&ignitionState);
    statusValues.ignitionEngagedActive = ignitionState;

    statusValues.mode = currentMode;
    statusValues.subState = currentSubstate;

    //If testing is done, start sending out the normal values.
    //NOTE: Could possibly forego this clause and just send them always
    //NOTE: Previous note is now in effect
    //if (verificationDone){
    
    //Serial.println(values.slowUpdated);
    sendValuesToSerial(&values, statusValues);
    
    //}

    if (currentMode != SEQUENCE){
      delay(1000/limitedSampleRate);
    }
  }
}