#include <Arduino.h>

const int MOTOR_FREQUENCY = 50; // Hz
const int MOTOR_RESOLUTION = 11; // Bits
const int PWM_MIN = 1000;              // Minimum pulse width in microseconds
const int PWM_MAX = 2000;           // Maximum pulse width in microseconds

/**
 * @brief The Motor Controller class.
 *
 */
struct MotorController
{
    /**
     * @brief The configured channel for the motor controller
     *
     */
    const uint8_t channel;

    /**
     * @brief Construct a new Motor Controller object
     *
     * @param pin The PWM pin connected to the motor controller
     * @param channel The channel
     */
    MotorController(const uint8_t pin, const uint8_t channel);

    /**
     * @brief Set the pulse width of the PWM going to the motor controller
     *
     * @param value Pulse width in microseconds
     */
    void set(int value);

    /**
     * @brief Stops the motor
     * 
     */
    void stop();
};