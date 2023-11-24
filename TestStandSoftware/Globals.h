/* Filename:      Globals.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 *
 * Purpose:       Header file for the Globals <<environmental>> object containing 
 *                global constants and user defined types. 
 *                This header is to be included in any file that requires
 *                the datatypes or values defined here.
 */

#include <stdint.h>
#include <Arduino.h>

//Prevent multiple definitions with the if statement
#ifndef GLOBALS_H
#define GLOBALS_H

//Pin Enumerators - Add Pins for different functionallities here
typedef enum {
  MAIN_VALVE_PIN = 2,
  DUMP_VALVE_PIN = 3,
  FEEDING_VALVE_PIN = 4,
  TEST_MODE_LED_PIN = 6,        //In V1 the indicator LEDs use the servo connector
  FORCED_SEQUENCE_LED_PIN = 7,  //In V1 the indicator LEDs use the servo connector
  IGNITION_SENSE_PIN = 8,
  HEATING_SENSE_PIN = 9,
  DUMP_VALVE_BUTTON_PIN = 10,
  IGNITER_CONTROL_PIN = 12,
  BUZZER_CONTROL_PIN = 19,
  MAIN_VALVE_BUTTON_PIN = 20,
  FEEDING_VALVE_BUTTON_PIN = 21,
  AUTO_TEST_START_PIN = 23,
  THERMOCOUPLE_CS_PIN0 = 24, 
  THERMOCOUPLE_CS_PIN1 = 26,
  IGN_SW_RELAY_TEST_PIN = 27,
  THERMOCOUPLE_CS_PIN2 = 28, 
  IGN_GND_RELAY_TEST_DRIVE_PIN = 29,
  THERMOCOUPLE_CS_PIN3 = 30,
  MAIN_VALVE_TEST_PIN = 31,
  FORCED_SEQUENECE_PIN = 33,
  SW_RESET_PIN = 35
} pin_names_t;

//Pin Enumerators for Analog Pins
typedef enum {
  PRESSURE_INPUT_PIN0 = A0,
  PRESSURE_INPUT_PIN1 = A1,
  PRESSURE_INPUT_PIN2 = A2,
  LOADCELL_INPUT_PIN = A3,
  TMP36_INPUT_PIN = A4,
  INFRARED_INPUT_PIN = A6,
  IGN_GND_RELAY_TEST_MEASURE_PIN = A15
}pin_names_analog_t;

//Enumeration for the different modes of the system
typedef enum {
  INIT,
  TEST,
  WAIT,
  SEQUENCE,
  SAFE,
  SHUTDOWN
} mode_t;

//Used mainly for debugging and for user information
const char modeStrings[6][10] = {"INIT", "TEST", "WAIT", "SEQUENCE", "SAFE", "SHUTDOWN"};

//Enumeration for the different substates of the firing sequence
typedef enum {
  ALL_OFF,
  IGNIT_ON,
  VALVE_ON,
  IGNIT_OFF,
  VALVE_OFF,
  FINISHED
} substate_t;

//Used mainly for debugging and for user information
const char substateStrings[6][10] = {"ALL_OFF", "IGNIT_ON", "VALVE_ON", "IGNIT_OFF", "VALVE_OFF", "FINISHED"};

//Enumeration for the different states of the automated testing sequence
typedef enum {
  TEST_START,
  OFF_STATE_BUTTON,
  OFF_STATE_TEST,
  HEAT_ON_BUTTON,
  HEAT_ON_TEST,
  HEAT_RELEASE,
  VALVE_ON_BUTTON,
  VALVE_ON_TEST,
  VALVE_RELEASE,
  IGN_ON_BUTTON,
  IGN_ON_TEST,
  IGN_RELEASE,
  TEST_END
} verificationState_t;

//How long are the actuators let to settle during testing sequence (ms)
const int16_t actuatorTestSettleTime = 250;

//How long does the software wait at the end of the verification sequence (s)
const int16_t verificationEndCount = 10;

//Which ADC readings counts as a passes for the ignition ground relay tests
const int16_t ignitionGroundOpenPassLimit = 190; // 190 / 1024 * 5V ~= 1V -> Open Relay
const int16_t ignitionGroundClosedPassLimit = 5; // 5 / 1024 * 5V ~= 0V -> Closed Relay


//Structure for storing the states of the test pins
struct testInput_t {
  bool startTest;
  bool forced;
  bool resetSW;
  uint16_t IGN_GND_IN;
  bool IGN_SW_IN;
  bool MAIN_VALVE_IN;
};

//How many (non-pullup) test input pins does the system have
const int16_t testInputCount = 2;

//How many pullup input test pins does the system have
const int16_t testPullupCount = 3;

//How many test output pins does the system have
const int16_t testOutputCount = 1;

