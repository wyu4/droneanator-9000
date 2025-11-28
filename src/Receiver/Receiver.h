const int CONTROLLER_MIN_RATE = 1000; // Minimum output rate of the controller (microseconds)
const int CONTROLLER_MAX_RATE = 2000; // Maximium output rate of the controller (microseconds)
const int CONTROLLER_MID_RATE = 1500; // (MAX_RATE + MIN_RATE) / 2

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
 * @brief Get the desired roll rate
 *
 * @return Angle in degrees
 */
float getDesiredRoll();

/**
 * @brief Get the desired pitch rate
 *
 * @return Angle in degrees
 */
float getDesiredPitch();

/**
 * @brief Get the desired throttle rate
 *
 * @return int Microseconds
 */
float getDesiredThrottle();

/**
 * @brief Get the desired yaw rate
 *
 * @return Angle in degrees
 */
float getDesiredYaw();