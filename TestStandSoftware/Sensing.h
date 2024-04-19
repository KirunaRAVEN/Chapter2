/* Filename:      Sensing.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.4 (19.04.2024)
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


/* Function:      FreeRTOS task handling the measurements of the different sensors,
 *                storing those to the LatestValues object and checking them for
 *                faults using the FaultDetection object.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void senseLoop(values_t* values);

#endif