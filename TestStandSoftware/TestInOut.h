/* Filename:      TestInOut.h
 * Author:        Eemeli Mykrä
 * Date:          17.04.2023
 *
 * Purpose:       Header file for the TestInOut <<device>> object. 
 *                Contains function definitions.
 */
#include <stdint.h>
#include "Globals.h"

#ifndef TESTINOUT_H
#define TESTINOUT_H


/* Function:      Initialize the TestInOut object hardware and software
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initTestInOut(void);


/* Function:      Read the input pins used to control the TEST mode
 *                and to read the actuator states in TEST mode.
 *
 * IN:            testInput_t pointer where the readout will be stored
 * OUT:           Nothing
 */
void readTestInput(testInput_t* testInput);

/* Function:      Set the state of the pin at testOutputPins[pinIndex] to 
 *                the desired state.
 *
 * IN:            uint16_t index of the pin in the testOutputPins array,
 *                Boolean that the pin will be set to
 * OUT:           Nothing
 */
void activateOutputPin(uint16_t pinIndex, bool pinState);


#endif