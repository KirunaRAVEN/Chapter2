/* Filename:      Globals.h
 * Author:        Eemeli Mykrä
 * Date:          21.11.2022
 * Version:       V1.5 (16.05.2024)
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
  OXIDIZER_VALVE_PIN = 2,
  DUMP_VALVE_PIN = 3,
  N2FEEDING_VALVE_PIN = 4,
  TEST_MODE_LED_PIN = 6,        //In V1 the indicator LEDs use the servo connector
  REPEAT_SEQUENCE_LED_PIN = 7,  //In V1 the indicator LEDs use the servo connector
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
  REPEAT_SEQUENECE_PIN = 33,
  SW_RESET_PIN = 35
} pin_names_t;

//Pin Enumerators for Analog Pins
typedef enum {
  PRESSURE_INPUT_PIN0 = A0,
  PRESSURE_INPUT_PIN1 = A1,
  PRESSURE_INPUT_PIN2 = A2,
  LOADCELL_INPUT_PIN = A3,
  TMP36_INPUT_PIN = A4,
  PRESSURE_INPUT_PIN3 = A5,
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
  PURGING,
  FINISHED
} substate_t;

//Used mainly for debugging and for user information
const char substateStrings[7][10] = {"ALL_OFF", "IGNIT_ON", "VALVE_ON", "IGNIT_OFF", "VALVE_OFF", "PURGING", "FINISHED"};

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
const int16_t ignitionGroundOpenPassLimit = 150; // 150 / 1024 * 5V ~= 0.75V -> Open Relay
const int16_t ignitionGroundClosedPassLimit = 10; // 10 / 1024 * 5V ~= 0V -> Closed Relay

//Structure for storing the states of the test pins
struct testInput_t {
  bool startTest;
  bool repeat;
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
  /* NOT CURRENTLY USED
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

//Not used anymore since FreeRTOS isn't used
typedef enum{
  LOW_PRIORITY = 0,
  NORMAL_PRIORITY = 1,
  HIGH_PRIORITY = 2,
  CRITICAL_PRIORITY = 3
}taskPriorities_t;

//How many bytes allocated to task memory.
//Not used anymore since FreeRTOS isn't used
const int16_t taskMemoryBytes = 512;

/*
 * All delays are absolute in relation to the firing sequence start time.
 * Time is set  when firing sequence is entered: Ignition pressed + pressure met.
 * Discuss with Engine team should the timing be relative or absolute.
 * This makes no actual difference but is a preference/safety thing.
 */

//Ignition 

//How long does the ignition need to be pressed to start the ignition (ms)
const int16_t ignitionSafeTime = 1 * 1000;

//How long of a burn do we want (ms)
const int16_t burnTime = 5*1000;

//How long the igniter burns (ms) Measured from igniter test video
//Not used anymore since valve opening time is now decided based experiments,
//But still used to turn off the ignition signal
const int16_t igniterBurnLength = 900; //Outside the engine burn length was 1920 ms

//How long from ignition signal to igniter igniting (ms) Measured from igniter test videos
//Not used anymore since valve opening time is now decided based experiments
const int16_t igniterDelay = 50;

//How long from sequence start until opening valves (ms)
// The value of 150 ms was decided on 2024.05.10 based on experimental results
// They consists of valve opening delays and igniter temperature curves
const int16_t valveOnTime = 150;

//How long from sequence start to turning ignition off (ms)
const int16_t ignitionOffTime = igniterBurnLength;

//How long from sequence start until closing valves (ms)
const int16_t valveOffTime = valveOnTime + burnTime;

//How long from sequence start until the start of purging (ms)
const int16_t oxidiserEmptyTime = valveOffTime + 500;  //Placeholder value

//How long from sequence start until the end of purging (ms)
const int16_t purgingTime = oxidiserEmptyTime + 4*1000;  //Placeholder value

//How many valves the system has
const int16_t valveCount = 3; 

//How many 5V output pressure sensors does the system have
const int16_t pressureCount5V = 4;

//How many 20mA output pressure sensors does the system have
const int16_t pressureCount20mA = 0;

//What resistance is used with the current output pressure sensors. (Ohm)
const int16_t pressureResistance = 250;

//How many temperature sensors does the system have
const int16_t tempCount = 4;

//How many infra red sensors does the system have
const int16_t infraCount = 1;

