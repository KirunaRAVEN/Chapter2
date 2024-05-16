/* Filename:      FaultDetection.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.5 (16.05.2024)
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

static int16_t N2OFeedingPassCount;
static int16_t chamberPassCount;
static int16_t casingPassCount;
static mode_t fetchedMode;
static bool fetchedWarning;
static bool activateSafe;
static bool activateWarning;

void initFaultDetect(){
  N2OFeedingPassCount = 0;
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
  
  //N2O feeding pressure SAFE mode entry disabled for first hot flow  in version V_1.45 on (10.05.2024)
  
  float realN2OPressure = calibrationADC * refADC * (values.N2OFeedingPressure / maxADC);
  realN2OPressure = pressureCalibration_K[FEEDING_PRESSURE_OXIDIZER] * realN2OPressure + pressureCalibration_B[FEEDING_PRESSURE_OXIDIZER];
  /*
  if (values.N2OFeedingPressure > N2OFeedingPressureThreshold){
    N2OFeedingPassCount++;
    if (N2OFeedingPassCount > successivePasses){
      activateSafe = true;
    }
  }else{N2OFeedingPassCount = 0;}
  */

  

  float realCombustionPressure = calibrationADC * refADC * (values.combustionPressure / maxADC);
  realCombustionPressure = pressureCalibration_K[CHAMBER_PRESSURE] * realCombustionPressure + pressureCalibration_B[CHAMBER_PRESSURE];

  if (realCombustionPressure > chamberPressureThreshold){
    chamberPassCount++;
    if (chamberPassCount > successivePasses){
      activateSafe = true;
    }
  }else{chamberPassCount = 0;}

  //Nozzle temperature SAFE mode entry disabled for first hot flow  in version V_1.45 on (10.05.2024)
  /*
  
  float realNozzleTemperature = values.nozzleTemperature * 0.25

  if (realNozzleTemperature > casingTemperatureThreshold){
    casingPassCount++;
    if (casingPassCount > successivePasses){
      activateSafe = true;
    }
  }else{casingPassCount = 0;}
  */

  //WARNING TRIGGERS
  if (realN2OPressure > N2OFeedingPressureWarning){
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
