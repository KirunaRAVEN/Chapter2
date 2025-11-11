/* Filename:        ControlBoxTX.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            15.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Source file for ControlBoxTX class, This class handles reading the state of buttons.
 *
 */

#include "ControlBoxTX.h"

void ControlBoxTX::begin() {
    Serial1.begin(CONTROL_BOX_BAUDRATE);

    for (int i = 0; i < BUTTON_COUNT; i++) {
        pinMode(CONTROL_BOX_PINS[i], INPUT_PULLUP);
        _counter[i] = 0;    // Reset counters
    }

    _message.allButtons = 0x01;

    for (int i = 0; i<20; i++) {
        updateState();
        delay(10);
    }
}

void ControlBoxTX::updateState() {
    _message.allButtons = 0; // Reset the message

    for (int i = 0; i < BUTTON_COUNT; i++) {
        _counter[i] += (digitalRead(CONTROL_BOX_PINS[i]) == LOW) ? 1 : -1;  // Read each pin and sum or subtract from the counter.
        _counter[i] = constrain(_counter[i], 0, 25);                       // Constrain the counter to be between 0 and 100.
        if (_counter[i] > 20) {                                             // If the counter is greater than a threshold, the button is considered pressed.
            bitSet(_message.allButtons, i);
        }
        else {
            bitClear(_message.allButtons, i);
        }
    }
}

int8_t _counter[BUTTON_COUNT];
ControlBoxStateMessage _message;

