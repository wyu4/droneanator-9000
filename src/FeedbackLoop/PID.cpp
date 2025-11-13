#include <Arduino.h>
#include "PID.h"

PID::PID(float p, float i, float d) : kP(p), kI(i), kD(d)
{
    this->lastMillis = millis();
}

void PID::setSetpoint(float setpoint)
{
    this->setpoint = setpoint;
    this->reset();
}

void PID::reset()
{
    this->errorSum = 0;
    this->lastError = 0;
    this->lastMillis = millis();
}

float PID::calculate(float feedback)
{
    // Storing time
    const unsigned long currentTime = millis();
    long deltaTime = currentTime - this->lastMillis;
    if (deltaTime <= this->minDeltaTime)
        deltaTime = this->minDeltaTime + 1; // Make sure the delta time is a valid value, and rewrite to 1ms if not
    this->lastMillis = currentTime;

    const float currentError = this->setpoint - feedback;

    // Add the average of the current and last error to account for inconsistent loop times
    if (this->errorSumClamp < 0) // clamp value is negative, add without clamping
    {
        this->errorSum += (((currentError + this->lastError) * deltaTime) / 2.0);
    }
    else // Clamp the error sum
    {
        const float newErrorSum = this->errorSum + (((currentError + this->lastError) * deltaTime) / 2.0);
        if (newErrorSum > this->errorSumClamp)
        {
            this->errorSum = this->errorSumClamp;
        }
        else if (newErrorSum < -this->errorSumClamp)
        {
            this->errorSum = -this->errorSumClamp;
        }
        else
        {
            this->errorSum = newErrorSum;
        }
    }

    // PID calculations
    const float pTerm = this->kP * currentError;                             // Current error, multiplied by kP
    const float iTerm = this->kI * this->errorSum;                           // Sum of errors multiplied by kI
    const float dTerm = this->kD * ((currentError - lastError) / deltaTime); // Slope of the error (deltaError / deltaTime), multiplied by kD

    this->lastError = currentError;

    return pTerm + iTerm + dTerm;
}