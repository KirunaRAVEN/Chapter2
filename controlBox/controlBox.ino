/* Filename:        controlBox.ino
 * Author:          Diego Almendro Wieczorek
 * Date:            14.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Control-Box buttons and switches reading and UART TX.
 */

#include <Arduino.h>

#define CONTROLBOX_ARDUINO false

#if CONTROLBOX_ARDUINO
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
#else
#include "ControlBoxRX.h"

#define CONTROL_BOX_RECEIVE_PERIOD 200 // ms
ControlBoxRX controlBoxRX;

void setup() {
    Serial.begin(115200);
    controlBoxRX.begin(&Serial2); // RX=P4 in ESPWROOM32
}

unsigned long lastReceiveTime = 0;
void loop() {
    int answer = -1;
    if(millis() - lastReceiveTime >= CONTROL_BOX_RECEIVE_PERIOD)
    {
        answer = controlBoxRX.receiveMessage();
        lastReceiveTime = millis();
        Serial.print(byte(controlBoxRX.getMessage().allButtons), BIN);
        Serial.print(", ");
        Serial.println(answer);
    }
}

#endif