#include <Arduino.h>
#include <IBusBM.h>
#include "Receiver.h"

const uint8_t ROLL_CHANNEL = 0;
const uint8_t PITCH_CHANNEL = 1;
const uint8_t THROTTLE_CHANNEL = 2;
const uint8_t YAW_CHANNEL = 3;
const float tiltMultiplier = 0.02;
const int tiltOffset = 1500; // -500 to 500
const uint16_t maxThrottle = 1800;

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
    return (IBus.readChannel(ROLL_CHANNEL) - tiltOffset) * tiltMultiplier;
}

int getDesiredPitch()
{
    return (IBus.readChannel(PITCH_CHANNEL) - tiltOffset) * tiltMultiplier;
}

int getDesiredThrottle()
{
    return min(IBus.readChannel(THROTTLE_CHANNEL), maxThrottle);
}

int getDesiredYaw()
{
    return (IBus.readChannel(YAW_CHANNEL) - tiltOffset) * tiltMultiplier;
}