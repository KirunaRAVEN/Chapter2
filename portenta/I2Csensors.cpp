#include "I2Csensors.h"

ADS1x15 adc16(ADDR_16BIT, true);
ADS1x15 adc12(ADDR_12BIT, false);


ADS1x15::ADS1x15(uint8_t address, bool is1115)
    /*
    Constructor function
    */
    : addr(address),
    isADS1115(is1115) {}

bool ADS1x15::present() {
    /*
    is the ADC present?
    */
    Wire.beginTransmission(addr);
    return (Wire.endTransmission() == 0);
}

void ADS1x15::startConversation(uint8_t ch) {
    /*
    Start a conversation on the given channel
    */
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

bool ADS1x15::conversationReady() {
    /*
    Check if conversion finished
    */
    Wire.beginTransmission(addr);
    Wire.write(0x01);  // Config register
    Wire.endTransmission();

    Wire.requestFrom(addr, (uint8_t)2);
    if (Wire.available() < 2) return false;

    uint16_t config = (Wire.read() << 8) | Wire.read();

    return (config & 0x8000); // OS bit = bit 15
}

float ADS1x15::readConversation() {
    /*
    Read result after conversion complete
    */
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

int initSensors() {
    /*
    initialize the sensors. Returns 1 on error and 0 on success.
    */
    Wire.begin();

    if(!adc16.present() || !adc12.present()) {
        return 1;
    }
    adc16.startConversation(0);
    adc12.startConversation(0);

    return 0;
}

void readSensors(struct dataPoint * data) {
    /*
    Read samples from every sensor. needs to be updated with where the data goes.
    Will eventually be used in the future for control sensing, but unused in the current scope.
    */
    for(int i = 0; i<4; i++) {
        // ADS1115 handling
        if (adc16.state.busy && adc16.conversationReady()) {
            adc16.readConversation();
            adc16.startConversation((i + 1) % 4);
        }

        // ADS1015 handling
        if (adc12.state.busy && adc12.conversationReady()) {
            adc12.readConversation();
            adc12.startConversation((i + 1) % 4);
        }
    }

    data->linePressure = adc16.state.lastValue[0];
    data->linePressure = adc16.state.lastValue[1];
    data->linePressure = adc16.state.lastValue[2];
    data->linePressure = adc16.state.lastValue[3];
    data->linePressure = adc12.state.lastValue[0];
    data->linePressure = adc12.state.lastValue[1];
    data->linePressure = adc12.state.lastValue[2];
    data->linePressure = adc12.state.lastValue[3];
    return;
}
