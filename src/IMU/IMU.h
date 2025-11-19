#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>

/**
 * @brief Initializes everything related to the IMU.
 *
 */
bool setupIMU();

float getIMURawAxis(int axis);

void updateIMU();

/**
 * @brief Get the Euler object fused by the IMU
 * 
 * @return imu::Vector<3> -
 */
imu::Vector<3> getMeasuredEuler();

/**
 * @brief Get the Quaternion object fused by the IMU
 * 
 * @return imu::Quaternion
 */
imu::Quaternion getMeasuredQuaternion();