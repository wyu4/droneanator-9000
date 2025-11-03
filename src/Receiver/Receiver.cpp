#include <Arduino.h>
#include <IBusBM.h>
#include "Receiver.h"

const int MIN_RATE = 1000; // Minimum output rate of the controller (microseconds)
const int MAX_RATE = 2000; // Maximium output rate of the controller (microseconds)
const int MID_RATE = 1500; // (MAX_RATE + MIN_RATE) / 2

const uint8_t ROLL_CHANNEL = 1;
const uint8_t PITCH_CHANNEL = 2;
const uint8_t YAW_CHANNEL = 3;

IBusBM IBus;

void setupReceiver()
{
    IBus.begin(Serial2, IBUSBM_NOTIMER);
}

void updateReceiver() {
    IBus.loop();
}

int getDesiredRoll() {
    return IBus.readChannel(ROLL_CHANNEL);
}

int getDesiredPitch() {
    return IBus.readChannel(PITCH_CHANNEL);
}

int getDesiredYaw() {
    return IBus.readChannel(YAW_CHANNEL);
}