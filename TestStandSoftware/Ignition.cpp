/* Filename:      Ignition.cpp
 * Author:        Eemeli Mykrä
 * Date:          29.03.2023
 * Version:       V1.54 (01.07.2024)
 *
 * Purpose:       Controlling the final software controlled relay responsible
 *                for igniting the engine
 */

#include <Arduino.h>
#include <stdint.h>

#include "Globals.h"

void initIgnition(){
  pinMode(IGNITER_CONTROL_PIN, OUTPUT);
  digitalWrite(IGNITER_CONTROL_PIN, LOW);
}

void setIgnition(bool state){
  //digitalWrite(IGNITER_CONTROL_PIN, state);
  if (state == true){PORTB |=  (1 << IGNITER_CONTROL_PIN_PORTB);}
  else              {PORTB &= ~(1 << IGNITER_CONTROL_PIN_PORTB);}

}

void getIgnition(bool* ignitionState){
  //*ignitionState = digitalRead(IGNITER_CONTROL_PIN);
  *ignitionState = PINB & (1 << IGNITER_CONTROL_PIN_PORTB);
}