/* Filename:        system.cpp
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         System level testbench software.
 */

Testbench::Testbench()
  : controlbox(&Serial2),
    thermocouplePiping("TC_Pipe", CS_PIN_1),
    thermocoupleChamber("TC_Chamber", CS_PIN_2),
    pressureOx1("P_Ox1", PIN_OX1),
    pressureOx2("P_Ox2", PIN_OX2),
    pressureLine("P_Line", PIN_LIN),
    pressureChamber("P_Chamber", PIN_CHAMBER),
    pressureN2("P_N2", PIN_N2)
{
    devices[0] = &controlbox;
    devices[1] = &thermocouplePiping;
    devices[2] = &thermocoupleChamber;
    devices[3] = &pressureOx1;
    devices[4] = &pressureOx2;
    devices[5] = &pressureLine;
    devices[6] = &pressureChamber;
    devices[7] = &pressureN2;
}
Testbench::begin()
{

}
Testbench::verification()
{

}
Testbench::sequence()
{

}
