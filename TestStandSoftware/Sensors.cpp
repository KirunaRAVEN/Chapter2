/* Filename:      Sensors.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.4 (19.04.2024)
 *
 * Purpose:       Parent object that decomposes to «device» objects for the
 *                various sensors and a «cyclic» task for measuring these sensors.
 */

#include "Globals.h"
//#include "LatestValues.h"
//#include "FaultDetection.h"
#include "Sensing.h"

void initSensors(){
  //Nothing to initialize currently
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

