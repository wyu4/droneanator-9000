#include <Arduino.h>
#include <IBusBM.h>
#include "Receiver.h"

const uint8_t useableChannels = 5;
float readings[] = {0, 0, 0, 0, 0};

IBusBM IBus;

void setupReceiver()
{
    IBus.begin(Serial2, IBUSBM_NOTIMER, 40, 45);
    delay(1000);
}

void updateReceiver()
{
    IBus.loop();
    for (uint8_t i = 0; i < useableChannels; i++) {
        readings[i] = (float) IBus.readChannel(i);
    }
}

float readChannel(uint8_t channel) {
    return readings[channel];
}