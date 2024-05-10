/* Filename:      Verification.h
 * Author:        Eemeli Mykrä
 * Date:          07.06.2023
 * Version:       V1.44 (06.05.2024)
 *
 * Purpose:       Header file for the Verification <<passive>> object. 
 *                Contains function definitions.
 */

#include "Globals.h"

#ifndef VERIFICATION_H
#define VERIFICATION_H


/* Function:      Initializes the Verification object software
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
 void initVerification(void);


/* Function:      Performs a step in the verification process based on the
 *                current verificationState, button presses and timing. Returns
 *                true when the verification has been performed succesfully.
 *
 * IN:            Latest values for control buttons and test pins       
 * OUT:           Boolean stating of the test has been completed       
 */
bool runVerificationStep(values_t buttonValues, testInput_t testInput);

#endif