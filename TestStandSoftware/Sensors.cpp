/* Filename:      Sensors.cpp
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 *
 * Purpose:       Parent object that decomposes to «device» objects for the
 *                various sensors and a «cyclic» task for measuring these sensors.
 */

#include "Globals.h"
#include "LatestValues.h"
#include "FaultDetection.h"

void initSensors(){
  //Nothing to initialize currently
}


void getValuesFromSensors(values_t* values){
  getLatest(values);
}

void sendToCheck(values_t values){
  checkData(values);
}
