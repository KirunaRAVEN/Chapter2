/* Filename:        ControlBoxRX.h
 * Author:          Diego Almendro Wieczorek
 * Date:            15.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Header file for ControlBoxTX class, This class handles receiving the state of buttons
 *                  and switches on a control box via UART.
 */

#include <Arduino.h>

#ifndef CONTROL_BOX_RX_H
#define CONTROL_BOX_RX_H

#define CONTROL_BOX_BAUDRATE 115200

// State of Control-Box buttons and switches
union ControlBoxStateMessage {
    struct {
        uint8_t ignitionButton : 1;
        uint8_t oxidizerValveButton : 1;
        uint8_t n2FeedingButton : 1;
        uint8_t dumpValveButton : 1;
        uint8_t heatingBlanket1Switch : 1;
        uint8_t heatingBlanket2Switch : 1;
        uint8_t emergencyButton : 1;
        uint8_t reserved : 1; // Padding to make the struct size a multiple of 1 byte
    };
    uint8_t allButtons; // Access all buttons as a single byte
};

class ControlBoxRX {
public:

    /* Function:    ControlBoxRX initialiser.
    *
    *  IN:          HardwareSerial instance (e.g. &Serial2)
    *  OUT:         Nothing
    */
    void begin(HardwareSerial* serial);

    /* Function:    Read the state of the buttons and switches via UART.
    *
    *  IN:          &Serial pointer for comunication.
    *  OUT:         0 if message received correctly. -1 if no data received. -2 if error in message.
    */
    int receiveMessage();

    /* Function:   Get the stored state of the buttons and switches.
    *
    *  IN:          Nothing
    *  OUT:         int 0 if ok, -1 if not received, -2 if received with error, -3 if strange data (boot/reconnection).
    */
    ControlBoxStateMessage getMessage() { return _message; }

private:
    HardwareSerial* _serial; // Reference to the HardwareSerial instance (e.g. &Serial2)
    ControlBoxStateMessage _message;
};

#endif
