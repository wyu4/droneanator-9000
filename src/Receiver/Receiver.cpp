#include <Arduino.h>
#include <IBusBM.h>
#include "Receiver.h"

const uint8_t ROLL_CHANNEL = 0;
const uint8_t PITCH_CHANNEL = 1;
const uint8_t THROTTLE_CHANNEL = 2;
const uint8_t YAW_CHANNEL = 3;

IBusBM IBus;

void setupReceiver()
{
    Serial2.begin(9600, SERIAL_8N1, 47, 48);
    delay(1000);
    IBus.begin(Serial2, IBUSBM_NOTIMER);
}

void updateReceiver()
{
    IBus.loop();
}

int getDesiredRoll()
{
    return IBus.readChannel(ROLL_CHANNEL);
}

int getDesiredPitch()
{
    return IBus.readChannel(PITCH_CHANNEL);
}

int getDesiredThrottle()
{
    return IBus.readChannel(THROTTLE_CHANNEL);
}

int getDesiredYaw()
{
    return IBus.readChannel(YAW_CHANNEL);
}