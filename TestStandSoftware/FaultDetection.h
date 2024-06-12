/* Filename:      FaultDetection.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.53 (12.06.2024)
 *
 * Purpose:       Header file for the FaultDetection <<passive>> object. 
 *                Contains function definitions.
 */

 //Prevent multiple definitions with the if statement
#include "Globals.h"

#ifndef FAULTDETECTION_H
#define FAULTDETECTION_H

/* Function:      Initialize the FaultDetection object
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initFaultDetect(void);

/* Function:      Compares all the newest measurements against safety limits.
 *                If there are violations, change the mode and send a report
 *                using a serial message.
 *
 * IN:            values_t data struct containing all the newest measurements.
 * OUT:           Nothing
 */
void checkData(values_t values);

#endif