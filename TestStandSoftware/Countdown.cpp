/* Filename:      Countdown.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 *
 * Purpose:       This object handles the countdown sequence. It controls the 
 *                mode and substate of the system based on timing or sensor
 *                readings to move through the automated sequence.
 *                Contains the FreeRTOS task called countdownLoop.
 */

#include <Arduino_FreeRTOS.h>
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
    
    xTaskCreate(
    countdownLoop        //Name of the task function
    ,  "CountdownLoop"   // A name just for humans
    ,  512               // This stack size can be checked & adjusted by reading the Stack Highwater
    ,  NULL              // Ppinter to passed variable
    ,  3                 // Priority, with 3 (configMAX_PRIORITIES - 1) being the highest, and 0 being the lowest.
    ,  NULL);            // Handle to the created task
}

void countdownLoop(){
  TickType_t lastCountdownWakeTime = xTaskGetTickCount();
  values_t values;
  mode_t currentMode;
  substate_t currentSubstate;
  testInput_t testInput;

  // Add the capability to print lines to the ground station computer if needed
  static char message[64];
  static char* msg = message;

  statusValues_t statusValues;
  bool valveState;
  bool ignitionState;

  //In forced sequence the system dismisses the feeding pressure limit for firing.
  bool forcedSequence = false;

  bool verificationDone = true;

  uint32_t countdownStartTime = 0;
  uint32_t ignitionPressTime = 0;
  while (true){
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

    //Fetch latest measurements
    forwardGetLatestValues(&values);

    // The dump valve is within the main loop as it must always be accessible.
    // A check is done for SAFE mode to avoid conflicts, as SAFE mode has the
    // dump valve hard-coded to open regardless of button input.
    if (currentMode != SAFE){
        setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
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

        // In WAIT mode, the operator should have the ability to open and close any controllable valve
        // Dump valve commented out as it is checked in every single loop regardless of mode
        // setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
        setValve(pin_names_t::MAIN_VALVE_PIN, values.mainValveButton);
        setValve(pin_names_t::FEEDING_VALVE_PIN, values.feedingButton);
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
              if ((millis() - ignitionPressTime > ignitionSafeTime) && values.dumpValveButton == false && values.feedingButton == true){
                countdownStartTime = millis();
                setNewSubstate(IGNIT_ON);
                setIgnition(true);
              }
              else {
                // TODO: Add flag so that this message is only sent once. Do it here
                // and not inside the if cases so only one boolean is needed.
                if (values.dumpValveButton == true){
                  msg = "Warning: Cannot begin sequence with dump valve open. \n";
                  sendMessageToSerial(msg);
                }
                if (values.feedingButton == false){
                  msg = "Warning: Cannot begin sequence with feeding valve closed. \n";
                  sendMessageToSerial(msg);
                }
              }
            }
            break;

          case IGNIT_ON:
              if (millis() - countdownStartTime > valveOnTime){
                setNewSubstate(VALVE_ON);
                setValve(pin_names_t::MAIN_VALVE_PIN, true);
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
                setValve(pin_names_t::MAIN_VALVE_PIN, false);
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

        setValve(pin_names_t::DUMP_VALVE_PIN, false); // FALSE because it is normally open
        setValve(pin_names_t::FEEDING_VALVE_PIN, false); // FALSE because it is normally closed
        // In safe mode, the dump value is hard-coded to open if SAFE MODE is entered.

        // TODO: Do we also want the MAIN_VALVE_PIN open here as well?
        break;

        
      case SHUTDOWN:
        //Testfire over
        // Dump valve commented out as it is checked in every single loop regardless of mode
        // setValve(pin_names_t::DUMP_VALVE_PIN, !values.dumpValveButton); //Inverted due to valve being normally open
        break;
    }

    //Using measurement here  instead
    //getValve(0, &valveState);
    //statusValues.valveActive = valveState;
    statusValues.valveActive = testInput.MAIN_VALVE_IN;

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

    xTaskDelayUntil(&lastCountdownWakeTime, countdownTickDelay);
  }
}