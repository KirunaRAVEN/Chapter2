/* Filename:        iDevice.h
 * Author:          Petar Babić and Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         Device interface for every sensor or actuator.
 */

#include "iDevice.h"

void iDevice::test(bool updateBefore)
{
    if (updateBefore) update();
    Serial.println(_name);
    Serial.print(": ");
    Serial.println(read());
}