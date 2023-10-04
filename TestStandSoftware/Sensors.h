/* Filename:      Sensors.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 *
 * Purpose:       Header file for the Sensors <<active>> object. 
 *                Contains function definitions.
 */

#include "Globals.h"

//Prevent multiple definitions with the if statement
#ifndef SENSORS_H
#define SENSORS_H

/* Function:      Initialize the Sensors object
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initSensors(void);


/* Function:      Intermediate interface for sending measurements to the 
 *                FaultDetection object to be checked for limit violations.
 *                Uses the checkData() interface.
 *
 * IN:            values_t structure containing the newest measurements
 * OUT:           Nothing
 */
void sendToCheck(values_t values);


/* Function:      Intermediate interface for getting the latest measurements
 *                from the proteced LatestValues object. 
 *                Uses the getLatest() interface.
 *
 * IN:            values_t pointer where the newest measurements will be stored
 * OUT:           Nothing
 */
void getValuesFromSensors(values_t* values);


#endif