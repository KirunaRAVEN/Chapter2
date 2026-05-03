#include "sensors.h"

// GENERIC ANALOG SENSOR
AnalogSensor::AnalogSensor(const char* name, int pin)
    : iDevice(name), _pin(pin) {}

void AnalogSensor::begin() {
    pinMode(_pin, INPUT);
}

void AnalogSensor::update() {
    iDevice::_value = analogRead(_pin);
}

// GENERIC DIGITAL SENSOR
DigitalSensor::DigitalSensor(const char* name, int pin)
    : iDevice(name), _pin(pin) {}

void DigitalSensor::begin() {
    pinMode(_pin, INPUT_PULLUP);
}

void DigitalSensor::update() {
    iDevice::_value = digitalRead(_pin);
}

// LOADCELL HX711
Loadcell::Loadcell(const char* name, int dataPin, int clockPin)
    : iDevice(name), _dataPin(dataPin), _clockPin(clockPin) {}
      
void Loadcell::begin()
{
    _loadcell.begin(_dataPin, _clockPin);
    _loadcell.set_raw_mode();
    _loadcell.tare();
    _loadcell.set_scale(1.0f);
}
void Loadcell::update() {
    iDevice::_value = (int)(_loadcell.get_value()*LOADCELL_PRESCALER);
}


// THERMOCOUPLE Adafruit_MAX31855
Thermocouple::Thermocouple(const char* name, int csPin)
    : iDevice(name), _thermocouple(csPin) {}

void Thermocouple::begin()
{
    _thermocouple.begin();
}
void Thermocouple::update()
{
    iDevice::_value = (int)(_thermocouple.readCelsius()*THERMOCOUPLE_PRESCALER);
}