//How many load cells does the system have
const int16_t loadCellCount = 1;

//How many total measurements per loop. Equal to the total count of sensors.
const int16_t sensorCount = pressureCount5V + pressureCount20mA + tempCount + infraCount;

//Structure for storing measurements with a timestamp
struct values_t {
  uint64_t timestamp;           //Time since Arduino startup in us
  uint32_t msTimestamp;         //Time since Arduino startup in ms
  uint64_t lastTimestamp;       //Used to detect overflow of 32bit microsecond timer
  uint64_t timeOverflowOffset;  //How many microseconds have been lost to 32bit overflow

  bool slowUpdated = false;     //If the less frequently sent values were updated this loop
  
  int N2FeedingPressure;        //N2 Feeding line pressure 
  int linePressure;      //Line pressure 
  int combustionPressure;      //Combustion chamber pressure 
  int N2OFeedingPressure;      //Oxidizer Feeding pressure 
  int loadCell;       //Back of the engine
  int bottleTemperature;   //Bottle temperature - Switched to TMP36 output, uses different pin
  int notConnectedTemperature;   //Injector temperature - Usually outputs NaN, not used in live_grapher_V3.py
  int nozzleTemperature;   //Nozzle temperature
  int pipingTemperature;   //Piping temperature 
  int IR;             //Plume Temperature
  
  bool dumpValveButton;             //Is dump valve button pressed (normally open)
  bool heatingBlanketButton;        //Is heating button pressed
  bool ignitionButton;              //Is ignition button pressed
  bool n2FeedingButton;             //Is N2 feeding valve button pressed (normally closed)
  bool oxidizerValveButton;         //Is the oxidizer valve button pressed (normally closed)
};

//Structure for holding the internal state of the software and control system.
struct statusValues_t{
  bool valveActive;             //Is the valve opened by the software
  bool ignitionEngagedActive;   //Is the ignition activated by the software

  int16_t mode;                 //Which mode the software is in
  int16_t subState;             //Which substate the software is in
};

//Sampling tick delay for the Sensing.senseLoop task (Ticks between excecutions)
//Not used anymore since FreeRTOS isn't used
const int16_t samplingTickDelay = 1;

//Tick delay for the Countdown.countdownLoop task (Ticks between excecutions)
//Not used anymore since FreeRTOS isn't used
const int16_t countdownTickDelay = 1;

//At what rate sensor sampling is done outside the SEQUENCE (Hz)
const int16_t limitedSampleRate = 50;


/*
 * From V1.5 Onwards the conversion from ADC values to sensor data will be performed by the Rock 4C+.
 * This will be done in combination with reading the Serial data and generating the csv file.
 */

//What resolution will the built in ADC use (bit)
const int16_t resolutionADC = 10;

//Maximum ADC value
const int16_t maxADC = pow(2, resolutionADC) - 1;

//Optimal ADC reference voltage
const float refADC = 5.00;

//Measured ADC reference voltage
const float measuredADC = 5.00;

//ADC calibration multiplier
const float calibrationADC = measuredADC / refADC;

//Mode to start in
const mode_t startMode = INIT;

//Substate to start in
const substate_t startSubstate = ALL_OFF;

//How long the system waits until starting automatic sequence (ms)
//Not used anymore
const int16_t sensorSettleTime = 2 * 1000;

//Which pressure sensors corresponds to which "location"
typedef enum{
  FEEDING_PRESSURE_OXIDIZER = 0,
  LINE_PRESSURE = 1,
  CHAMBER_PRESSURE = 2,
  FEEDING_PRESSURE_N2 = 3
}pressureSensorNames_t;

//Pressure sensor maximum pressure;
const int16_t maxPressure5V_100Bar = 100;
const int16_t maxPressure5V_25Bar = 25;

//Pressure sensor calibration data for pressure sensor 0 (Serial No: 667662) OXIDIZER FEEDING
const float pressureZero0 = -0.003;                           //Voltage
const float pressureSpan0 = 5.003;                            //Voltage
const float pressureLinearity0 = 0.12493;                     //in precentage. Not used for calibration
const float pressureLine_K0 = maxPressure5V_100Bar / pressureSpan0;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float maunalPressureOffset0 = 0;                        //How many bars of offset is seen in experimental data
const float pressureLine_B0 = maxPressure5V_100Bar - pressureLine_K0 * (pressureSpan0 + pressureZero0) - maunalPressureOffset0;

