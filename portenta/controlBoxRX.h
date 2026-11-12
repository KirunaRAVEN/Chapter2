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
#define CONTROL_BOX_TIMEOUT_MS 300 // link counts as dead after 300 ms without a valid byte (6 missed messages at 20 Hz)

// State of Control-Box buttons and switches
union ControlBoxStateMessage {
    struct {
        uint8_t ignitionButton : 1;
        uint8_t oxidizerButton : 1;
        uint8_t nitrogenButton : 1;
        uint8_t dumpButton : 1;
        uint8_t heating1Switch : 1;
        uint8_t heating2Switch : 1;
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

    /* Function: Is the control box still talking to us? 
    *
    *  IN:          Nothing
    *  OUT:         true if the control box is still sending messages, false otherwise.
    */
    bool isLinkAlive();

private:
    HardwareSerial* _serial; // Reference to the HardwareSerial instance (e.g. &Serial2)
    ControlBoxStateMessage _message;
    unsigned long _lastValidMs; // Timestamp of the last valid message received
    bool _everReceived; // Flag to indicate if any valid message has ever been received
};

#endif
