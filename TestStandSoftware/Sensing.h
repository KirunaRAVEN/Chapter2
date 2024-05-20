/* Filename:      Sensing.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       Header file for the <<cyclic>> Sensing object. 
 *                Contains function definitions.
 */

//Prevent multiple definitions with the if statement
#ifndef SENSING_H
#define SENSING_H

/* Function:      Initialize the Sensing object
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initSensing(void);


/* Function:      Function for handling the measurements of the different sensors
 *                and saving the timestamp
 *
 * IN:            values_t pointer where the new measurements are saved
 *                substate_t telling which substate the system is in
 * OUT:           Nothing
 */
void senseLoop(values_t* values, substate_t currentSubstate);

#endif