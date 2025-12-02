#include <Wire.h>

#define ADDR_16BIT 0x48   // ADS1115 (ADDR -> GND)
#define ADDR_12BIT 0x4A   // ADS1015 (ADDR -> SDA)

struct ADCState {
  bool busy = false;
  uint8_t currentChannel = 0;
  unsigned long startTime = 0;
  float lastValue[4] = {0, 0, 0, 0};
};

class ADS1x15 {
public:
  ADS1x15(uint8_t address, bool is1115)
    : addr(address), isADS1115(is1115) {}

  bool present() {
    Wire.beginTransmission(addr);
    return (Wire.endTransmission() == 0);
  }

  // Start conversion on a channel
  void startConversion(uint8_t ch) {
    if (ch > 3) return;

    uint16_t config = 0x8383 | (ch << 12);

    Wire.beginTransmission(addr);
    Wire.write(0x01);
    Wire.write(config >> 8);
    Wire.write(config & 0xFF);
    Wire.endTransmission();

    state.busy = true;
    state.currentChannel = ch;
    state.startTime = millis();
  }

  // Check if conversion finished
  bool conversionReady() {
    Wire.beginTransmission(addr);
    Wire.write(0x01);  // Config register
    Wire.endTransmission();

    Wire.requestFrom(addr, (uint8_t)2);
    if (Wire.available() < 2) return false;

    uint16_t config = (Wire.read() << 8) | Wire.read();

    return (config & 0x8000); // OS bit = bit 15
  }

  // Read result after conversion complete
  float readConversion() {
    Wire.beginTransmission(addr);
    Wire.write(0x00);
    Wire.endTransmission();

    Wire.requestFrom(addr, (uint8_t)2);

    if (Wire.available() < 2) return NAN;
    int16_t raw = (Wire.read() << 8) | Wire.read();

    float voltage;

    if (isADS1115) {
      voltage = (raw * 4.096F) / 32768.0F;
    } else {
      voltage = (raw * 4.096F) / 2048.0F;
    }

    state.lastValue[state.currentChannel] = voltage;
    state.busy = false;

    return voltage;
  }

  ADCState state;

private:
  uint8_t addr;
  bool isADS1115;
};

// Create both ADC objects
ADS1x15 adc16(ADDR_16BIT, true);
ADS1x15 adc12(ADDR_12BIT, false);

int currentChannel = 0;
unsigned long lastPrint = 0;

// This is just to check if the Portenta can see the ADCs
void initDAQ(){
  Wire.begin();

  Serial.println("Non-blocking ADS1115 + ADS1015 Multi-Channel Reader");

  Serial.print("ADS1115 present: ");
  Serial.println(adc16.present() ? "YES" : "NO");

  Serial.print("ADS1015 present: ");
  Serial.println(adc12.present() ? "YES" : "NO");

  adc16.startConversion(currentChannel);
  adc12.startConversion(currentChannel);
}

void I2CADC(struct datapoint *data){
  // ADS1115 handling
  if (adc16.state.busy && adc16.conversionReady()) {
    adc16.readConversion();
    adc16.startConversion((currentChannel + 1) % 4);
  }

  // ADS1015 handling
  if (adc12.state.busy && adc12.conversionReady()) {
    adc12.readConversion();
    adc12.startConversion((currentChannel + 1) % 4);
  }

  currentChannel = (currentChannel + 1) % 4;

  // Print results at 1 Hz
  if (millis() - lastPrint > 1000) {
    data->sensor1 = adc16.readChannel(0);
    data->sensor2 = adc16.readChannel(1);
    data->sensor3 = adc16.readChannel(2);
    data->sensor4 = adc16.readChannel(3);
    data->sensor5 = adc12.readChannel(0);
    data->sensor6 = adc12.readChannel(1);
    data->sensor7 = adc12.readChannel(2);
    data->sensor8 = adc12.readChannel(3);
  }
}

void loop() {
  I2CADC(datapoint);
}
