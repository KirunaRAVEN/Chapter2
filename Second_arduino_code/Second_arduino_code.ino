// Authors: Christoffer Brun, Ivar Tylén, Alexander Bülow
// Date: 12/11/2024
// Version: 1.1
// Purpose: Reads and then sends the values read on analog pins 0-4
// Todo: 

//float refADC = 5; //assuming 5v reference, can change based on measurements
//float maxADC = 1023; // 1023, maximum value for a 10bit digital converter.
// calibration values for the pressure sensors 
// in the order nitrogen k, k2, k3, nitrogen b, b2, b3, placeholders currently. 
//float calVal[6] = {1 , 1, 1, 0, 0, 0}; 
//float nitrogenPressure = 0 , oxyPressure1 = 0, oxyPressure2 = 0, blanketTemp1 = 0, blanketTemp2 =0;  

// ensuring that the compiler does not add any padding 
struct __attribute__((packed)) DataPacket{
  uint32_t TimeStamp; // Allows for about 9 hours of data recording
  uint16_t nitrogenPressure; // analogRead only outputs 10 bits so 16 bits is enough
  uint16_t blanketTemp1; // same reasoning as before
  uint16_t blanketTemp2; // same reasoning as before
  uint8_t blanketstatus1; // minimal addressable size
  uint8_t blanketstatus2; // minimal addressable size
}Packet;

//Global variables
 uint64_t timeOverflowOffset = 0;
 uint32_t checkTimestamp = 0;
 const uint8_t START_MARKER = 0x7E;
 const uint8_t END_MARKER = 0x7F;
 const uint8_t ESCAPE_BYTE = 0x7D;
 const uint8_t ESCAPE_XOR = 0x20;

// Allows us 
void sendData(const uint8_t* data, size_t length){
  Serial.write(START_MARKER);
  for (size_t i = 0; i < length; i++){
    uint8_t byte = data[i];
    if (byte == START_MARKER || byte == END_MARKER || byte == ESCAPE_BYTE){
      Serial.write(ESCAPE_BYTE);
      Serial.write(byte ^ ESCAPE_XOR);
    }
    else {
      Serial.write(byte);
    }
  }
  Serial.write(END_MARKER);
}

void setup() {
  // Initialize serial communication at 115200 baud
  Serial.begin(115200);
  // Set the Arduino ADC clock prescaler to get faster analogRead()
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits
  ADCSRA |= bit (ADPS2);                               //  16

  // configuring the pins for input only 
  pinMode(13, INPUT);
  pinMode(8, INPUT);

}
 
void loop() {
  uint32_t loopStart = millis();
  uint32_t currentTime = micros(); 

  //Handles micros overflow, which will happen every 71 or so minutes
  if (checkTimestamp > currentTime){
    timeOverflowOffset += 4294967296ULL;
  }
  checkTimestamp = currentTime;
  uint64_t fullTime = currentTime + timeOverflowOffset;
  // updating the data within the struct
  Packet.TimeStamp = (uint32_t)(fullTime >> 3); 
  Packet.nitrogenPressure = analogRead(A0);
  Packet.blanketTemp1 = analogRead(A3);
  Packet.blanketTemp2 = analogRead(A4);
  Packet.blanketstatus1 = digitalRead(13); 
  Packet.blanketstatus2 = digitalRead(8);

  // writes the struct via serial in sequences of 8 bits
  sendData((uint8_t*)&Packet, sizeof(Packet));

  // makes sure we have a update frequency of 50 Hz or less
  uint32_t elapsed = millis() - loopStart;

  if (elapsed < 20) {
  delay(20 - elapsed);
  }


/*
  uint64_t t1= millis();
  // Read the analog voltage from pin A0-A5
  int32_t nitrogenPressure = analogRead(A0);
  int32_t blanketTemp1 =     analogRead(A3);
  int32_t blanketTemp2 =     analogRead(A4);
  bool    blanketstatus1  =  digitalRead(13);
  bool    blanketstatus2  =  digitalRead(8);

  //Save timestamp
  uint64_t newTimestamp = micros(); //Timestamp at start of loop

  //Account for 32-bit counter overflow
  if (newTimestamp < checkTimestamp){
    timeOverflowOffset += 4294967295;
  }

  checkTimestamp = newTimestamp;
  newTimestamp += timeOverflowOffset;  //Arduino time in us

  uint32_t timestamp = (uint32_t) (newTimestamp >> 3); 
  //Print the voltage value to the serial monitor
    Serial.print(timestamp);
    Serial.print(", ");

  //Nitrogen pressure
    Serial.print(nitrogenPressure);
    Serial.print(", ");

  //Oxygen tank 1 temp
    Serial.print(blanketTemp1);
  Serial.print(", ");

  // Oxygen tank 2 temp
  Serial.print(blanketTemp2);
  Serial.print(", ");

  // Oxygen status 1
  Serial.print(blanketstatus1);
  Serial.print(", ");

  // Oxygen status 2
  Serial.println(blanketstatus2);

  uint64_t t2 = millis(); // Timestamp at end of code.
  delay(20 - (t2 - t1)); // ensures we only send data with 20ms intervals. */
}
