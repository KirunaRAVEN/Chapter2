/* Filename:      TestStandSoftware.ino
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.31 (10.03.2024)
 *
 * Purpose:       Main file of the Test Stand Software. First file to be
 *                started when Arduino is powered up. Responsible for starting
 *                the start() interface of the Init object.
 */

//For access to the start() interface
#include "Init.h"

//Ran once on startup
void setup() {
  //Use the Init.start() interface for initializing and starting the software.
  start();
}

//Ran continiously. When using FreeRTOS this loop is never used or entered
void loop() {}
