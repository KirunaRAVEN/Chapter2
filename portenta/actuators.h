/* Filename:        actuators.h
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         Actuators library header file.
 */

#ifndef ACTUATORS_H
#define ACTUATORS_H

#include <Arduino.h>
#include "iDevice.h"

#define OPEN true
#define CLOSED !OPEN
#define ACTIVATED true
#define DEACTIVATED !ACTIVATED
 // GENERIC ACTUATOR (relay)
class DigitalActuator : public iDevice {
public:
  DigitalActuator(const char* name, int pin, bool inverse = false);
  void begin() override;
  void update() override;
  void write(bool state);
  void open() {write(OPEN);}
  void close() {write(CLOSED);}
  void activate() {write(ACTIVATED);}
  void deactivate() {write(DEACTIVATED);}
private:
  const int _pin;
  const bool _inverse;
};

 // Servo (e.g. valves)
#include <Servo.h>
class Servovalve : public iDevice {
public:
  Servovalve(const char* name, int pin, int openAngle, int closedAngle);
  void begin() override;
  void update() override;
  void write(bool state);
  void open() {write(OPEN);}
  void close() {write(CLOSED);}
private:
  Servo myservo;
  const int _pin;
  const int _openAngle;
  const int _closedAngle;
};

#endif /* ACTUATORS_H */