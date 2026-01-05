const int CONTROLLER_MIN_RATE = 1000; // Minimum output rate of the controller (microseconds)
const int CONTROLLER_MAX_RATE = 2000; // Maximium output rate of the controller (microseconds)
const uint8_t ROLL_CHANNEL = 0;
const uint8_t PITCH_CHANNEL = 1;
const uint8_t THROTTLE_CHANNEL = 2;
const uint8_t YAW_CHANNEL = 3;
const uint8_t ARM_CHANNEL = 4;

/**
 * @brief Initializes everything needed to receive data from the pilot.
 *
 */
void setupReceiver();

/**
 * @brief Everything that should go in the flight controller's loop
 *
 */
void updateReceiver();

/**
 * @brief Read a receiver channel
 * 
 * @param channel
 * @return Pulse-width (uS)
 */
float readChannel(uint8_t channel);