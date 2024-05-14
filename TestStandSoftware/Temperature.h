/* Filename:      Temperature.h
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.45 (11.05.2024)
 *
 * Purpose:       Header file for the Temperature <<device>> object. 
 *                Contains function definitions.
 */

 //Prevent multiple definitions with the if statement
#ifndef TEMPERATURE_H
#define TEMPERATURE_H

/* Function:      Initialize the Temperature object hardware and software
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initTemp(void);

/* Function:      Read the temperature sensors and return the measurements.
 *
 * IN:            Temperature sensor number to be read.
 * OUT:           Float value with the temperature measurements
 */
float readTemp(uint16_t senorNum);


/* Function:      Read the TMP36 temperature sensors and return the measurements.
 *
 * IN:            Nothing
 * OUT:           Float value with the temperature measurements
 */
float readTMP36(void);

#endif