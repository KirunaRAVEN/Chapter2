/* Filename:      Buzzer.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.45 (11.05.2024)
 *
 * Purpose:       Controls a buzzer that is used to indicate a restart of the 
 *                system as well as act as a warning if any of the sensor values 
 *                go outside set limits.
 */

#include <Arduino.h>
//#include <Arduino_FreeRTOS.h>
//#include <semphr.h>
#include <stdint.h>

#include "Globals.h"
#include "Buzzer.h"

/* First version of the Test Bench utilizes one of the valve driver mosfets (pin 4) to drive the 
 * buzzer at a higher voltage. To prevent cyclic dependency, the valve pin is
 * reused here instead of calling the Valve-object interfaces. Ideally the next
 * version should have more Mosfets and relays for improved controllability.
 */

//static SemaphoreHandle_t buzzerSemaphore;
static uint32_t buzzerStartTime;

//Current version does not support buzzer patterns
//static int16_t buzzerPatternIndex;
//static int16_t buzzerCycleCount;
//static int16_t* currentPattern;
//static int16_t patternLength;
//static buzzerPattern_t currentPatternName;
//static buzzerTiming_t buzzerTimings;

static bool timedSequence;

void initBuzzer(){
  pinMode(BUZZER_CONTROL_PIN, OUTPUT);
  
  //Turn Buzzer ON when software starts, turns off after ~1 second.
  digitalWrite(BUZZER_CONTROL_PIN, HIGH);
  
  timedSequence = true;
  buzzerStartTime = millis();

  //buzzerSemaphore = xSemaphoreCreateMutex();
}


void setBuzzer(bool state){
  //if (xSemaphoreTake(buzzerSemaphore, 10) == pdTRUE){
    digitalWrite(BUZZER_CONTROL_PIN, state);
  //  xSemaphoreGive(buzzerSemaphore);
  //}
}

void updateBuzzer(){
  /*
  if (currentPatternName != BUZZER_OFF){
    ;
  }*/
  if (timedSequence == true){
    if (millis() - buzzerStartTime >= buzzerOnTime){
      setBuzzer(false);
      timedSequence = false;
    }
  }
}


//Current version does not support buzzer patterns

/*
void startBuzzerPattern(buzzerPattern_t newPattern){
  currentPatternName = newPattern;

  if (currentPatternName == BUZZER_OFF){
    setBuzzer(false);
    return;
  }
  
  buzzerStartTime = millis();

  buzzerPatternIndex = 1;
  buzzerCycleCount = 0;

  switch (newPattern){
    case BUZZER_WARNING:
      currentPattern = &buzzerTimings.warning
      patternLength = sizeof(buzzerTimings.warning/sizeof(buzzerTimings.warning[0]) - 1;
      break;

    case BUZZER_RESET:
      currentPattern = &buzzerTimings.reset
      patternLength = sizeof(buzzerTimings.reset/sizeof(buzzerTimings.reset[0]) - 1;
      break;

    case BUZZER_TEST:
      currentPattern = &buzzerTimings.test
      patternLength = sizeof(buzzerTimings.test/sizeof(buzzerTimings.test[0]) - 1;
      break;
  }
}
*/