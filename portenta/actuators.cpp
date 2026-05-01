/* Filename:        actuators.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         Actuators library implementations.
 */
 // GENERIC ACTUATOR
Actuator::Actuator(const char* name, int pin, bool inverse)
    : iDevice(name), _pin(pin), _inverse(inverse) {}

void Actuator::begin() {
    pinMode(_pin, OUTPUT);
    deactivate();
}

void Actuador::update() {
    iDevice::_value = _inverse?!digitalRead(_pin):digitalRead(_pin);
}

void Actuador::write(bool state) {
    digitalWrite(_inverse?!state:state);
}