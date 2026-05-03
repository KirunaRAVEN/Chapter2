/* Filename:        actuators.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         Actuators library implementations.
 */

#include "actuators.h"

 // DigitalActuator like a relay (for valve, contact or whatever)
DigitalActuator::DigitalActuator(const char* name, int pin, bool inverse)
    : iDevice(name), _pin(pin), _inverse(inverse) {}

void DigitalActuator::begin() {
    pinMode(_pin, OUTPUT);
    deactivate();
}

void DigitalActuator::update() {
    iDevice::_value = _inverse?!digitalRead(_pin):digitalRead(_pin);
}

void DigitalActuator::write(bool state) {
    digitalWrite(_pin, _inverse?!state:state);
}

 // Servovalve (for valve)
Servovalve::Servovalve(const char* name, int pin, int openAngle, int closedAngle)
    : iDevice(name), _pin(pin), _openAngle(openAngle), _closedAngle(closedAngle) {}

void Servovalve::begin() {
    myservo.attach(_pin);
    close();
}

void Servovalve::update() {
    int currentAngle = myservo.read();
    iDevice::_value = (currentAngle==_openAngle)?1:(currentAngle==_closedAngle)?0:-1;
}

void Servovalve::write(bool state) {
    myservo.write((state==OPEN)?_openAngle:_closedAngle);
}