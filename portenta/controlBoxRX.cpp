/* Filename:        ControlBoxRX.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            15.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Source file for ControlBoxRX class, This class handles receiving the state of buttons.
 *
 */

#include "controlBoxRX.h"

void ControlBoxRX::begin(HardwareSerial* serial = &Serial) {
    _serial = serial;
    _serial->begin(CONTROL_BOX_BAUDRATE);

    _message.allButtons = 0x00; //nothing pressed until told otherwise
    _lastValidMs = 0;
    _everReceived = false;
}

int ControlBoxRX::receiveMessage() {
    int status;
    int availableBytes = _serial->available();

    if (availableBytes > 10) {
        while (_serial->available() > 0) {
            _serial->read();  // Dump everything
        }
        status  = -3; // Error in message: too much strange data (reconnection)
    } else if (availableBytes > 0) {
        while (_serial->available() > 1) {
            _serial->read();  // Dump everything except the last byte
        }
        ControlBoxStateMessage candidateMessage;
        candidateMessage.allButtons = _serial->read();

        if (1 == candidateMessage.reserved) {
            status = -2; // Error in message
        } else {
            _message = candidateMessage;
            _lastValidMs = millis();
            _everReceived = true;
            status = 0; // Message received correctly
        }
    } else {
        status = -1;  // nothing this loop: keep the last good state

    }

    if (!isLinkAlive()){
        _message.allButtons = 0x00; // Reset to no buttons pressed if link is dead
    }
    return status;
}

bool ControlBoxRX::isLinkAlive() {
    return _everReceived && (millis() - _lastValidMs < CONTROL_BOX_TIMEOUT_MS); // Link is alive if we have ever received a message and the last valid message was within the timeout period
}

