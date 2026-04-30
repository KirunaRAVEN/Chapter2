/* Filename:        system.h
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         System level testbench software.
 */

#include "globals.h"
#include "pinMap.h"

#include "controlBoxRX.h"
#include "sensors.h"
#include "actuators.h"
#include "comms.h"

#include "buttonAndActuatorVerification.h"
#include "firingSequence.h"

enum devices {
  CONTROLBOX,
  TC_PIPING,
  TC_CHAMBER,
  P_OX1,
  P_OX2,
  P_LINE,
  P_CHAMBER,
  P_N2,

  DEVICES_COUNT
};

class Testbench {
public:
  Testbench();

  void begin();
  void verification();
  void sequence();

private:
  ControlBoxRX controlbox;
  Thermocouple thermocouplePiping;
  Thermocouple thermocoupleChamber;
  Sensor pressureOx1, pressureOx2, pressureLine, pressureChamber, pressureN2;

  iDevice* devices[DEVICES_COUNT];

  struct normalPacket data;
};