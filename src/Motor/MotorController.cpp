#include "MotorController.h"

MotorController::MotorController(const uint8_t pin, const uint8_t channel) : channel(channel)
{
    ledcSetup(channel, 50, 12);
    ledcAttachPin(pin, channel);
    // pinMode(pin, OUTPUT);
}

void MotorController::set(int value)
{
    ledcWrite(this->channel, map(value, 1000, 2000, 204, 409));
}

void MotorController::stop()
{
    this->set(1000);
}