/**
 * @brief Initializes everything needed to receive data from the pilot.
 *
 */
void setupReceiver();

/**
 * @brief Reads the roll channel of the iBus connection with the receiver
 * 
 * @return roll input
 */
int readReceiverRoll();

/**
 * @brief Reads the pitch channel of the iBus connection with the receiver
 * 
 * @return pitch input
 */
int readReceiverPitch();

/**
 * @brief Reads the yaw channel of the iBus connection with the receiver
 * 
 * @return yaw input
 */
int readReceiverYaw();

/**
 * @brief Reads the throttle channel of the iBus connection with the receiver
 * 
 * @return throttle input
 */
int readReceiverThrottle();

/**
 * @brief Reads the throttle channel of the iBus connection with the receiver
 * 
 * @return throttle input
 */
int readReceiverArming();