//How many analog input test pins does the system have
const int16_t testAnalogCount = 1;

//Structure for storing the timings of buzzer message patterns
struct buzzerTiming_t{
  /*
   * First index is number of cylces (0=indefinite)
   * Rest are timing in milliseconds of beign ON and OFF repeating.
   */
  int16_t warning[3] = {0, 750, 250};
  int16_t reset[2] = {1, 1000};
  int16_t test[7] = {1, 200, 200, 200, 200, 200, 200};
};

//Enumeration of buzzer patterns
typedef enum{
  BUZZER_OFF,
  BUZZER_WARNING,
  BUZZER_RESET,
  BUZZER_TEST
}buzzerPattern_t;

typedef enum{
  LOW_PRIORITY = 0,
  NORMAL_PRIORITY = 1,
  HIGH_PRIORITY = 2,
  CRITICAL_PRIORITY = 3
}taskPriorities_t

//How many bytes allocated to task memory.
const int16_t taskMemoryBytes = 512;

/*
 * All delays are absolute in relation to the firing sequence start time.
 * Time is set  when firing sequence is entered: Ignition pressed + pressure met.
 * Discuss with Engine team should the timing be relative or absolute.
 * This makes no actual difference but is a preference/safety thing.
 */

//Ignition 

//Consider changing this to use math to make readin it

//How long of a burn do we want (ms)
const int16_t burnTime = 7*1000;

//How long the igniter burns (ms) Measured from igniter test video
const int16_t igniterBurnLength = 900; //Outside the engine burn length was 1920 ms

//How long from ignition signal to igniter igniting (ms) Measured from igniter test videoS
const int16_t igniterDelay = 50;

//How long does the ignition need to be pressed to start the ignition (ms)
const int16_t ignitionSafeTime = 1 * 1000;

//How long from sequence start until opening valves (ms)
const int16_t valveOnTime = igniterDelay + igniterBurnLength / 2;

//How long from sequence start to turning ignition off (ms)
const int16_t ignitionOffTime = igniterBurnLength;

//How long from sequence start until closing valves (ms)
const int16_t valveOffTime = valveOnTime + burnTime;

//How long from sequence start until calling the test finished (ms)
const int16_t cooldownTime = valveOffTime + 10 * 1000;  //Placeholder value

//How many valves the system has
const int16_t valveCount = 3; 

//How many 5V output pressure sensors does the system have
const int16_t pressureCount5V = 3;

//What resistance is used with the current output pressure sensors. (Ohm)
const int16_t pressureResistance = 250;

//How many temperature sensors does the system have
const int16_t tempCount = 4;

//How many infra red sensors does the system have
const int16_t infraCount = 1;

//How many load cells does the system have
const int16_t loadCellCount = 1;

//How many total measurements per loop. Equal to the total count of sensors.
const int16_t sensorCount = pressureCount5V  + tempCount + infraCount;

//Structure for storing measurements with a timestamp
struct values_t {
  uint32_t timestamp;   //Time since Arduino startup
  float pressure0;      //Feeding line
  float pressure1;      //Oxidizer line
  float pressure2;      //Combustion chamber
  float loadCell;      //Back of the engine
  float temperature0;   //Bottle temperature - Switched to TMP36 output, uses different pin
  float temperature1;   //Injector temperature - Usually outputs NaN, not used in live_grapher_V3.py
  float temperature2;   //Nozzle temperature
  float temperature3;   //Ambient temperature
  float IR;             //Plume Temperature
  
  bool dumpValveButton;             //Is dump valve button pressed (normally open)
  bool heatingBlanketButton;        //Is heating button pressed
  bool ignitionButton;              //Is ignition button pressed
  bool feedingButton;               //Is feeding valve button pressed (normally closed)
  bool mainValveButton;             //Is the main oxidizer valve button pressed (normally closed)

};

//Structure for holding the internal state of the software and control system.
struct statusValues_t{
  bool valveActive;             //Is the valve opened by the software
  bool ignitionEngagedActive;   //Is the ignition activated by the software

  int16_t mode;                 //Which mode the software is in
  int16_t subState;             //Which substate the software is in
};

//Sampling tick delay for the Sensing.senseLoop task (Ticks between excecutions)
const int16_t samplingTickDelay = 1;

//Tick delay for the Countdown.countdownLoop task (Ticks between excecutions)
const int16_t countdownTickDelay = 1;

//What resolution will the built in ADC use (bit)
const int16_t resolutionADC = 10;

//Maximum ADC value
const int16_t maxADC = pow(2, resolutionADC) - 1;

//Optimal ADC reference voltage
const float refADC = 5.00;

//Measured ADC reference voltage
const float measuredADC = 4.98;

