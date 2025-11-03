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
 * @return int Microseconds
 */
int getDesiredRoll();

/**
 * @brief Get the desired pitch rate
 * 
 * @return int Microseconds
 */
int getDesiredPitch();


/**
 * @brief Get the desired yaw rate
 * 
 * @return int Microseconds
 */
int getDesiredYaw();