/*
Sensors library
*/
#ifndef SENSORS_H
#define SENSORS_H

#include "globals.h"
#include "iDevice.h"

#define SENSOR_RESOLUTION 10  // ADC bits
#define V_REF 5.0             // voltage reference
#define MAX_PRESSURE 100.0    //the maximum measurable pressure

#define LOADCELL_PRESCALER 0.001f // To fit the measurement in the int range
#define THERMOCOUPLE_PRESCALER 100.0f // To fit the measurement in the int range

// #define LOADCELL_FACTOR .539f/-7766.0f*9.81f;
// //Standard measured 0.5L Monster Can Weight (539 g) 
// //divided by measured raw value for that weight (-7766) and transformed to N (1kg = 9.81N).
// #define GENERAL_PRESSURE_FACTOR MAX_PRESSURE / ((1 << SENSOR_RESOLUTION) - 1)
// #define CHAMBER_PRESSURE_FACTOR GENERAL_PRESSURE_FACTOR / 4
// // #define TEMPERATURE_FACTOR V_REF / ((1 << SENSOR_RESOLUTION) - 1);
// #define IR_TEMPERATURE_FACTOR 0.8678f;
// #define IR_TEMPERATURE_OFFSET -26.18f;
// // Regression from (temp/rawMeasurement): (10/42)(23/56)(27/60)(34/70)
// #define OX_TEMPERATURE_FACTOR 100.0f;
// #define OX_TEMPERATURE_OFFSET -50.0f;

// GENERIC SENSOR
class Sensor : public iDevice {
public:
  Sensor(const char* name, int pin);
  void begin() override;
  void update() override;
private:
  const int _pin;
};

// LOADCELL HX711
#include <HX711.h>  // Rob Tilaart library
class Loadcell : public iDevice {
public:
  Loadcell(const char* name, int dataPin, int clockPin);
  void begin() override;
  void update() override;
private:
  HX711 loadcell;
  const int _dataPin;
  const int _clockPin;
};

// THERMOCOUPLE Adafruit_MAX31855
#include "Adafruit_MAX31855.h"
class Thermocouple : public iDevice {
public:
  Thermocouple(const char* name, int csPin);
  void begin() override;
  void update() override;
private:
  Adafruit_MAX31855 thermocouple;
};

// FROM HERE ON, THE CODE IS TO BE DELETED AT THE END OF THE OOP REFACTOR
void initLoadcell();

int readAllSensors();

float fastRead();

#endif /* SENSORS_H */
