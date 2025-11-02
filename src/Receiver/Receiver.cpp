#include <Arduino.h>
#include <IBusBM.h>
#include "Receiver.h"

const uint8_t ROLL = 1;
const uint8_t PITCH = 2;
const uint8_t THROTTLE = 3;
const uint8_t YAW = 4;
const uint8_t ARMING = 5;

IBusBM IBus;

void setupReceiver()
{
    IBus.begin(Serial2, IBUSBM_NOTIMER);
}

int readReceiverRoll()
{
    return IBus.readChannel(ROLL);
}

int readReceiverPitch()
{
    return IBus.readChannel(PITCH);
}

int readReceiverYaw()
{
    return IBus.readChannel(YAW);
}

int readReceiverThrottle()
{
    return IBus.readChannel(THROTTLE);
}

int readReceiverArming()
{
    return IBus.readChannel(ARMING);
}