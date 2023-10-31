/* Filename:      Valves.h
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 *
 * Purpose:       Header file for the Valves <<device>> object. 
 *                Contains function definitions. Test
 */
 
#ifndef VALVES_H
#define VALVES_H

 /* Function:     Initialize the Valves object hardware and software.
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initValves(void);


/* Function:      Set a specific valve to the desired state.
 *
 * IN:            uint16_t with the number of the valve to be controlled,
 *                Boolean with state the valve will be set to
 * OUT:           Nothing
 */
void setValve(uint16_t valveNum, bool state);


/* Function:      Read the state of a specidic valve.
 *
 * IN:            uint16_t with the number of the valve to be checked,
 *                Boolean pointer where the state will be stored.
 * OUT:           Nothing
 */
void getValve(uint16_t valveNum, bool* valveState);

#endif