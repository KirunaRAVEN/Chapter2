/* Filename:      ControlSensing.h
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 *
 * Purpose:       Header file for the ControlSensing <<device>> object. 
 *                Contains function definitions.
 */

#include "Globals.h"

#ifndef CONTROLSENSING_H
#define CONTROLSENSING_H

/* Function:      Initialize the pins used for reading the control inputs.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initControlSensing(void);


/* Function:      Read the input pin used to initiate manual venting
 *
 * IN:            Nothing
 * OUT:           Boolean describing whether the button has been pressed
 */
bool readVenting(void);


/* Function:      Read the input pin detecting if ignition is requested.
 *
 * IN:            Nothing
 * OUT:           Boolean describing whether the button has been pressed
 */
bool readIgnition(void);


/* Function:      Read the input pin detecting if heating blankets are on.
 *
 * IN:            Nothing
 * OUT:           Boolean describing whether the button has been pressed
 */
bool readHeating(void);

#endif