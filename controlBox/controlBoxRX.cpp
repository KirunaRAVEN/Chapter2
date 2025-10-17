/* Filename:        ControlBoxRX.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            15.10.2025
 * Version:         V1.00 (15.10.2025)
 *
 * Purpose:         Source file for ControlBoxRX class, This class handles receiving the state of buttons.
 *
 */

#include "ControlBoxRX.h"

void ControlBoxRX::begin(HardwareSerial* serial = &Serial) {
    _serial = serial;
    serial->begin(CONTROL_BOX_BAUDRATE);
}

int ControlBoxRX::receiveMessage() {
    _message.allButtons = 0x01; // Reset the message
    if (_serial->available() > 0) {
        while (_serial->available() > 1) {
            _serial->read();  // Dump everything except the last byte
        }
        _message.allButtons = _serial->read();
        if (_message.reserved == 1) {
            return -2; // Error in message
        }
        else {
            return 0; // Message received correctly
        }
    }
    else {
        return -1; // No data received
    }
}

