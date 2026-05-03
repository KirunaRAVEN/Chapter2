#include "Arduino.h"
/* Filename:        system.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         System level testbench software.
 */

#include "testbench.h"

Testbench::Testbench()
  : controlbox("ControlBox RX", &Serial2),
    thermocouplePiping("TC_Pipe", PIN_SPI_CS_TC_PIPING),
    thermocoupleChamber("TC_Chamber", PIN_SPI_CS_TC_CHAMBER),
    pressureOx1("P_Ox1", PIN_OXIDIZER1_PRESSURE),
    pressureOx2("P_Ox2", PIN_OXIDIZER2_PRESSURE),
    pressureLine("P_Line", PIN_LINE_PRESSURE),
    pressureChamber("P_Chamber", PIN_CHAMBER_PRESSURE),
    pressureN2("P_N2", PIN_NITROGEN_PRESSURE),
    temperatureOx1("T_Ox1", PIN_OXIDIZER1_TEMP),
    temperatureOx2("T_Ox2", PIN_OXIDIZER2_TEMP),
    temperaturePlume("T_Plume", PIN_PLUME_TEMP),
    loadcell("Loadcell", PIN_LOADCELL_DATA, PIN_LOADCELL_CLOCK),

    ignitionRelay("Ignition relay", PIN_IGNITION_RELAY),
    ignitionArmed("Ignition armed", PIN_IGNITION_ARM),
    valveOx1("Valve Ox1", PIN_OXIDIZER1_RELAY),
    valveOx2("Valve Ox2", PIN_OXIDIZER2_RELAY),
    valveN2("Valve N2", PIN_NITROGEN_RELAY),
    heaterOx1("Heater Ox1", PIN_HEATING1_RELAY),
    heaterOx2("Heater Ox2", PIN_HEATING2_RELAY),
    valveDump("Dump valve", PIN_DUMP_PIN, DUMP_OPEN, DUMP_CLOSE),

    light("Warning light", PIN_LIGHT_SIGNAL),
    siren("Warning siren", PIN_SIREN_SIGNAL),
    camera("Camera signal", PIN_HIGH_SPEED_SIGNAL),

    testSwitch("Test Switch", PIN_TEST_MODE_BUTTON),
    testLed("Test LED", PIN_TEST_LED_SIGNAL),
    errorLed("Error LED", PIN_ERROR_LED_SIGNAL)
{
    devices[D_CONTROLBOX] = &controlbox;
    devices[D_T_PIPING] = &thermocouplePiping;
    devices[D_T_CHAMBER] = &thermocoupleChamber;
    devices[D_P_OX1] = &pressureOx1;
    devices[D_P_OX2] = &pressureOx2;
    devices[D_P_LINE] = &pressureLine;
    devices[D_P_CHAMBER] = &pressureChamber;
    devices[D_P_N2] = &pressureN2;
    devices[D_T_OX1] = &temperatureOx1;
    devices[D_T_OX2] = &temperatureOx2;
    devices[D_T_PLUME] = &temperaturePlume;
    devices[D_F_LOADCELL] = &loadcell;

    devices[D_R_IGNRELAY] = &ignitionRelay;
    devices[D_R_IGNARMED] = &ignitionArmed;
    devices[D_V_OX1] = &valveOx1;
    devices[D_V_OX2] = &valveOx2;
    devices[D_V_N2] = &valveN2;
    devices[D_H_OX1] = &heaterOx1;
    devices[D_H_OX2] = &heaterOx2;
    devices[D_V_DUMP] = &valveDump;

    devices[D_R_LIGHT] = &light;
    devices[D_R_SIREN] = &siren;
    devices[D_R_CAMERA] = &camera;

    // devices[D_SW_TEST] = &testSwitch;
    // devices[D_L_TEST] = &testLed;
    // devices[D_L_ERROR] = &errorLed;
}
void Testbench::begin()
{
  for(auto device: devices) {
    device->begin();
  }

  testSwitch.begin();
  testLed.begin();
  errorLed.begin();

  ethernet.begin();
}
void Testbench::update()
{
  for(auto device: devices) {
    device->update();
  }
}
int Testbench::read()
{
  static ControlBoxStateMessage message = controlbox.getMessage();

  dataPacket.data.timestamp           = millis();
  dataPacket.data.N2OFeedingPressure1 = devices[D_P_OX1]->read();
  dataPacket.data.N2OFeedingPressure2 = devices[D_P_OX2]->read();
  dataPacket.data.linePressure        = devices[D_P_LINE]->read();
  dataPacket.data.chamberPressure     = devices[D_P_CHAMBER]->read();
  dataPacket.data.N2FeedingPressure   = devices[D_P_N2]->read();

  dataPacket.data.bottleTemperature1  = devices[D_T_OX1]->read();
  dataPacket.data.bottleTemperature2  = devices[D_T_OX2]->read();
  dataPacket.data.plumeTemperature    = devices[D_T_PLUME]->read();
  dataPacket.data.pipingTemperature   = devices[D_T_PIPING]->read();
  dataPacket.data.chamberTemperature  = devices[D_T_CHAMBER]->read();

  dataPacket.data.loadcellReading     = devices[D_F_LOADCELL]->read();

  // dataPacket.state.mode=;
  // dataPacket.state.subState=;
  // dataPacket.state.message=;
  dataPacket.state.dumpValveButton    = message.dumpButton;
  dataPacket.state.heatingBlanketButton1=message.heating1Switch;
  dataPacket.state.heatingBlanketButton2=message.heating2Switch;
  dataPacket.state.ignitionButton     = message.ignitionButton;
  dataPacket.state.N2ValveButton      = message.nitrogenButton;
  dataPacket.state.N2OValveButton     = message.oxidizerButton;
  // dataPacket.state.valveActive=;
  // dataPacket.state.ignitionEngagedActive=;

  return 0;
}
void Testbench::send()
{
  ethernet.send(NORMAL_PACKET, (void *) &dataPacket, sizeof(struct normalPacket));
}
void Testbench::normalCycle()
{
  static unsigned long int slowTime = millis();
  if(millis()-slowTime > MAIN_LOOP_PERIOD)
  {
    update();
    read();
    send();
  }
}
void Testbench::fastCycle()
{
  static unsigned long int fastTime = micros();
  if(millis()-fastTime > FAST_LOOP_PERIOD)
  {
    devices[D_P_CHAMBER]->update();
    int sample = devices[D_P_CHAMBER]->read();
    ethernet.send(FAST_PACKET, (void *) &sample, sizeof(int));
  }
}
void Testbench::verification()
{

}
void Testbench::sequence()
{

}