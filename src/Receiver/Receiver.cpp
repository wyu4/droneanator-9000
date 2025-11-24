#include <Arduino.h>
#include <IBusBM.h>
#include "Receiver.h"

const uint8_t ROLL_CHANNEL = 0;
const uint8_t PITCH_CHANNEL = 1;
const uint8_t THROTTLE_CHANNEL = 2;
const uint8_t YAW_CHANNEL = 3;
const float multiplier = 0.02;
const int offset = 1500;

IBusBM IBus;

void setupReceiver()
{
    Serial2.begin(9600, SERIAL_8N1, 40, 41);
    delay(1000);
    IBus.begin(Serial2, IBUSBM_NOTIMER);
}

void updateReceiver()
{
    IBus.loop();
}

int getDesiredRoll()
{
    return (IBus.readChannel(ROLL_CHANNEL) - offset) * multiplier;
}

int getDesiredPitch()
{
    return (IBus.readChannel(PITCH_CHANNEL) - offset) * multiplier;
}

int getDesiredThrottle()
{
    return IBus.readChannel(THROTTLE_CHANNEL);
}

int getDesiredYaw()
{
    return (IBus.readChannel(YAW_CHANNEL) - offset) * multiplier;
}