#include <Arduino.h>

const uint32_t PWM_FREQUENCY = 250; // Hz
const uint32_t PWM_RESOLUTION = 12; // Bits, meaning a min of 0, and max of 2^(whatever this variable is set to)
const int PWM_MIN = 0;              // Should be zero microseconds
const int PWM_MAX = 2000;           // Maximum calibrated value
const float PWM_MULTIPLIER = 1.024; // (2 ^ PWM_RESOLUTION) / PWM_MAX microseconds.

/**
 * @brief Initializes everything needed to communicate with motor controllers.
 *
 */
void setupMotorControllers();

/**
 * @brief The Motor Controller class.
 *
 */
struct MotorController
{
    /**
     * @brief The configured PWM pin for the motor controller
     *
     */
    const uint8_t pin;

    /**
     * @brief Construct a new Motor Controller object
     *
     * @param pin The PWM pin connected to the motor controller
     */
    MotorController(const uint8_t pin);

    /**
     * @brief Set the raw PWM value going to the motor controller
     *
     * @param value Raw PWM value
     */
    void set(int value);

    /**
     * @brief Stops the motor by sending a duty cycle of 50%.
     *
     */
    void stop();
};