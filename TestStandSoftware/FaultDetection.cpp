/* Filename:      FaultDetection.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.41 (24.04.2024)
 *
 * Purpose:       Checks each set of measured data against set thresholds.
 *                If the limit is passed consecutively, SAFE mode is triggered. 
 *                Also triggers the warning if sensors values are close to the
 *                thresholds.
 */
 
#include <stdint.h>

#include "Globals.h"
#include "FaultDetection.h"
#include "Mode.h"
#include "Buzzer.h"

static int16_t feedingPassCount;
static int16_t chamberPassCount;
static int16_t casingPassCount;
static mode_t fetchedMode;
static bool fetchedWarning;
static bool activateSafe;
static bool activateWarning;

void initFaultDetect(){
  feedingPassCount = 0;
  chamberPassCount = 0;
  casingPassCount = 0;
}

//Do we want to explicitly record information of passed values?
//It can also be analysed from data after.
void checkData(values_t values){
  
  activateSafe = false;
  activateWarning = false;
  getMode(&fetchedMode);
  getWarning(&fetchedWarning);
  
  //REDLINE TRIGGERS
  if (values.pressure3 > feedingPressureThreshold){
    feedingPassCount++;
    if (feedingPassCount > successivePasses){
      activateSafe = true;
    }
  }else{feedingPassCount = 0;}

  if (values.pressure2 > chamberPressureThreshold){
    chamberPassCount++;
    if (chamberPassCount > successivePasses){
      activateSafe = true;
    }
  }else{chamberPassCount = 0;}

  if (values.temperature2 > casingTemperatureThreshold){
    casingPassCount++;
    if (casingPassCount > successivePasses){
      activateSafe = true;
    }
  }else{casingPassCount = 0;}

  //WARNING TRIGGERS
  if (values.pressure0 > feedingPressureWarning){
      activateWarning = true;
  }

  //Affecting the modes
  if ((fetchedMode != SAFE) && (activateSafe == true)){
    setMode(SAFE);
    //What other things to trigger when entering safe mode?
    //Buzzer?
    
    //Serial.print("Threshold Triggered!\n");

  }

  /* Checks if the determined warning status is different from the current status.
   * This could be done in a more simple way by rewriting the value to the 
   * Arduino pin every loop. This could be done in Countdown loop.
   */ 
  if (fetchedWarning != activateWarning){
    setWarning(activateWarning);
    setBuzzer(activateWarning);
    /*
    if (activateWarning == true){
      startBuzzerPattern(BUZZER_WARNING);
    }
    else{
      startBuzzerPattern(BUZZER_OFF);
    }
    */
  }

  


}
