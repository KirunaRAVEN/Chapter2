/* Filename:      Valves.h
 * Author:        Eemeli Mykrä
 * Date:          27.01.2023
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Header file for the Valves <<device>> object. 
 *                Contains function definitions.
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
void setValve(pin_names_t valve_pin, bool state);


/* Function:      Read the state of a specidic valve.
 *
 * IN:            uint16_t with the number of the valve to be checked,
 *                Boolean pointer where the state will be stored.
 * OUT:           Nothing
 */
void getValve(pin_names_t valve_pin, bool* valveState);

#endif