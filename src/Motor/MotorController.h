#include <Arduino.h>

const int PWM_MIN = 1000;              // Microseconds
const int PWM_MAX = 2000;           // Microseconds

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
     * @brief Set the raw PWM value going to the motor controller
     *
     * @param value Raw PWM value
     */
    void set(int value);

    /**
     * @brief Stops the motor by sending a duty cycle of 0%.
     *
     */
    void stop();
};