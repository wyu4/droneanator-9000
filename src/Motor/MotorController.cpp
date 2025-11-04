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
    int constrained = constrain(value - PWM_MIN, 0, PWM_MAX - PWM_MIN);
    analogWrite(this->pin, PWM_MULTIPLIER * constrained);
}

void MotorController::stop()
{
    this->set(0);
}