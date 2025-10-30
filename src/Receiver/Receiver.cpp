#include <Arduino.h>
#include <IBusBM.h>
#include "Receiver.h"

const uint8_t ROLL = 1;
const uint8_t PITCH = 2;
const uint8_t THROTTLE = 3;
const uint8_t YAW = 4;
const uint8_t ARMING = 5;

IBusBM connection;

void setupReceiver()
{
    connection.begin(Serial2);
}

int readReceiverRoll()
{
    return connection.readChannel(ROLL);
}

int readReceiverPitch()
{
    return connection.readChannel(PITCH);
}

int readReceiverYaw()
{
    return connection.readChannel(YAW);
}

int readReceiverThrottle()
{
    return connection.readChannel(THROTTLE);
}

int readReceiverArming()
{
    return connection.readChannel(ARMING);
}