/* Filename:        controlBox.ino
 * Author:          Diego Almendro Wieczorek
 * Date:            14.10.2025
 * Version:         V1.00 (14.10.2025)
 *
 * Purpose:         Control-Box buttons and switches reading and UART TX.
 */

#include <Arduino.h>

#include "ControlBoxTX.h"

#define CONTROL_BOX_UPDATE_PERIOD 100 // ms
ControlBoxTX controlBoxTX;

void setup() {
    controlBoxTX.begin();
}

void loop() {
    static unsigned long lastSendTime = 0;
    controlBoxTX.updateState();
    delay(20);
    if(millis() - lastSendTime >= CONTROL_BOX_UPDATE_PERIOD)
    {
        lastSendTime = millis();
        controlBoxTX.sendState();
    }
}