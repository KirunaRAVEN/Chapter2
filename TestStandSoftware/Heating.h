/* Filename:      Heating.h
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       Header file for the Heating <<device>> object. 
 *                Contains function definitions.
 */

 //Prevent multiple definitions with the if statement
 #ifndef HEATING_H
 #define HEATING_H

/* Function:      Initialize the pins used to control the heating blanket.     
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initHeating(void);


/* Function:      Set heating blanket to a desired state
 *
 * IN:            State to set the heating blanket to: true = ON, false = OFF
 * OUT:           Nothing
 */
void setHeating(bool newState);


/* Function:      Get the current state of the heating blanket
 *
 * IN:            Nothing
 * OUT:           State of the heating blanket
 */
void getHeating(bool state);

#endif