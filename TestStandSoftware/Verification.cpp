/* Filename:      Verification.cpp
 * Author:        Eemeli Mykrä
 * Date:          07.06.2023
 *
 * Purpose:       Responsible for running the sequence through a verification sequence.
 *                Prompts the control box operator to press buttons at relevant times.
 */

#include <Arduino.h>
#include <Arduino_FreeRTOS.h>
#include <stdint.h>
#include "Globals.h"
#include "Verification.h"
#include "Ignition.h"
#include "Valves.h"
#include "TestAutomation.h"

static bool ignitionPowerPassed;
static bool ignitionGroundPassed;
static bool ignitionSoftwarePassed;
static bool heatingPassed;
static bool valvePassed;
static bool allPassed;
static bool testCompleted;

static verificationState_t verificationState;

static uint32_t testStateChangeTime;
static uint32_t endCountTime;
static int16_t endCount;
static char message[64];
static char* msg = message;

bool runVerificationStep(values_t buttonValues, testInput_t testInput){
  if (!testCompleted){
    switch (verificationState){
      case TEST_START:
        msg = "Running testing sequence\n";
        sendMessageToSerial(msg);
        msg = "Testing all OFF-states...\nRelease all buttons!\n\n";
        sendMessageToSerial(msg);
        verificationState = OFF_STATE_BUTTON;
        break;

      case OFF_STATE_BUTTON:
        if (!buttonValues.mainValveButton && !buttonValues.ignitionButton && !buttonValues.heatingBlanketButton){
          msg = "No button presses detected\n";
          sendMessageToSerial(msg);
          testStateChangeTime = millis();
          verificationState = OFF_STATE_TEST;

          setTestOutput(0, true);
          setIgnition(false);
          setValve(pin_names_t::MAIN_VALVE_PIN, false);
          }
        break;
        
      case OFF_STATE_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          ignitionPowerPassed = (buttonValues.ignitionButton == false);
          ignitionGroundPassed = (testInput.IGN_GND_IN > ignitionGroundOpenPassLimit);
          ignitionSoftwarePassed = (testInput.IGN_SW_IN == false);
          heatingPassed = (buttonValues.heatingBlanketButton == false);
          valvePassed = (testInput.MAIN_VALVE_IN == true);   //Inverted input

          allPassed = allPassed && ignitionPowerPassed && ignitionGroundPassed && ignitionSoftwarePassed && heatingPassed && valvePassed;

          msg = "Ignition 24V relay OFF-state:\n";
          sendMessageToSerial(msg);
          msg = ignitionPowerPassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);
          
          msg = "Ignition GND relay OFF-state:\n";
          sendMessageToSerial(msg);
          msg = ignitionGroundPassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Ignition SW relay OFF-state:\n";
          sendMessageToSerial(msg);
          msg = ignitionSoftwarePassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Heating relay OFF-state:\n";
          sendMessageToSerial(msg);
          msg = heatingPassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Main Valve OFF-state:\n";
          sendMessageToSerial(msg);
          msg = valvePassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          setTestOutput(0, false);
          
          verificationState = HEAT_ON_BUTTON;
        
          msg = "Testing heating relay ON-state...\n";
          sendMessageToSerial(msg);          
          msg = "Press the Heating button!\n";
          sendMessageToSerial(msg);
        }
        break;

      case HEAT_ON_BUTTON:
        if (buttonValues.heatingBlanketButton){
          msg = "Heating button press detected\n";
          sendMessageToSerial(msg);

          testStateChangeTime = millis();
          verificationState = HEAT_ON_TEST;
        }
        break;

      case HEAT_ON_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          heatingPassed = (buttonValues.heatingBlanketButton == true);

          allPassed = allPassed && heatingPassed;

          msg = "Heating relay ON-state:\n";
          sendMessageToSerial(msg);
          msg = heatingPassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Release the Heating button\n\n";
          sendMessageToSerial(msg);

          verificationState = HEAT_RELEASE;

        }      
        break;
      
      case HEAT_RELEASE:
        if (!buttonValues.heatingBlanketButton){    
          verificationState = VALVE_ON_BUTTON;

          msg = "Testing manual valve ON-state...\n";
          sendMessageToSerial(msg);
          msg = "Press the Manual Vent button!\n";
          sendMessageToSerial(msg);

        }
        break;
      
      case VALVE_ON_BUTTON:
        if (buttonValues.mainValveButton){
          msg = "Manual vent button press detected\n";
          sendMessageToSerial(msg);

          setValve(pin_names_t::MAIN_VALVE_PIN, true);

          testStateChangeTime = millis();
          verificationState = VALVE_ON_TEST;
        }
        break;
        
      case VALVE_ON_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          valvePassed = (testInput.MAIN_VALVE_IN == false);   //Inverted input

          allPassed = allPassed && valvePassed;


          msg = "Main valve ON-state:\n";
          sendMessageToSerial(msg);
          msg = valvePassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Release the Manual Vent button\n\n";
          sendMessageToSerial(msg);

          verificationState = VALVE_RELEASE;
        }
        break;

      case VALVE_RELEASE:
        if (!buttonValues.mainValveButton){
          setValve(pin_names_t::MAIN_VALVE_PIN, false);

          verificationState = IGN_ON_BUTTON;

          msg = "Testing ignition relays ON-state...\n";
          sendMessageToSerial(msg);
          msg = "Press the Ignition button!\n";
          sendMessageToSerial(msg);
        }
        break;
      
      case IGN_ON_BUTTON:
        if (buttonValues.ignitionButton){
          msg = "Ignition button press detected\n";
          sendMessageToSerial(msg);

          setTestOutput(0, true);

          testStateChangeTime = millis();
          verificationState = IGN_ON_TEST;
        }
        break;

      case IGN_ON_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          ignitionPowerPassed = (buttonValues.ignitionButton == true);
          ignitionGroundPassed = (testInput.IGN_GND_IN < ignitionGroundClosedPassLimit);
          ignitionSoftwarePassed = (testInput.IGN_SW_IN == false); //Inverted output

          allPassed = allPassed && ignitionPowerPassed && ignitionGroundPassed && ignitionSoftwarePassed;


          msg = "Ignition Power relay ON-state:\n";
          sendMessageToSerial(msg);
          msg = ignitionPowerPassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Ignition Ground relay ON-state:\n";
          sendMessageToSerial(msg);
          msg = ignitionGroundPassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Ignition Software relay ON-state:\n";
          sendMessageToSerial(msg);
          msg = ignitionSoftwarePassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);

          msg = "Release the Ignition button\n\n";
          sendMessageToSerial(msg);

          verificationState = IGN_RELEASE;

        }
        break;

      case IGN_RELEASE:
        if (!buttonValues.ignitionButton){
          setTestOutput(0, false);

          msg = "Actuator testing completed...\n";
          sendMessageToSerial(msg);
          msg = "Verification status:\n";
          sendMessageToSerial(msg);
          msg = allPassed ? "Passed\n" : "Failed\n";
          sendMessageToSerial(msg);


          if (allPassed){
            msg = "All tests passed!\nStarting up software...\n";
            sendMessageToSerial(msg);
          }
          else{
            msg = "Fault detected!\nFind and fix the issue!\n";
            sendMessageToSerial(msg);
            msg = "Restarting test sequence...\n";
            sendMessageToSerial(msg);
          }
          verificationState = TEST_END;
          //Immediately start the countdown
          endCountTime = millis() - 1000;
        }
        break;

      case TEST_END:
        if (millis() - endCountTime >= 1000){
          msg = "";
          sendMessageToSerial(itoa(endCount, msg, 10));
          msg = "...\n";
          sendMessageToSerial(msg);

          endCountTime = millis();
          endCount--;
        }
        if (endCount == -1){
          //msg = "0...\n";
          //sendMessageToSerial(msg);
          if (allPassed){
            testCompleted = true;
          }
          else{
            testCompleted = false;
            //Reset values for new test sequence
            allPassed = true;
            endCount = verificationEndCount;
            verificationState = TEST_START;
          }
        }
        break; 
    }
  }

  return testCompleted;
}


void initVerification(){
  //Tests are assumed to pass by default. Failures set these to false.
  ignitionPowerPassed = true;
  ignitionGroundPassed = true;
  ignitionSoftwarePassed = true;
  heatingPassed = true;
  valvePassed = true;
  
  /* Initially set to pass, bitwise AND with all the other tests
   * determines if all tests have passed.
   */
  allPassed = true;

  //Set to true once the tests have passed
  testCompleted = false;
  
  //Initial state for the verification
  verificationState = TEST_START;

  //Counter for seconds at the end of the test.
  endCount = verificationEndCount;
}