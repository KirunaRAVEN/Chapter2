/* Filename:        actuators.h
 * Author:          Diego Almendro Wieczorek
 * Date:            26.04.2026
 * Version:         V1.0 (26.04.2026)
 *
 * Purpose:         Actuators library header file.
 */
 #define OPEN true
 #define CLOSED false
 #define ACTIVATED true
 #define DEACTIVATED false

 // GENERIC ACTUATOR (relay)
class Actuator : public iDevice {
public:
  Actuator(const char* name, int pin, bool inverse = false);
  void begin() override;
  void update() override;
  void write(bool state);
  void open() {write(OPEN);}
  void close() {write(CLOSED);}
  void activate() {write(ACTIVATED);}
  void deactivate() {write(DEACTIVATED);}
private:
  const int _pin;
  const bool _inverse;
};