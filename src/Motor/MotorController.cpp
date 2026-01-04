#include "MotorController.h"

const int MAX_DUTY = pow(2, MOTOR_RESOLUTION) - 1;
const int PERIOD = (1.0F/MOTOR_FREQUENCY) * 1000000;

MotorController::MotorController(const uint8_t pin, const uint8_t channel) : channel(channel)
{
    ledcSetup(channel, MOTOR_FREQUENCY, MOTOR_RESOLUTION);
    ledcAttachPin(pin, channel);
    // pinMode(pin, OUTPUT);
}

void MotorController::set(int value)
{
    int clamped = constrain(value, PWM_MIN, PWM_MAX);
    int output = ((clamped * MAX_DUTY) / PERIOD);
    ledcWrite(this->channel, output);
    // Serial.printf("Motor %d: value=%d duty=%d\n", channel, value, output);
    // Serial.printf("Max duty %d and period %duS\n", MAX_DUTY, PERIOD);
}

void MotorController::stop()
{
    this->set(1000);
}