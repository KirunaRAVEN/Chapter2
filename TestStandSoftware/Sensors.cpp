/* Filename:      Sensors.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.5 (16.05.2024)
 *
 * Purpose:       Parent object that decomposes to «device» objects for the
 *                various sensors and a «cyclic» task for measuring these sensors.
 */

#include "Globals.h"
#include <Arduino.h>
//#include "LatestValues.h"
//#include "FaultDetection.h"
#include "Sensing.h"

void initSensors(){
  // Set the Arduino ADC clock prescaler to get faster analogRead()

    ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits
  
    // uncomment as required
    
    //ADCSRA |= bit (ADPS0);                               //   2  
    //ADCSRA |= bit (ADPS1);                               //   4  
    //ADCSRA |= bit (ADPS0) | bit (ADPS1);                 //   8  
    ADCSRA |= bit (ADPS2);                               //  16 
    //ADCSRA |= bit (ADPS0) | bit (ADPS2);                 //  32 
    //ADCSRA |= bit (ADPS1) | bit (ADPS2);                 //  64 
    //ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // 128

}

void getValuesFromSensors(values_t* values){
  //Changed to get directly from senseLoop in V1.31
  senseLoop(values);
}

/*
void sendToCheck(values_t values){
  checkData(values);
}
*/

/*
void callSenseLoop(){
  senseLoop();
}
*/

