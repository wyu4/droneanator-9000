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
    int constrained = constrain(value, PWM_MIN, PWM_MAX);
    // Serial.println(constrained);
    analogWrite(this->pin, PWM_MULTIPLIER * constrained);
}

void MotorController::stop()
{
    this->set(1000);
}