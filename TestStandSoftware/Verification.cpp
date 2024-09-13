/* Filename:      Verification.cpp
 * Author:        Eemeli Mykrä
 * Date:          07.06.2023
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Responsible for running the sequence through a verification sequence.
 *                Prompts the control box operator to press buttons at relevant times.
 */

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
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
static bool oxidizerValvePassed;
static bool allPassed;
static bool testCompleted;

static bool buttonFaultMessageFlag = false;

static verificationState_t verificationState;

static uint32_t testStateChangeTime;
static uint32_t endCountTime;
static int16_t endCount;
//static char message[64];
//static char* msg = message;
static uint16_t msg;

bool runVerificationStep(values_t buttonValues, testInput_t testInput){
  if (!testCompleted){
    switch (verificationState){
      case TEST_START:
        sendMessageToSerial(MSG_TEST_SEQUENCE_START);
        verificationState = OFF_STATE_BUTTON;
        break;

      case OFF_STATE_BUTTON:
        if (!buttonValues.oxidizerValveButton && !buttonValues.ignitionButton && !buttonValues.heatingBlanketButton){
          sendMessageToSerial(MSG_NO_BUTTONS);
          testStateChangeTime = millis();
          verificationState = OFF_STATE_TEST;

          setTestOutput(IGN_GND_RELAY_TEST_DRIVE_PIN, true);
          setIgnition(false);
          setValve(pin_names_t::OXIDIZER_VALVE_PIN, false);
          }
          // Logic to warn the console user which buttons are still pressed before test sequence can continue.
          else if (buttonFaultMessageFlag == false){
            if (buttonValues.oxidizerValveButton == true){
              sendMessageToSerial(MSG_RELEASE_OX);
              buttonFaultMessageFlag = true;
            }
            if (buttonValues.ignitionButton == true){
              sendMessageToSerial(MSG_RELEASE_IGN);
              buttonFaultMessageFlag = true;
            }
            if (buttonValues.heatingBlanketButton == true){
              sendMessageToSerial(MSG_RELEASE_HEAT);
              buttonFaultMessageFlag = true;
            }
          }
          
        break;
        
      case OFF_STATE_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          ignitionPowerPassed = (buttonValues.ignitionButton == false);
          ignitionGroundPassed = (testInput.IGN_GND_IN > ignitionGroundOpenPassLimit);
          ignitionSoftwarePassed = (testInput.IGN_VOLTAGE_IN == false);
          heatingPassed = (buttonValues.heatingBlanketButton == false);
          oxidizerValvePassed = (testInput.MAIN_VALVE_VOLTAGE_IN == true);   //Inverted input

          allPassed = allPassed && ignitionPowerPassed && ignitionGroundPassed && ignitionSoftwarePassed && heatingPassed && oxidizerValvePassed;

          sendMessageToSerial(MSG_IGN_24_OFF);
          sendMessageToSerial(ignitionPowerPassed ? MSG_PASS : MSG_FAIL);
          
          sendMessageToSerial(MSG_IGN_GND_OFF);
          sendMessageToSerial(ignitionGroundPassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_IGN_SW_OFF);
          sendMessageToSerial(ignitionSoftwarePassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_HEAT_OFF);
          sendMessageToSerial(heatingPassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_OX_OFF);
          sendMessageToSerial(oxidizerValvePassed ? MSG_PASS : MSG_FAIL);

          setTestOutput(IGN_GND_RELAY_TEST_DRIVE_PIN, false);
          
          verificationState = HEAT_ON_BUTTON;
        
          sendMessageToSerial(MSG_HEAT_ON_START);   
        }
        break;

      case HEAT_ON_BUTTON:
        if (buttonValues.heatingBlanketButton){
          sendMessageToSerial(MSG_HEAT_BUTTON);

          testStateChangeTime = millis();
          verificationState = HEAT_ON_TEST;
        }
        break;

      case HEAT_ON_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          heatingPassed = (buttonValues.heatingBlanketButton == true);

          allPassed = allPassed && heatingPassed;

          sendMessageToSerial(MSG_HEAT_ON_RESULT);
          sendMessageToSerial(heatingPassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_HEAT_RELEASE);

          verificationState = HEAT_RELEASE;

        }      
        break;
      
      case HEAT_RELEASE:
        if (!buttonValues.heatingBlanketButton){    
          verificationState = VALVE_ON_BUTTON;

          sendMessageToSerial(MSG_OX_ON_START);

        }
        break;
      
      case VALVE_ON_BUTTON:
        if (buttonValues.oxidizerValveButton){
          sendMessageToSerial(MSG_OX_BUTTON);

          setValve(pin_names_t::OXIDIZER_VALVE_PIN, true);

          testStateChangeTime = millis();
          verificationState = VALVE_ON_TEST;
        }
        break;
        
      case VALVE_ON_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          oxidizerValvePassed = (testInput.MAIN_VALVE_VOLTAGE_IN == false);   //Inverted input

          allPassed = allPassed && oxidizerValvePassed;


          sendMessageToSerial(MSG_OX_ON_RESULT);
          sendMessageToSerial(oxidizerValvePassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_OX_RELEASE);

          verificationState = VALVE_RELEASE;
        }
        break;

      case VALVE_RELEASE:
        if (!buttonValues.oxidizerValveButton){
          setValve(pin_names_t::OXIDIZER_VALVE_PIN, false);

          verificationState = IGN_ON_BUTTON;

          sendMessageToSerial(MSG_IGN_ON_START);
        }
        break;
      
      case IGN_ON_BUTTON:
        if (buttonValues.ignitionButton){
          sendMessageToSerial(MSG_IGN_BUTTON);

          setTestOutput(IGN_GND_RELAY_TEST_DRIVE_PIN, true);

          testStateChangeTime = millis();
          verificationState = IGN_ON_TEST;
        }
        break;

      case IGN_ON_TEST:
        //Enough Time has passed
        if (millis() - testStateChangeTime > actuatorTestSettleTime){
          ignitionPowerPassed = (buttonValues.ignitionButton == true);

          //WARNING: FOLLOWING TEST SET TO PASS ALWAYS.
          //This is due to a change in the ignition system rendering the test useless.
          ignitionGroundPassed = true; //(testInput.IGN_GND_IN < ignitionGroundClosedPassLimit);

          ignitionSoftwarePassed = (testInput.IGN_VOLTAGE_IN == false); //Inverted output

          allPassed = allPassed && ignitionPowerPassed && ignitionGroundPassed && ignitionSoftwarePassed;


          sendMessageToSerial(MSG_IGN_ON_24_RESULT);
          sendMessageToSerial(ignitionPowerPassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_IGN_ON_GND_RESULT);
          sendMessageToSerial(ignitionGroundPassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_IGN_ON_SW_RESULT);
          sendMessageToSerial(ignitionSoftwarePassed ? MSG_PASS : MSG_FAIL);

          sendMessageToSerial(MSG_IGN_ON_RELEASE);

          verificationState = IGN_RELEASE;

        }
        break;

      case IGN_RELEASE:
        if (!buttonValues.ignitionButton){
          setTestOutput(IGN_GND_RELAY_TEST_DRIVE_PIN, false);

          sendMessageToSerial(MSG_TEST_FINISH);
          sendMessageToSerial(allPassed ? MSG_PASS : MSG_FAIL);


          if (allPassed){
            sendMessageToSerial(MSG_TEST_PASSED);
          }
          else{
            sendMessageToSerial(MSG_TEST_FAILED);
          }
          verificationState = TEST_END;
          //Immediately start the countdown
          endCountTime = millis() - 1000;
        }
        break;

      case TEST_END:
        if (millis() - endCountTime >= 1000){
          //I'm sorry about this crime against humanity
          //char* listOfNumbers[11] = {"0", "1", "2", "3", "4", "5", "6", "7", "8", "9", "10"};
          //sendMessageToSerial(listOfNumbers[endCount]);
          //msg = "...";
          sendMessageToSerial(MSG_TEST_ENDING);

          endCountTime = millis();
          endCount--;
        }
        if (endCount == -1){
          //msg = "0...\\n";
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
  oxidizerValvePassed = true;
  
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
