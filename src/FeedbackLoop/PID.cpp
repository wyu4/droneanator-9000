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
}

float PID::calculate(const float &feedback)
{
    // Storing time (this is inefficfent :D)
    const unsigned long currentTime = millis();
    long deltaTime = currentTime - this->lastMillis;
    if (deltaTime <= this->minDeltaTime)
        deltaTime = this->minDeltaTime + 1; // Make sure the delta time is a valid value, and rewrite to 1ms if not
    this->lastMillis = currentTime;

    float currentError = this->setpoint - feedback;
    if (tolerance >= 0 && abs(currentError) <= tolerance)
    {
        return 0;
    }

    // if (period > 0)
    // {
    //     while (currentError > (period / 2))
    //         currentError -= (period);
    //     while (currentError < -(period / 2))
    //         currentError += (period);
    // }

    // Add the average of the current and last error to account for inconsistent loop times
    if (this->errorSumClamp < 0) // clamp value is negative, add without clamping
    {
        this->errorSum += (((currentError + this->lastError) * deltaTime) / 2.0);
    }
    else // Clamp the error sum
    {
        this->errorSum = constrain(this->errorSum + (((currentError + this->lastError) * deltaTime) / 2.0), -this->errorSumClamp, this->errorSumClamp);
    }

    // PID calculations
    const float pTerm = this->kP * currentError;                             // Current error, multiplied by kP
    const float iTerm = this->kI * this->errorSum;                           // Sum of errors multiplied by kI
    const float dTerm = this->kD * ((currentError - lastError) / deltaTime); // Slope of the error (deltaError / deltaTime), multiplied by kD

    this->lastError = currentError;

    float output = pTerm + iTerm + dTerm;

    // Clamping the output
    output = constrain(output, -this->outputClamp, this->outputClamp);

    return output;
}