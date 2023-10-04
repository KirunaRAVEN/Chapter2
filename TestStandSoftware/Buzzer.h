/* Filename:      Buzzer.h
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 *
 * Purpose:       Header file for the Buzzer <<device>> object. 
 *                Contains function definitions.
 */

#include "Globals.h"

//Prevent multiple definitions with the if statement
 #ifndef BUZZER_H
 #define BUZZER_H

/* Function:      Initialize the pins used to control the buzzer.     
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initBuzzer(void);

/* Function:      Set buzzer to a desired state
 *
 * IN:            State to set the buzzer to: true = ON, false = OFF
 * OUT:           Nothing
 */
void setBuzzer(bool state);


/* Function:      Update the buzzer based on current timing(/pattern)
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void updateBuzzer(void);


/* Function:      Start playing a desired pattern with the buzzer
 *
 * IN:            New pattern to be played
 * OUT:           Nothing
 */
//void startBuzzerPattern(buzzerPattern_t newPattern)

#endif