/* Filename:      Mode.h
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.4 (19.04.2024)
 *
 * Purpose:       Header file for the Mode <<protected>> object. 
 *                Contains function definitions.
 */

#include "Globals.h"


#ifndef MODE_H
#define MODE_H

/* Function:      Initialize the object with starting mode and substate.
 *                Create the protection mutex semaphores.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initMode(void);


/* Function:      Set the current mode to the given mode.
 *
 * IN:            New desired mode
 * OUT:           Nothing
 */
void setMode(mode_t newMode);


/* Function:      Get the current mode with a mode_t pointer.
 *
 * IN:            mode_t pointer where the current mode will be stored
 * OUT:           Nothing
 */
void getMode(mode_t *mode);


/* Function:      Set the current substate to the given substate.
 *
 * IN:            New desired substate
 * OUT:           Nothing
 */
void setSubstate(substate_t newSubstate);


/* Function:      Get the current substate with a substate_t pointer.
 *
 * IN:            substate_t pointer where the current substate will be stored.
 * OUT:           Nothing
 */
void getSubstate(substate_t *substate);


/* Function:      CURRENTLY UNUSED.
 *                Could be used to trigger certain events upon entering
 *                the SAFE mode.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void enterSafeMode(void);


/* Function:      Set the warning state to a desired value
 *
 * IN:            Boolean containing the new warning state
 * OUT:           Nothing
 */
void setWarning(bool newWarning);


/* Function:      Get the current warning state with a boolean pointer
 *
 * IN:            Boolean pointer where the current warning state will be stored.
 * OUT:           Nothing
 */
void getWarning(bool *warning);


/* Function:      Set the forced sequence indicator LED to the desired state
 *
 * IN:            State the LED is set to
 * OUT:           Nothing
 */
void setForcedIndicator(bool state);


/* Function:      Set the TEST mode indicator LED to the desired state
 *
 * IN:            State the LED is set to
 * OUT:           Nothing
 */
void setTestModeIndicator(bool state);

#endif