/* Filename:        ControlBoxRX.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            15.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Source file for ControlBoxRX class, This class handles receiving the state of buttons.
 *
 */

#include "controlBoxRX.h"
ControlBoxRX::ControlBoxRX(const char* name, HardwareSerial* serial)
    : iDevice(name), _serial(serial) {}

void ControlBoxRX::begin() {
    _serial->begin(CONTROL_BOX_BAUDRATE);
}

int ControlBoxRX::receiveMessage() {
    _message.allButtons = 0x01; // Reset the message
    int availableBytes = _serial->available();
    if (availableBytes > 10) {
        while (_serial->available() > 0) {
            _serial->read();  // Dump everything
        }
        _message.allButtons = 0x01; // Reset the message
        return -3; // Error in message: too much strange data (reconnection)
    } else if (availableBytes > 0) {
        while (_serial->available() > 1) {
            _serial->read();  // Dump everything except the last byte
        }
        _message.allButtons = _serial->read();
        if (_message.reserved == 1) {
            _message.allButtons = 0x01; // Reset the message
            return -2; // Error in message
        }
        else {
            return 0; // Message received correctly
        }
    } else {
        _message.allButtons = 0x01; // Reset the message
        return -1; // No data received
    }
}

