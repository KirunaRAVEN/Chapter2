#ifndef I2CSENSORS_H
#define I2CSENSORS_H

#include <Wire.h>
#include "globals.h"

#define ADDR_16BIT 0x48   // ADS1115 (ADDR -> GND)
#define ADDR_12BIT 0x4A   // ADS1015 (ADDR -> SDA)

struct ADCState {
    bool busy;
    uint8_t currentChannel;
    unsigned long startTime;
    float lastValue[4];
};

class ADS1x15 {
private:
    uint8_t addr;
    bool isADS1115;
public:
    ADS1x15(uint8_t address, bool is1115);

    bool present();
    void startConversation(uint8_t ch);
    bool conversationReady();
    float readConversation();
    ADCState state;
};

void initDAQ();

void I2CADC(struct datapoint * data);

#endif /*I2CSENSORS_H*/
