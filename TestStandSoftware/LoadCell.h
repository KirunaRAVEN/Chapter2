/* Filename:      LoadCell.h
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.3 (10.03.2024)
 *
 * Purpose:       Header file for the LoadCell <<device>> object. 
 *                Contains function definitions.
 */

#include <stdint.h>

//Prevent multiple definitions with the if statement
#ifndef LOADCELL_H
#define LOADCELL_H

/* Function:      Initialize the LoadCell object hardware and software
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initLoad(void);

/* Function:      Read the load cells and return the measurements.
 *
 * IN:            Load cell number to read
 * OUT:           float value with the load cell measurement of the given sensor number
 */
float readLoad();

#endif