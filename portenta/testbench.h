/* Filename:        system.h
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         System level testbench software.
 */

#ifndef TESTBENCH_H
#define TESTBENCH_H

#include "globals.h"
#include "pinMap.h"

#include "iDevice.h"
#include "controlBoxRX.h"
#include "sensors.h"
#include "actuators.h"

#include "comms.h"

// #include "buttonAndActuatorVerification.h"
// #include "firingSequence.h"

enum device_list {
  D_CONTROLBOX,
  D_T_PIPING,
  D_T_CHAMBER,
  D_P_OX1,
  D_P_OX2,
  D_P_LINE,
  D_P_CHAMBER,
  D_P_N2,
  D_T_OX1,
  D_T_OX2,
  D_T_PLUME,
  D_F_LOADCELL,
  // D_SW_TEST,
  
  D_R_IGNRELAY,
  D_R_IGNARMED,
  D_V_OX1,
  D_V_OX2,
  D_V_N2,
  D_H_OX1,
  D_H_OX2,
  D_V_DUMP,
  D_R_LIGHT,
  D_R_SIREN,
  D_R_CAMERA,
  // D_L_TEST,
  // D_L_ERROR,
  D_DEVICES_COUNT
};

class Testbench {
public:
  Testbench();

  void begin();
  void update();
  void read();
  void send();

  void normalCycle();
  void fastCycle(); //Only chamber pressure

  void verification();
  void sequence();

private:
  // Sensors
  ControlBoxRX controlbox;
  Thermocouple thermocouplePiping;
  Thermocouple thermocoupleChamber;
  AnalogSensor pressureOx1, pressureOx2, pressureLine, pressureChamber, pressureN2, temperatureOx1, temperatureOx2, temperaturePlume;
  Loadcell loadcell;
  // Actuators
  DigitalActuator ignitionRelay, ignitionArmed, valveOx1, valveOx2, valveN2, heaterOx1, heaterOx2;
  Servovalve valveDump;
  DigitalActuator light, siren, camera;
  // Devices
  iDevice* devices[D_DEVICES_COUNT];
  
  DigitalSensor testSwitch;
  DigitalActuator testLed, errorLed;

  // Comms
  Comms ethernet;
  struct normalPacket dataPacket;
};

#endif /* TESTBENCH_H */