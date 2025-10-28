#include "MotorController.h"

void setupMotorControllers() {
    analogWriteFrequency(250);
    analogWriteResolution(12);
    delay(250);
}


MotorController::MotorController(const uint8_t pin) : pin(pin) {
    pinMode(pin, OUTPUT);
}

void MotorController::set(int value) {
    analogWrite(this->pin, value);
}