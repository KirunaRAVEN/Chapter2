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

#include <Arduino_PortentaBreakout.h>
#include "ControlBoxRX.h"
breakoutPin relayPins[] = {PWM0, PWM1, PWM2, CAN1_TX, CAMERA_D0P, CAMERA_D0N, GPIO_4}; //N2,OX1,OX2,
#define CONTROL_BOX_RECEIVE_PERIOD 200 // ms
ControlBoxRX controlBoxRX;
UART UART0Breakout = UART(UART0_TX, UART0_RX);

void setup() {
    for(auto i: relayPins){
        Breakout.pinMode(i, OUTPUT);
        Breakout.digitalWrite(i,HIGH);
    }
    controlBoxRX.begin(&UART0Breakout); // RX=P4 in ESPWROOM32 SUBSTITUTE THE PORTENTA ONE
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
        if (answer == 0) {
            Breakout.digitalWrite(relayPins[0],controlBoxRX.getMessage().ignitionButton?HIGH:LOW);
            Breakout.digitalWrite(relayPins[1],controlBoxRX.getMessage().oxidizerValveButton?HIGH:LOW);
            Breakout.digitalWrite(relayPins[2],controlBoxRX.getMessage().n2FeedingButton?HIGH:LOW);
            Breakout.digitalWrite(relayPins[3],controlBoxRX.getMessage().dumpValveButton?HIGH:LOW);
            Breakout.digitalWrite(relayPins[4],controlBoxRX.getMessage().heatingBlanket1Switch?HIGH:LOW);
            Breakout.digitalWrite(relayPins[5],controlBoxRX.getMessage().heatingBlanket2Switch?HIGH:LOW);
            Breakout.digitalWrite(relayPins[6],controlBoxRX.getMessage().emergencyButton?HIGH:LOW);
        }
        else
        {
            Breakout.digitalWrite(relayPins[0],HIGH);
            Breakout.digitalWrite(relayPins[1],HIGH);
            Breakout.digitalWrite(relayPins[2],HIGH);
            Breakout.digitalWrite(relayPins[3],HIGH);
            Breakout.digitalWrite(relayPins[4],HIGH);
            Breakout.digitalWrite(relayPins[5],HIGH);
            Breakout.digitalWrite(relayPins[6],HIGH);
        }
    }
}

#endif