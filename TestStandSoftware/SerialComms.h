/* Filename:      SerialComms.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.55 (13.09.2024)
 *
 * Purpose:       Header file for the SerialComms <<device>> object. 
 *                Contains function definitions.
 */

#include "Globals.h"

#ifndef SERIALCOMMS_H
#define SERIALCOMMS_H


/* Function:      Initialize the Serial object hardware and software
 *
 * IN:            Nothing
 * OUT:           Nothing
 */
void initSerial(void);


/* Function:      Sends the latest measurements and the status values of the 
 *                system using the Arduino Serial interface.
 *
 * IN:            Pointer to a values_t struct with the latest sensor measurements,
 *                statusValues_t struct with information about the status of the SW
 * OUT:           Nothing
 */
void writeValues(values_t* values, statusValues_t statusValues);


/* WriteMessage and WriteIntMessage are removed for now to test the 
* functionality of a message field in the main data line
*/

/* Function:      Sends a text message using the Serial interface
 * 
 * IN:            Pointer to a character array with the text to be sent
 * OUT:           Nothing
 */
//void writeMessage(char* message);


/* Function:      Sends a single integer using the Serial interface
 *                This is currently only used by the Verification object
 *                and could be replaced by writeMessage above if a working 
 *                int to char array converstion is implemented. 
 *                Both itoa() and sprintf() caused issues.
 * 
 * IN:            A single int16_t integer to be sent
 * OUT:           Nothing
 */
//void writeIntMessage(int16_t integer);


/* Function:      Saves a message to be sent within the dataline.
 *                Once the dataline with the message is sent, 
 *                the message field is cleared.
 * 
 * IN:            A single int16_t integer to be sent
 * OUT:           Nothing
 */
void saveMessage(uint16_t message);


/* Function:      Sends a given byte array of given length using
 *                the Serial.write() interface. Pads the message with
 *                start, end and masking bytes where neccessary
 * 
 * IN:            Pointer to a byte array to be sent
 *                Length of the array to be sent
 * OUT:           Nothing
 */
void sendByteArray(uint8_t *data, uint8_t length);


/* Function:      For switching the BAUD rate on the fly.
 * 
 * IN:            The desired baudrate
 * OUT:           Nothing
 */
void switchBaudrate(uint16_t newBaud);

#endif