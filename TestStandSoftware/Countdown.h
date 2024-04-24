/* Filename:      Countdown.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.41 (24.04.2024)
 *
 * Purpose:       Header file for the Countdown <<cyclic>> object. 
 *                Contains function definitions.
 */

 //Prevent multiple definitions with the if statement
 #ifndef COUNTDOWN_H
 #define COUNTDOWN_H

/* Function:      Initialize the Countdown object
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initCountdown(void);

/* Function:      FreeRTOS task function for the automated countdown sequence
 *                Reads the current mode and substate of the system and controls
 *                the various actuators based on timing, sensor readings and 
 *                external control
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void countdownLoop(void);

#endif