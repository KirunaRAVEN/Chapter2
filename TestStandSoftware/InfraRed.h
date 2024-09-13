/* Filename:      InfraRed.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Header file for the InfraRed <<device>> object. 
 *                Contains function definitions.
 */

 //Prevent multiple definitions with the if statement
#ifndef INFRARED_H
#define INFRARED_H

/* Function:      Initialize the InfraRed object hardware and software
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initIR(void);

/* Function:      Read the infrared sensors and return the measurements.
 *
 * IN:            Number of the sensor to be read. 
 * OUT:           Float with the measurement in degrees of Celsius
 */
int readIR();

#endif