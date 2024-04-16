/* Filename:      Pressure.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.31 (10.03.2024)
 *
 * Purpose:       Header file for the Pressure <<device>> object. 
 *                Contains function definitions.
 */

 //Prevent multiple definitions with the if statement

#include <stdint.h>


#ifndef PRESSURE_H
#define PRESSURE_H

/* Function:      Initialize the Pressure object hardware and software
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initPressure(void);

/* Function:      Read the 5v output pressure sensors and return the measurements.
 *
 * IN:            Sensor number to be read.
 * OUT:           Float with the pressure measurements
 */
float readPressure5V(uint16_t sensorNumber);

/* Function:      Read the 20mA output pressure sensors and return the measurements.
 *
 * IN:            Sensor number to be read.
 * OUT:           Float with the pressure measurements
 */
float readPressure20mA(uint16_t sensorNumber);

#endif