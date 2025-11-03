#include "MotorController.h"

void setupMotorControllers()
{
    analogWriteFrequency(PWM_FREQUENCY);
    analogWriteResolution(PWM_RESOLUTION);
    delay(250);
}

MotorController::MotorController(const uint8_t pin) : pin(pin)
{
    pinMode(pin, OUTPUT);
}

void MotorController::set(int value)
{
    analogWrite(this->pin, PWM_MULTIPLIER * value);
}

void MotorController::stop()
{
    this->set(PWM_MID);
}