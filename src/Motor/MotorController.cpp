#include "MotorController.h"

const int MAX_DUTY = pow(2, MOTOR_RESOLUTION) - 1; // Biggest (unsigned) number given the resolution
const int PERIOD = (1.0F/MOTOR_FREQUENCY) * 1000000; // Time it takes to compelte one PWM cycle in microseconds

MotorController::MotorController(const uint8_t pin, const uint8_t channel) : channel(channel)
{
    ledcSetup(channel, MOTOR_FREQUENCY, MOTOR_RESOLUTION);
    ledcAttachPin(pin, channel);
}

void MotorController::set(int pulseWidth)
{
    int clamped = constrain(pulseWidth, PWM_MIN, PWM_MAX);
    int output = ((clamped * MAX_DUTY) / PERIOD); // Mathematically: calculated duty cycle as decimal, and multiplies by max duty
    ledcWrite(this->channel, output);
}

void MotorController::stop()
{
    this->set(1000);
}