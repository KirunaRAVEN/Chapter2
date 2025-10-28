/* Filename:        UartCommunication.h
 * Author:          Diego Almendro Wieczorek
 * Date:            07.10.2025
 * Version:         V1.00 (07.10.2025)
 *
 * Purpose:         Header file for the UartCommunication class.
 */

#include <Arduino.h>

#ifndef UART_COMMUNICATION_H
#define UART_COMMUNICATION_H

class UartCommunication {
private:
    HardwareSerial* p_serialPort;  // Pointer to the serial interface
    unsigned long baudRate;

public:
    /* Function:    UartCommunication object constructor
    *
    *  IN:          Hardware Serial port (e.g., Serial1) and baud rate (default 9600)
    *  OUT:         Nothing
    */
    UartCommunication(HardwareSerial* p_serialPort, unsigned long baudRate = 9600);

    /* Function:    Initialize the UartCommunication object communication
    *
    *  IN:          Nothing
    *  OUT:         Nothing
    */
    void begin();

    /* Function:    Send a String message via UART
    *
    *  IN:          String message to send
    *  OUT:         Nothing
    */
    void send(const String& message);

    /* Function:    Check how many bytes are available to read
    *
    *  IN:          Nothing
    *  OUT:         Number of bytes available to read, or -1 if error
    */
    int8_t available();

    /* Function:    Read a single character
    *
    *  IN:          Nothing
    *  OUT:         A single character read from UART
    */
    char readChar();

    /* Function:    Read a full line (blocking until newline)
    *
    *  IN:          Nothing
    *  OUT:         A String containing the line read from UART
    */
    String readLine();
};

#endif
