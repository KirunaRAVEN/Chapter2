/* Filename:      Init.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.31 (10.03.2024)
 *
 * Purpose:       Main initialization file responsible for calling all the other
 *                initialization functions of other objects. Also starts the 
 *                FreeRTOS scheduler.
 */

//#include <Arduino_FreeRTOS.h>

#include "Globals.h"

#include "Init.h"
#include "Mode.h"
#include "SerialComms.h"
#include "Sensors.h"
#include "Pressure.h"
#include "LoadCell.h"
#include "Temperature.h"
#include "Sensing.h"
#include "LatestValues.h"
#include "TestAutomation.h"
#include "Countdown.h"
#include "Valves.h"
#include "Buzzer.h"
#include "Heating.h"
#include "FaultDetection.h"
#include "InfraRed.h"
#include "ControlSensing.h"
#include "Ignition.h"
#include "TestInOut.h"
#include "Verification.h"


void start(){
  initTestInOut();

  initMode();

  initFaultDetect();

  initSerial();

  initSensors();
    initIR();
    initPressure();
    initLoad();
    initTemp();
    initLatestValues();
    initControlSensing();

  initTestAutomation();
    initValves();
    initIgnition();
    //initHeating(); /SW control not available in first version
    initVerification();


  initBuzzer();

  //Initiate tasks last
  initSensing();
  initCountdown();

  //Start the software loop
  countdownLoop();

  //Start FreeRTOS
  //vTaskStartScheduler();

}