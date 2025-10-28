/* Filename:        control_box.h
 * Author:          Diego Almendro Wieczorek
 * Date:            13.10.2025
 * Version:         V1.00 (13.10.2025)
 *
 * Purpose:         Header file for the ControlBox class.
 */

#include <Arduino.h>

#ifndef CONTROL_BOX_H
#define CONTROL_BOX_H

// Connections of Arduino UNO to Control-Box buttons
enum ControlBoxPins {
    BUTTON1_PIN = 2, // Button 1 connected to digital pin 2
    BUTTON2_PIN = 3, // Button 2 connected to digital pin 3
    BUTTON3_PIN = 4, // Button 3 connected to digital pin 4
    BUTTON4_PIN = 5, // Button 4 connected to digital pin 5
    BUTTON5_PIN = 6, // Button 5 connected to digital pin 6
    BUTTON6_PIN = 7, // Button 6 connected to digital pin 7
    BUTTON7_PIN = 8, // Button 7 connected to digital pin 8
    NUM_BUTTONS = 6  // Total number of buttons
};

class ControlBox {
    
}


#endif