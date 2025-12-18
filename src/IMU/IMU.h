#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

/**
 * @brief Initializes everything related to the IMU.
 *
 */
bool setupIMU();

/**
 * @brief Retrieve the BNO055's raw axis readings, stored into a float array
 * 
 * @param array Array to fill with X, Y, and Z values
 * @attention Index 0: X (yaw, 0 to +360)
 * @attention Index 1: Y (roll, -90 to +90)
 * @attention Index 2: Z (pitch, -180 to +180)
 */
void getRawAxis(float *array);

/**
 * @brief Reset the IMU's applied offset to the current fused readings
 * 
 */
void calibrateIMU();

/**
 * @brief Get the Quaternion object fused by the IMU
 * 
 * @return imu::Quaternion
 */
imu::Quaternion getMeasuredQuaternionWithOffset();

/**
 * @brief Get the Measured Yaw Velocity in degrees per second
 * 
 * @return Deg/s
 */
float getMeasuredYawVelocity();
