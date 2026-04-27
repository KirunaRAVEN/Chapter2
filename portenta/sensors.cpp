#include "globals.h"

// GENERIC SENSOR
Sensor::Sensor(const char* name, int pin)
    : iDevice(name), _pin(pin) {}

void Sensor::begin() {
    pinMode(_pin, INPUT);
}

void Sensor::update() {
    // TODO: recalculate factors using V_REF and ADC_RESOLUTION
    iDevice::_value = analogRead(_pin);
}

// LOADCELL HX711
Loadcell::Loadcell(const char* name, int dataPin, int clockPin)
    : iDevice(name), _dataPin(dataPin), _clockPin(clockPin) {}
      
void Loadcell::begin()
{
    loadcell.begin(_dataPin, _clockPin);
    loadcell.set_raw_mode();
    loadcell.tare();
    loadcell.set_scale(1.0f);
}
void Loadcell::update() {
    iDevice::_value = (int)(loadcell.get_value()*LOADCELL_PRESCALER);
}


// THERMOCOUPLE Adafruit_MAX31855
Thermocouple::Thermocouple(const char* name, int csPin)
    : iDevice(name), thermocouple(csPin) {}

void Thermocouple::begin()
{
    thermocouple.begin();
}
void Thermocouple::update()
{
    iDevice::_value = (int)(thermocouple.readCelsius()*THERMOCOUPLE_PRESCALER);
}


// FROM HERE ON, THE CODE IS TO BE DELETED AT THE END OF THE OOP REFACTOR
HX711 loadcell;
Adafruit_MAX31855 thermocoupleChamber(SPI_CS_TC_CHAMBER);
Adafruit_MAX31855 thermocouplePiping(SPI_CS_TC_PIPING);

static const float pressureConversionFactor = MAX_PRESSURE / ((1 << SENSOR_RESOLUTION) - 1);
static const float tempConversionFactor = V_REF / ((1 << SENSOR_RESOLUTION) - 1);
static const float loadcellConversionFactor = .539f/-7766.0f*9.81f;
static const float plumeSensorOffset = -26.18;
static const float plumeSensorFactor = 0.8678;
// Regression from (temp/rawMeasurement): (10/42)(23/56)(27/60)(34/70)

void initLoadcell() {
    thermocoupleChamber.begin();
    thermocouplePiping.begin();

    loadcell.begin(LOADCELL_DATA, LOADCELL_CLOCK);
    loadcell.set_raw_mode();
    loadcell.tare();
    loadcell.set_scale(1.0f);
}

int readAllSensors() {
    /*
    sample all the sensors
    */

    g_packet.data.timestamp = millis();

    g_packet.data.N2OFeedingPressure1 = analogRead(OXIDIZER1_PRESSURE)*pressureConversionFactor;
    g_packet.data.N2OFeedingPressure2 = analogRead(OXIDIZER2_PRESSURE)*pressureConversionFactor;
    g_packet.data.linePressure = analogRead(LINE_PRESSURE)*pressureConversionFactor;
    g_packet.data.chamberPressure = analogRead(CHAMBER_PRESSURE)*pressureConversionFactor*0.25; // chamberPressure is only 0-25 bar
    g_packet.data.N2FeedingPressure = analogRead(NITROGEN_PRESSURE)*pressureConversionFactor;
    g_packet.data.loadcellReading = loadcell.get_value()*loadcellConversionFactor;
    g_packet.data.pipingTemperature = thermocouplePiping.readCelsius();
    g_packet.data.chamberTemperature = thermocoupleChamber.readCelsius();


#ifdef DEBUG
    Serial.print("Loadcell reading: ");
    Serial.println(g_packet.data.loadcellReading);
    Serial.print("Tc Cham: ");
    Serial.println(thermocoupleChamber.readCelsius());

    Serial.print("Tc Piping: ");
    Serial.println(thermocouplePiping.readCelsius());

#endif

    // gives the temp in Celsius
    g_packet.data.bottleTemperature1 = (((analogRead(OXIDIZER1_TEMP)*tempConversionFactor)-0.75)*100)+25;// ((x*f)*100-50)
    g_packet.data.bottleTemperature2 = (((analogRead(OXIDIZER2_TEMP)*tempConversionFactor)-0.75)*100)+25;
    g_packet.data.plumeTemperature = analogRead(PLUME_TEMP)*plumeSensorFactor+plumeSensorOffset;

    return 0;
}

float fastRead() {
    /*
    read only chamberPressure
    */
    return analogRead(CHAMBER_PRESSURE)*pressureConversionFactor*0.25;
}
