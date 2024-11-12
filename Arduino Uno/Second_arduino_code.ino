// Authors: Christoffer Brun, Ivar Tylén
// Date: 11/11/2024
// Version: 1.1
// Purpose: Reads and then sends the values read on analog pins 0-4
// Todo: 

//float refADC = 5; //assuming 5v reference, can change based on measurements
//float maxADC = 1023; // 1023, maximum value for a 10bit digital converter.
// calibration values for the pressure sensors 
// in the order nitrogen k, k2, k3, nitrogen b, b2, b3, placeholders currently. 
//float calVal[6] = {1 , 1, 1, 0, 0, 0}; 
//float nitrogenPressure = 0 , oxyPressure1 = 0, oxyPressure2 = 0, blanketTemp1 = 0, blanketTemp2 =0;  

void setup() {
  // Initialize serial communication at 1000000 baud
  Serial.begin(1000000);
  // Set the Arduino ADC clock prescaler to get faster analogRead()
  ADCSRA &= ~(bit (ADPS0) | bit (ADPS1) | bit (ADPS2)); // clear prescaler bits
 
  ADCSRA |= bit (ADPS2);                               //  16
  //ADCSRA |= bit (ADPS0) | bit (ADPS2);                 //  32
  //ADCSRA |= bit (ADPS1) | bit (ADPS2);                 //  64
  //ADCSRA |= bit (ADPS0) | bit (ADPS1) | bit (ADPS2);   // 128
}
 
void loop() {

  // Read the analog voltage from pin A0-A5
  float nitrogenPressure = analogRead(A0);
  float oxyPressure1 =     analogRead(A1);
  float oxyPressure2 =     analogRead(A2);
  float blanketTemp1 =     analogRead(A3);
  float blanketTemp2 =     analogRead(A4);
  float unknown =          analogRead(A5);

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
  Serial.print(micros());
  Serial.print(", ");

//Nitrogen pressure
  Serial.print(nitrogenPressure);
  Serial.print(", ");

//Oxygen 1 pressure
 Serial.print(oxyPressure1);
 Serial.print(", ");

//Oxygen 2 pressure
 Serial.print(oxyPressure2);
 Serial.print(", ");

 //Oxygen tank 1 temp
  Serial.print(blanketTemp1);
 Serial.print(", ");

 // Oxygen tank 2 temp
 Serial.println(blanketTemp2);
 //Serial.println(", ");
}
