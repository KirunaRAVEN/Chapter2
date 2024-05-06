/* Filename:      LatestValues.h
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.43 (06.05.2024)
 *
 * Purpose:       Header file for the LatestValues <<protected>> object. 
 *                Contains function definitions.
 */

#ifndef LATESTVALUES_H
#define LATESTVALUES_H

/* Function:      Initialize the latest values values_t struct with zeros.
 *                Create the protection semaphore.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initLatestValues(void);


/* Function:      Store the latest measurements in the protected object.
 *
 * IN:            values_t struct containing the values to be stored.
 * OUT:           Nothing
 */
void setLatest(values_t values);


/* Function:      Get the stored values from the protected object using a pointer.
 *
 * IN:            Pointer to a values_t struct where the values will be stored.
 * OUT:           Nothing
 */
void getLatest(values_t* values);

#endif