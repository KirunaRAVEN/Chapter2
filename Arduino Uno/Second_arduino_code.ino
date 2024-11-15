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

 uint64_t timeOverflowOffset;
 uint64_t checkTimestamp;

void setup() {
  // Initialize serial communication at 1000000 baud
  Serial.begin(1000000);
  // Set the Arduino ADC clock prescaler to get faster analogRead()
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits

  ADCSRA |= bit (ADPS2);                               //  16
  //ADCSRA |= bit (ADPS0) | bit (ADPS2);                 //  32
  //ADCSRA |= bit (ADPS1) | bit (ADPS2);                 //  64
  //ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // 128
  //init checktimestamp to 0
  timeOverflowOffset = 0;
  checkTimestamp = 0;
}
 
void loop() {

  // Read the analog voltage from pin A0-A5
  int32_t nitrogenPressure = analogRead(A0);
  int32_t blanketTemp1 =     analogRead(A3);
  int32_t blanketTemp2 =     analogRead(A4);
  bool    blanketstatus1  =  digitalRead(/*some pin*/);
  bool    blanketstatus2  =  digitalRead(/*some pin*/);
  /* Convert the analog value to voltage (assuming a 5V reference voltage) 
   * float voltage1 = calVal[0]*(refADC*(nitrogenPressure/maxADC)) + calVal[3];
   * float voltage2 = calVal[1]*(refADC*(oxyPressure1/maxADC)) + calVal[4];
   * float voltage3 = calVal[2]*(refADC*(oxyPressure2/maxADC)) + calVal[5];
   * float voltage4 = (refADC*(blanketTemp1/maxADC - 0.5)*100);
   * float voltage5 = (refADC*(blanketTemp2/maxADC - 0.5)*100);
   * float voltage99 = ((refADC*(blanketTemp2/(maxADC))-0.5)*100);
   * float voltage98 = (blanketTemp2/maxADC);
   * value of resistor: 4.66k ohm
   */
//Print the voltage value to the serial monitor

  //Save timestamp
  uint64_t newTimestamp = micros();

  //Account for 32-bit counter overflow
  if (newTimestamp < checkTimestamp){
    timeOverflowOffset += 4294967295;
  }

  checkTimestamp = newTimestamp;
  newTimestamp += timeOverflowOffset;  //Arduino time in us


uint32_t timestamp = (uint32_t) (timestamp >> 3);

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


}
