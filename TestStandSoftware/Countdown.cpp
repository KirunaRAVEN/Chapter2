/* Filename:      Countdown.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.43 (06.05.2024)
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

  //Initialize the values after startup
  values.pressure0 = 0;      //N2 Feeding pressure 
  values.pressure1 = 0;      //Oxidizer line pressure 
  values.pressure2 = 0;      //Combustion chamber pressure 
  values.pressure3 = 0;      //Oxidizer Feeding pressure 
  values.loadCell = 0;       //Back of the engine
  values.temperature0 = 0;   //Bottle temperature - Switched to TMP36 output, uses different pin
  values.temperature1 = 0;   //Injector temperature - Usually outputs NaN, not used in live_grapher_V3.py
  values.temperature2 = 0;   //Nozzle temperature
  values.temperature3 = 0;   //Ambient temperature
  values.IR = 0;             //Plume Temperature
  values.timestamp = 0;      //When was this set of values collected

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
            if (values.ignitionButton == false){
              setNewMode(WAIT);
            }
            
            //We might not want to have a hard pressure limit. Minimum firing 
            //pressure currently set to 0 bar.
            else if ((values.pressure0 > minimumFiringPressure) || forcedSequence == true){
              if ((millis() - ignitionPressTime > ignitionSafeTime) && values.dumpValveButton == false && values.n2FeedingButton == false && values.oxidizerValveButton == false){
                countdownStartTime = millis();
                setNewSubstate(IGNIT_ON);
                setIgnition(true);
              }
              else {
                if (ignitionValveStateFlag == false){
                  if (values.dumpValveButton == true){
                    msg = "Warning:\\nCannot begin sequence\\nwith dump valve open.";
                    ignitionValveStateFlag = true;
                    sendMessageToSerial(msg);
                  }
                  if (values.n2FeedingButton == true){
                    msg = "Warning:\\nCannot begin sequence\\nwith N2 feeding valve open.";
                    ignitionValveStateFlag = true;
                    sendMessageToSerial(msg);
                  }
                  if (values.oxidizerValveButton == true){
                    msg = "Warning:\\nCannot begin sequence\\nwith Oxidizer valve open.";
                    ignitionValveStateFlag = true;
                    sendMessageToSerial(msg);
                  }
                }
              }
            }
            break;

          case IGNIT_ON:
              if (millis() - countdownStartTime > valveOnTime){
                setNewSubstate(VALVE_ON);
                setValve(pin_names_t::OXIDIZER_VALVE_PIN, true);
              }
            break;

          case VALVE_ON:
              if (millis() - countdownStartTime > ignitionOffTime){
                setNewSubstate(IGNIT_OFF);
                setIgnition(false);
              }
            break;

          case IGNIT_OFF:
              if (millis() - countdownStartTime > valveOffTime){
                setNewSubstate(VALVE_OFF);
                setValve(pin_names_t::OXIDIZER_VALVE_PIN, false);
              }
            break;

          case VALVE_OFF:
              if (millis() - countdownStartTime > cooldownTime){
                setNewSubstate(FINISHED);
              }
            break;

          case FINISHED:
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
    sendValuesToSerial(values, statusValues);
    //}

    //Testing with no delay

    //xTaskDelayUntil(&lastCountdownWakeTime, countdownTickDelay);
  }
}