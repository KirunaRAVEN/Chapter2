/* Filename:        controlBox.ino
 * Author:          Diego Almendro Wieczorek
 * Date:            14.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Control-Box buttons and switches reading and UART TX.
 */

#include <Arduino.h>
#include "ControlBoxTX.h"

#define CONTROL_BOX_UPDATE_PERIOD 10 // ms
#define CONTROL_BOX_SEND_PERIOD 50 // ms
ControlBoxTX controlBoxTX;

void setup() {
    controlBoxTX.begin();
}

unsigned long lastUpdateTime = 0;
unsigned long lastSendTime = 0;
void loop() {

    if(millis() - lastSendTime >= CONTROL_BOX_UPDATE_PERIOD)
    {
        lastUpdateTime = millis();
        controlBoxTX.updateState();
    }
    if(millis() - lastSendTime >= CONTROL_BOX_SEND_PERIOD)
    {
        lastSendTime = millis();
        controlBoxTX.sendState();
    }
}