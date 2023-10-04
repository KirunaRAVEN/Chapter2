/* Filename:      Temperature.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 *
 * Purpose:       Respnsible for reading the temperature sensors found on the 
 *                test bench.
 */

#include <Arduino.h>

#include "Temperature.h"
#include "Globals.h"
#include "Adafruit_MAX31855.h"

static const uint16_t pinTMP36 = A4;

static const uint16_t tempChipSelectPins[tempCount] = {24, 26, 28, 30};

/* These pin definitions for SPI are not used, as the Adafruit_MAX31855
 * library configures and uses them internally.
 */
//const uint16_t clockPinSPI = 52;
//const uint16_t misoPin = 50;

static Adafruit_MAX31855 thermocouple0(tempChipSelectPins[0]);
static Adafruit_MAX31855 thermocouple1(tempChipSelectPins[1]);
static Adafruit_MAX31855 thermocouple2(tempChipSelectPins[2]);
static Adafruit_MAX31855 thermocouple3(tempChipSelectPins[3]);


static Adafruit_MAX31855 thermocouples[tempCount] = {thermocouple0, thermocouple1, thermocouple2, thermocouple3};

void initTemp(){
  for (uint16_t i = 0; i<tempCount; i++){
    thermocouples[i].begin();
    pinMode(tempChipSelectPins[i], OUTPUT);
  }
} 

float readTemp(uint16_t sensorNum){
  //digitalWrite(tempChipSelectPins[sensorNum], LOW);
  //Serial.print(thermocouples[sensorNum].readInternal());
  //Serial.print("\n");
  float temperature = thermocouples[sensorNum].readCelsius();;
  /*
  if (isnan(temperature)){
    Serial.print("Thermocouple fault(s) detected!\n");
     uint8_t e = thermocouples[sensorNum].readError();
     Serial.println(sensorNum);
     if (e & MAX31855_FAULT_OPEN) Serial.print("FAULT: Thermocouple is open - no connections.\n");
     if (e & MAX31855_FAULT_SHORT_GND) Serial.print("FAULT: Thermocouple is short-circuited to GND.\n");
     if (e & MAX31855_FAULT_SHORT_VCC) Serial.print("FAULT: Thermocouple is short-circuited to VCC.\n");
   } else {
     //Serial.print(temperature);
     //Serial.print("\n");
   }
  */
  
  return temperature;// * 0.25;
}

float readTMP36(){
  float sensorValue = analogRead(pinTMP36);
  float temperature = (calibrationADC * (sensorValue / maxADC) * refADC - 0.5) * 100;
  return temperature;
}
