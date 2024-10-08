/* Filename:      Temperature.cpp
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Respnsible for reading the temperature sensors found on the 
 *                test bench.
 */

#include <Arduino.h>

#include "Temperature.h"
#include "Globals.h"

/*
  NOTICE: Uses a modified version of the Adafruit library to gain access to the raw spiread32() function
*/
#include "Adafruit_MAX31855_RAVEN.h"

//This pin number list was kept due to the higher number of thermocouples, and the fact that reading TCs is done with differently
static const uint16_t tempChipSelectPins[tempCount] = {THERMOCOUPLE_CS_PIN0, THERMOCOUPLE_CS_PIN1, THERMOCOUPLE_CS_PIN2, THERMOCOUPLE_CS_PIN3};

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

int readTemp(uint16_t sensorNum){
  //digitalWrite(tempChipSelectPins[sensorNum], LOW);
  //Serial.print(thermocouples[sensorNum].readInternal());
  //Serial.print("\n");
  
  //float temperature = thermocouples[sensorNum].readCelsius();
  int32_t temperature = thermocouples[sensorNum].spiread32();
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

  if (temperature & 0x80000000) {
    // Negative value, drop the lower 18 bits and explicitly extend sign bits.
    temperature = 0xFFFFC000 | ((temperature >> 18) & 0x00003FFF);
  } else {
    // Positive value, just drop the lower 18 bits.
    temperature >>= 18;
  }
  // Serial.println(v, HEX);


  // LSB = 0.25 degrees C
  //centigrade *= 0.25;

  return temperature;// * 0.25;
}


int readTMP36(){
  // In V1.51 testing out different ref voltage
  //analogReference(INTERNAL2V56);
  uint16_t val = analogRead(TMP36_INPUT_PIN);
  //Set ref voltage back to default
  //analogReference(DEFAULT);

  return val;

  /* Measurement to value explanation:
   * calibration ADC = Ratio of how much the internal voltage is off from 5.00V
   * sensorValue = measured voltage on the pin, within 0...1023
   * maxADC = 1023, since we are using 10-bit analog to digital converter
   * refADV = expected ADC voltage of 5.00V. In V1.51 testing out different ref voltage
   * (voltage - 0.5) * 100 = voltage to celsius conversion formula for TMP36 sensors
   * Measurement is repeated TMP36AverageCount times each loop to reduce noise
   */
  /*
  float sensorValueSum = 0; 
  for (int i = 0; i < TMP36AverageCount; i++){
    sensorValueSum += analogRead(TMP36_INPUT_PIN);
  }

  float temperature = (calibrationADC * (sensorValueSum / (TMP36AverageCount * maxADC)) * refADC - 0.5) * 100;
  return temperature;
  */
}
