/* Filename:        ControlBoxRX.h
 * Author:          Diego Almendro Wieczorek
 * Date:            15.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Header file for ControlBoxTX class, This class handles receiving the state of buttons
 *                  and switches on a control box via UART.
 */

#include <Arduino.h>
#include "iDevice.h"

#ifndef CONTROL_BOX_RX_H
#define CONTROL_BOX_RX_H

#define CONTROL_BOX_NAME "ControlBoxRX"
#define CONTROL_BOX_BAUDRATE 115200

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

class ControlBoxRX : iDevice {
public:

    /* Function:    ControlBoxRX constructor.
    *
    *  IN:          Class nameHardwareSerial instance (e.g. &Serial2)
    *  OUT:         Nothing
    */
    ControlBoxRX(HardwareSerial* serial = &Serial2);

    /* Function:    ControlBoxRX initialiser.
    *
    *  IN:          Nothing
    *  OUT:         Nothing
    */
    void begin();

    /* Function:    Read the state of the buttons and switches via UART.
    *
    *  IN:          Nothing.
    *  OUT:         0 if message received correctly. -1 if no data received. -2 if error in message.
    */
    int receiveMessage();
    
    /* Function:    Read the state of the buttons and switches via UART void.
    *
    *  IN:          Nothing
    *  OUT:         Nothing.
    */
    void update() {receiveMessage(); iDevice::_value = (int)_message.allButtons;}

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