//ADC calibration multiplier
const float calibrationADC = measuredADC / refADC;

//Mode to start in
const mode_t startMode = INIT;

//Substate to start in
const substate_t startSubstate = ALL_OFF;

//How long the system waits until starting automatic sequence (ms)
const int16_t sensorSettleTime = 2 * 1000;

//Which pressure sensors corresponds to which "location"
typedef enum{
  FEEDING_PRESSURE = 0,
  LINE_PRESSURE = 1,
  CHAMBER_PRESSURE = 2
}pressureSensorNames_t;

//Pressure sensor maximum pressure;
const int16_t maxPressure5V = 100;

//Pressure sensor calibration data for pressure sensor 0 (Serial No: 667662) FEEDING
const float pressureZero0 = -0.003;                           //Voltage
const float pressureSpan0 = 5.003;                            //Voltage
const float pressureLinearity0 = 0.12493;                     //in precentage. Not used for calibration
const float pressureLine_K0 = maxPressure5V / pressureSpan0;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float pressureLine_B0 = maxPressure5V - pressureLine_K0 * (pressureSpan0 + pressureZero0);

//Pressure sensor calibration data for pressure sensor 1 (Serial No: 1073014) LINE
const float pressureZero1 = 0.01;                             //Voltage
const float pressureSpan1 = 4.997;                            //Voltage
const float pressureLinearity1 = 0.10154;                     //in percent. Not used for calibration
const float pressureLine_K1 = maxPressure5V / pressureSpan1;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float pressureLine_B1 = maxPressure5V - pressureLine_K1 * (pressureSpan1 + pressureZero1);

//Pressure sensor calibration data for pressure sensor 2 (Serial No: 1073012) CHAMBER
const float pressureZero2 = 0.001;                            //Voltage
const float pressureSpan2 = 5.002;                            //Voltage
const float pressureLinearity2 = 0.11859;                     //in precentage. Not used for calibration
const float pressureLine_K2 = maxPressure5V / pressureSpan2;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float pressureLine_B2 = maxPressure5V - pressureLine_K2 * (pressureSpan2 + pressureZero2);

//Arrays of 5V pressure sensors calibration data
const float pressureCalibration_K[pressureCount5V] = {pressureLine_K0, pressureLine_K1, pressureLine_K2};
const float pressureCalibration_B[pressureCount5V] = {pressureLine_B0, pressureLine_B1, pressureLine_B2};

//IR sensor minimum and maximum values
const int16_t minIR = -50;
const int16_t maxIR = 1030;

//TMP36 ranges
const int16_t minTMP36 = -40;
const int16_t maxTMP36 = 125;

//Load cell minimum and maximum values
const int16_t minLoad = 0;
const int16_t maxLoad = 250 * 4.44822;  //Conversion to Newtons

//Load cell calibration data.
const float loadCellZeroPointVoltage = 0.432; //Placeholder value
const float loadCellSpan = 4.0; //Placeholder value

const float loadCellLine_K = maxLoad / loadCellSpan; //Slope of the calibrated data
//Zero offset of the calibrated data
const float loadCellLine_B = maxLoad - loadCellLine_K * (loadCellSpan + loadCellZeroPointVoltage);

//How many measurements are taken per value to reduce noise
const int16_t loadCellAverageCount = 4;

//Which thermocouple corresponds to which "location"
typedef enum{
  NOT_CONNECTED0 = 0,
  NOT_CONNECTED_1 = 1,
  NOZZLE_TC = 2,
  AMBIENT_TC = 3
}tempSensorNames_t;

/*
 * Change the names of automation limit variables here to better
 * represent when they are used.
 */

//Minimum pressure required to start the firing sequence (bar)
const int16_t minimumFiringPressure = 0;  //Set to 0 bar to always allow the firing

//Pressure sensor 0 closing pressure. Not used in current design. Test is timed
//const int16_t closePressure0 = 2;

//At what temperature are the heating blankets turned off
//Software has no ability to control the heating blanket in the current design.
const int16_t feedingTemperatureLimit = 35;  //Placeholder value

//Buzzer warning length (ms)
const int16_t buzzerOnTime = 1 * 500;

//Baudrate for serial communications
const uint32_t serialBaud = 115200;

//Fault thresholds for initiating an emergency stop
const int16_t successivePasses = 5; //N successive passes lead to threshold trigger

const int16_t feedingPressureThreshold = 65;    //Needs confirmation
const int16_t chamberPressureThreshold = 25;    //Needs confirmation
const int16_t casingTemperatureThreshold = 800; //Placeholder Value
//nst int16_t More Thresholds to be added

//Warning thresholds
const int16_t feedingPressureWarning = 60;

//Other stuff to come. Add any constants here instead of in each separate file.

#endif