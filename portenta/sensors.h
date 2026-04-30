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
  HX711 _loadcell;
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
  Adafruit_MAX31855 _thermocouple;
};
/*
// FROM HERE ON, THE CODE IS TO BE DELETED AT THE END OF THE OOP REFACTOR
void initLoadcell();

int readAllSensors();

float fastRead();
*/
#endif /* SENSORS_H */
