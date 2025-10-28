#include <Arduino.h>

/**
 * @brief Initializes everything needed to communicate with motor controllers.
 * 
 */
void setupMotorControllers();

/**
 * @brief The Motor Controller class.
 * 
 */
struct MotorController {
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
};