//Pressure sensor calibration data for pressure sensor 1 (Serial No: 1073014) LINE
const float pressureZero1 = 0.01;                             //Voltage
const float pressureSpan1 = 4.997;                            //Voltage
const float pressureLinearity1 = 0.10154;                     //in percent. Not used for calibration
const float pressureLine_K1 = maxPressure5V_100Bar / pressureSpan1;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float maunalPressureOffset1 = 0;                        //How many bars of offset is seen in experimental data
const float pressureLine_B1 = maxPressure5V_100Bar - pressureLine_K1 * (pressureSpan1 + pressureZero1) - maunalPressureOffset1;

//Pressure sensor calibration data for pressure sensor 2 (Serial No: 1040112) CHAMBER
const float pressureZero2 = 0.000;                            //Voltage
const float pressureSpan2 = 4.996;                            //Voltage
const float pressureLinearity2 = 0.03146;                     //in precentage. Not used for calibration
const float pressureLine_K2 = maxPressure5V_25Bar / pressureSpan2;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float maunalPressureOffset2 = 0;                        //How many bars of offset is seen in experimental data
const float pressureLine_B2 = maxPressure5V_25Bar - pressureLine_K2 * (pressureSpan2 + pressureZero2) - maunalPressureOffset2;

//Pressure sensor calibration data for pressure sensor 3 (Serial No: 1086286) NITROGEN FEEDING
const float pressureZero3 = -0.005;                           //Voltage
const float pressureSpan3 = 5.007;                            //Voltage
const float pressureLinearity3 = 0.03709;                     //in precentage. Not used for calibration
const float pressureLine_K3 = maxPressure5V_100Bar / pressureSpan3;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float maunalPressureOffset3 = 0;                        //How many bars of offset is seen in experimental data
const float pressureLine_B3 = maxPressure5V_100Bar - pressureLine_K3 * (pressureSpan3 + pressureZero3) - maunalPressureOffset3;

//---NOT ATTACHED---
//Pressure sensor calibration data for pressure sensor 4 (Serial No: 1086284) OXIDIZER FEEDING BACKUP
const float pressureZero4 = -0.005;                           //Voltage
const float pressureSpan4 = 5.035;                            //Voltage
const float pressureLinearity4 = 0.14196;                     //in precentage. Not used for calibration
const float pressureLine_K4 = maxPressure5V_100Bar / pressureSpan4;  //Slope of the calibrated data
//Zero offset of the calibrated data
const float maunalPressureOffset4 = 0;                        //How many bars of offset is seen in experimental data
const float pressureLine_B4 = maxPressure5V_100Bar - pressureLine_K4 * (pressureSpan4 + pressureZero4) - maunalPressureOffset4;

//Arrays of 5V pressure sensors calibration data
const float pressureCalibration_K[pressureCount5V] = {pressureLine_K0, pressureLine_K1, pressureLine_K2, pressureLine_K3};
const float pressureCalibration_B[pressureCount5V] = {pressureLine_B0, pressureLine_B1, pressureLine_B2, pressureLine_B3};

//Current (20mA) pressure sensor minimum and maximum values
const int16_t minPressureCurrent = 4;     //(mA)
const int16_t maxPressureCurrent = 20;    //(mA)
const float maxcombustionPressure0mA = 172.3689; //(bar)

//Calibration data for the 20mA output pressure sensors
//How many measurements are performed each time the 20mA sensor is used
const int16_t pressureAverageCount20mA = 1;
//Data is incomplete, based only on zero point offset
const float pressureZero20mA = 0.5;         //Bar --- TO CHANGE. Not sure why 0.5 See: https://www.farnell.com/datasheets/3626069.pdf
const float pressureSpan20mA = 172.3689;    //Bar
//Slope of the calibrated data
const float pressureLine_K20mA = maxcombustionPressure0mA / pressureSpan20mA;
//Zero offset of the calibrated data
const float pressureLine_B20mA = maxcombustionPressure0mA - pressureLine_K20mA * (pressureSpan20mA + pressureZero20mA);

//At what rate the temperature data is gathered (hz)
const uint16_t tempSensorRate = 10;

//IR sensor minimum and maximum values
const int16_t minIR = -50;
const int16_t maxIR = 1030;

