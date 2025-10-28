/* Filename:        UartCommunication.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            07.10.2025
 * Version:         V1.00 (07.10.2025)
 *  
 * Purpose:         The UartCommunication class provides an interface for UART communication.
 *                  This class allows initializing the serial port, sending and receiving data.
 *                  It was designed to communicate the ground station with the Portenta H7 via UART.
 */

UartCommunication::UartCommunication(HardwareSerial* p_serialPort_, unsigned long baudRate_ = 9600) {
    p_serialPort = p_serialPort_;
    baudRate = baudRate_;
}

void UartCommunication::begin() {
    if (p_serialPort) {
        p_serialPort->begin(baudRate);
    }
}

void UartCommunication::send(const String& message) {
    if (p_serialPort) {
        p_serialPort->print(message);
    }
}

int8 UartCommunication::available() {
    if(p_serialPort == nullptr) {
        return -1; // Error: Serial port not initialized
    } else {
        return p_serialPort->available();
    }
}

char UartCommunication::readChar() {
    if (p_serialPort && p_serialPort->available()) {
        return p_serialPort->read();
    }
    return '\0';
}

String UartCommunication::readLine() {
    String line = "";
    while (p_serialPort && p_serialPort->available()) {
        char c = p_serialPort->read();
        if (c == '\n') break;
        line += c;
    }
    return line;
}