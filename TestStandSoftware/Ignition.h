/* Filename:      Ignition.h
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.54 (01.07.2024)
 *
 * Purpose:       Header file for the Ignition <<device>> object. 
 *                Contains function definitions.
 */

#ifndef IGNITION_H
#define IGNITION_H

/* Function:      Initialize the pins used to control the final ignition relay.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initIgnition(void);


/* Function:      Set the ignition control pin to a desired value
 *
 * IN:            Boolean stating the desires state: true = ON, false = OFF
 * OUT:           Nothing
 */
void setIgnition(bool state);


/* Function:      Read the state of the ignition pin.
 *
 * IN:            Boolean pointer where the current state will be stored
 * OUT:           Nothing
 */
void getIgnition(bool* ignitionState);

#endif