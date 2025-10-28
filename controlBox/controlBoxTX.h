/* Filename:        ControlBoxTX.h
 * Author:          Diego Almendro Wieczorek
 * Date:            15.10.2025
 * Version:         V1.01 (28.10.2025)
 *
 * Purpose:         Header file for ControlBoxTX class, This class handles reading the state of buttons
 *                  and switches on a control box as well as sending their states via UART.
 */

#include <Arduino.h>

#ifndef CONTROL_BOX_TX
#define CONTROL_BOX_TX

#define CONTROL_BOX_BAUDRATE 115200
#define CONTROL_BOX_UPDATE_TIME 10 // ms

enum ControlBoxButton {
    IGNITION_BUTTON,
    OXIDIZER_VALVE_BUTTON,
    N2_FEEDING_BUTTON,
    DUMP_VALVE_BUTTON,
    HEATING_BLANKET_1_SWITCH,
    HEATING_BLANKET_2_SWITCH,
    EMERGENCY_BUTTON,
    BUTTON_COUNT
};

const uint8_t CONTROL_BOX_PINS[] = { 4,5,6,7,8,9,10 };

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

class ControlBoxTX {
public:

    /* Function:    ControlBoxTX initialiser.
    *
    *  IN:          Nothing
    *  OUT:         Nothing
    */
    void begin();

    /* Function:    Read and update the state byte of the buttons and switches.
    *
    *  IN:          Nothing
    *  OUT:         Nothing
    */
    void updateState();

    /* Function:    Send the state of the buttons and switches via UART.
    *
    *  IN:          Nothing
    *  OUT:         Nothing
    */
    void sendState() {Serial.write(_message.allButtons);}

private:
    int8_t _counter[BUTTON_COUNT];
    ControlBoxStateMessage _message;
};

#endif