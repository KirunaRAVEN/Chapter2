/* Filename:        iDevice.h
 * Author:          Petar Babić and Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         Device interface for every sensor or actuator.
 */

#ifndef IDEVICE_H
#define IDEVICE_H

#include <Arduino.h>

class iDevice {
public:
    // Constructor & Destructor
    iDevice(const char* name):_name(name), _value(0) {}

    // Core interface methods
    virtual void begin() = 0;       // Initialize device
    virtual void update() = 0;      // Update readings
    int read() {return _value;};  // Read stored value (generic return)
    void test(bool updateBefore=false);    // Self-test / health check Serial print
protected:
    const char* _name;
    int _value;
};

#endif /* IDEVICE_H */