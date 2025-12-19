#ifndef PINMAP_H
#define PINMAP_H

#include <Arduino.h>
#include <Arduino_PortentaBreakout.h>

// UART
//UART UART0Breakout = UART(UART0_TX, UART0_RX);

// I2C

// DIGITAL INPUTS

// breakoutPin digitalInputs[] = {};
// void setInputPins(){
//   for(auto i : digitalInputs[]) {
//       Breakout.pinMode(i, INPUT);
//   }
// }

// ANALOG INPUTS: pressure, themperature and other sensors

// breakoutPin analogInputs[] = {};
// void setInputPins(){
//   for(auto i : analogInputs[]) {
//       Breakout.pinMode(i, INPUT);
//   }
// }

// DIGITAL OUTPUTS

enum ControlBoxButton { // TODO
    IGNITION_BUTTON,
    OXIDIZER_VALVE_BUTTON,
    N2_FEEDING_BUTTON,
    DUMP_VALVE_BUTTON,
    HEATING_BLANKET_1_SWITCH,
    HEATING_BLANKET_2_SWITCH,
    EMERGENCY_BUTTON,
    BUTTON_COUNT
};
//breakoutPin relayPins[] = {PWM0, PWM1, PWM2, CAN1_TX, CAMERA_D0P, CAMERA_D0N, GPIO_4}; //TODO
enum outPins {
    SIREN_SIGNAL,
    LIGHT_SIGNAL,
    HEATING1_RELAY,
    HEATING2_RELAY
    IGNITION_ARM,
    IGNITION_RELAY,
    NITROGEN_RELAY,
    OXIDIZER1_RELAY,
    OXIDIZER2_RELAY,
    DUMP_SIGNAL,
    TEST_LED_SIGNAL,
    HIGH_SPEED_SIGNAL
};

enum inPins {
    IGNITION_BUTTON,
    RESET_BUTTON,
    TEST_MODE_BUTTON
};

#define RELAY_ON LOW
#define RELAY_OFF HIGH
/*
breakoutPin digitalOutputs[] = {};
void setOutputPins(){
  for(auto i : digitalOutputs[]) {
      Breakout.pinMode(i, OUTPUT);
  }
}
*/
#endif /* PINMAP_H */
