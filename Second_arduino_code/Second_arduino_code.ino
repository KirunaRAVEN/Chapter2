// Authors: Alexander Bülow
// Date: 30/10/2025
// Version: 1.0
// Purpose: Reads and then sends the values read on analog pins 0-4

#include <Wire.h>
#include <multi_channel_relay.h>

Multi_Channel_Relay relay;

static uint64_t micros_epoch = 0;   // counts 2^32-microsecond epochs
static uint32_t last_micros = 0;
const uint8_t PIN_BTN_1   = 2; //Button for heating relay 1
const uint8_t PIN_BTN_2   = 3; //Button for heating relay 1
const uint8_t PIN_STAT_1 = 13; //Status from relay 1
const uint8_t PIN_STAT_2 = 8; //Status from relay 2

void setup() {
  // Initialize serial communication at 1000000 baud
  Serial.begin(115200);
  // Set the Arduino ADC clock prescaler to get faster analogRead()
  #if defined(ADCSRA)
    ADCSRA &= ~(bit(ADPS0) | bit(ADPS1) | bit(ADPS2)); // clear prescaler
    ADCSRA |= bit(ADPS2);                               // prescaler = 16
  #endif

  pinMode(PIN_BTN_1, INPUT); //Button input
  pinMode(PIN_BTN_2, INPUT); //Button input
  pinMode(PIN_STAT_1, INPUT); //Status input
  pinMode(PIN_STAT_2, INPUT); //Status input

  Wire.begin();
  // Set I2C address and start relay
  relay.begin(0x11);
  // Ensures we are in a off position to start with (A bit redundant but it is fine)
  relay.turn_off_channel(1);
  relay.turn_off_channel(4);
}
 
void loop() {
  uint32_t t1 = millis();
  // Analog reads 
  int32_t nitrogenPressure = analogRead(A0);
  int32_t blanketTemp1     = analogRead(A2);
  int32_t blanketTemp2     = analogRead(A3);

  // Digital reads 
  bool Heating_blanket_button_1 = (digitalRead(PIN_BTN_1) == HIGH);
  bool Heating_blanket_button_2 = (digitalRead(PIN_BTN_2) == HIGH);
  bool blanketstatus1 = digitalRead(PIN_STAT_1);
  bool blanketstatus2 = digitalRead(PIN_STAT_2);

  // Switches relay on or off depending on button input
  if (Heating_blanket_button_1) {
    relay.turn_on_channel(1);
  } else {
    relay.turn_off_channel(1);
  }

  if (Heating_blanket_button_2) {
    relay.turn_on_channel(4);
  } else {
    relay.turn_off_channel(4);
  }

  //Account for 32-bit counter overflow
  uint32_t now = micros();
  if (now < last_micros) {
    // micros() wrapped (every 71.6 minutes)
    micros_epoch += (1ULL << 32);
  }
  last_micros = now;
  uint64_t ts_us = micros_epoch + now;

  uint32_t timestamp = (uint32_t)(ts_us >> 3);

  static uint32_t next_due = 0;
  uint32_t now_ms = millis();

  if ((int32_t)(now_ms - next_due) >= 0) {
    // Output once every 20 ms
    Serial.print(timestamp);        Serial.print(", ");
    Serial.print(nitrogenPressure); Serial.print(", ");
    Serial.print(blanketTemp1);     Serial.print(", ");
    Serial.print(blanketTemp2);     Serial.print(", ");
    Serial.print(blanketstatus1);   Serial.print(", ");
    Serial.println(blanketstatus2);

    next_due = now_ms + 20;
  }
  delay(1);
}