//How many measurements are taken per value to reduce noise on the IR sensor
const int16_t IrAverageCount = 4;

//TMP36 ranges
const int16_t minTMP36 = -40;
const int16_t maxTMP36 = 125;

//How many measurements are taken per value to reduce noise on the TMP36
const int16_t TMP36AverageCount = 4;

//Load cell minimum and maximum values
const int16_t minLoad = 0;
const int16_t maxLoad = 250 * 4.44822;  //Conversion to Newtons

//Load cell calibration data.
const float loadCellZeroPointVoltage = 0.5; //Placeholder value
const float loadCellSpan = 4.0; //Placeholder value

const float loadCellLine_K = maxLoad / loadCellSpan; //Slope of the calibrated data
//Zero offset of the calibrated data
const float loadCellLine_B = maxLoad - loadCellLine_K * (loadCellSpan + loadCellZeroPointVoltage);

//How many measurements are taken per value to reduce noise on the load cell
const int16_t loadCellAverageCount = 4;

//Which thermocouple corresponds to which "location"
typedef enum{
  NOT_CONNECTED0 = 0,
  NOT_CONNECTED_1 = 1,
  NOZZLE_TC = 2,
  PIPING_TC = 3
}tempSensorNames_t;

/*
 * Change the names of automation limit variables here to better
 * represent when they are used.
 */

//Minimum pressure required to start the firing sequence (bar)
const int16_t minimumFiringPressure = -1;  //Set to -1 bar to always allow the firing

//Pressure sensor 0 closing pressure. Not used in current design. Test is timed
//const int16_t closeN2FeedingPressure = 2;

//At what temperature are the heating blankets turned off
//Software has no ability to control the heating blanket in the current design.
const int16_t feedingTemperatureLimit = 35;  //Placeholder value

//Buzzer warning length (ms)
const int16_t buzzerOnTime = 1 * 500;

//Baudrate for serial communications
const uint32_t serialBaudNormal = 1000000;

// Switching baudrate not utilized currently
//const uint32_t serialBaudFast = 1000000;

//Fault thresholds for initiating an emergency stop
const int16_t successivePasses = 12; //N successive passes lead to threshold trigger

const int16_t N2OFeedingPressureThreshold = 70;    //Needs confirmation
const int16_t chamberPressureThreshold = 20;    //Needs confirmation
const int16_t casingTemperatureThreshold = 800; //Placeholder Value
//nst int16_t More Thresholds to be added

//Warning thresholds
const int16_t N2OFeedingPressureWarning = 65;

//Indexes of all the possible messages to send
typedef enum{
  MSG_TEST_SEQUENCE_START = 1,
  MSG_NO_BUTTONS = 2,
  MSG_RELEASE_OX = 3,
  MSG_RELEASE_IGN = 4,
  MSG_RELEASE_HEAT = 5,
  MSG_PASS = 6,
  MSG_FAIL = 7,
  MSG_IGN_24_OFF = 8,
  MSG_IGN_GND_OFF = 9,
  MSG_IGN_SW_OFF = 10,
  MSG_HEAT_OFF = 11,
  MSG_OX_OFF = 12,
  MSG_HEAT_ON_START = 13,
  MSG_HEAT_BUTTON = 14,
  MSG_HEAT_ON_RESULT = 15,
  MSG_HEAT_RELEASE = 16,
  MSG_OX_ON_START = 17,
  MSG_OX_BUTTON = 18,
  MSG_OX_ON_RESULT = 19,
  MSG_OX_RELEASE = 20,
  MSG_IGN_ON_START = 21,
  MSG_IGN_BUTTON = 22,
  MSG_IGN_ON_24_RESULT = 23,
  MSG_IGN_ON_GND_RESULT = 24,
  MSG_IGN_ON_SW_RESULT = 25,
  MSG_IGN_ON_RELEASE = 26,
  MSG_TEST_FINISH = 27,
  MSG_TEST_PASSED = 28,
  MSG_TEST_FAILED = 29,
  MSG_TEST_ENDING = 30,
  MSG_DUMP_WARNING = 31,
  MSG_N2_FEED_WARNING = 32,
  MSG_OX_FEED_WARNING = 33
  }messageIndices_t;

//Maximum length of the message buffer;
const uint16_t msgBufferSize = 16;



//Other stuff to come. Add any constants here instead of in each separate file.

#endif