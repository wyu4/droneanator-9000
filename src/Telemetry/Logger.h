#include <Arduino.h>


/**
 * @brief Initializes everything needed to log data.
 *
 */
void setupLogger();

/**
 * @brief Print a message to the logger
 *
 * @param message Message to log
 */
void print(const char message[]);

/**
 * @brief Print a message to the logger
 *
 * @param message Message to log
 */
void print(const String message);


/**
 * @brief Sends IMU data
 * 
 * @param imu Euler and quaternion values read from the IMU (1: euler_x, 2: euler_y, 3: euler_z, 4: quat_x, 5: quat_y, 6: quat_z, 7: quat_w)
 */
void sendIMU(float imu[7]);

/**
 * @brief Everything that should go in the flight controller's loop
 * 
 */
void updateLogger();