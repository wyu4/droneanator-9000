#include <Wire.h>
#include "IMU.h"

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

uint8_t systemStatus = 0;
uint8_t testResult = 0;
uint8_t systemError = 0;

imu::Quaternion offsetQuat(1, 0, 0, 0);
imu::Vector<3> offsetEuler(0, 0, 0);

/**
 * @brief Invert a Quaternion object
 *
 * @param original Original quaternion object
 * @return imu::Quaternion Inverted quaternion object
 */
imu::Quaternion invertQuaternion(const imu::Quaternion &original)
{
	return imu::Quaternion(original.w(), -original.x(), -original.y(), -original.z());
}

/**
 * @brief Reads the system status of the IMU
 *
 */
void readStatus()
{
	bno.getSystemStatus(&systemStatus, &testResult, &systemError);
}

bool setupIMU()
{
	Wire.begin(20, 19, 100000);
	if (!bno.begin(adafruit_bno055_opmode_t::OPERATION_MODE_IMUPLUS))
	{
		return false;
	}

	bno.setExtCrystalUse(true);

	// Wait for fusion to start up
	while (systemStatus < 4)
	{
		readStatus();
		delay(1000);
	}

	delay(1000);
	return true;
}

void getRawAxis(float *array)
{
	sensors_event_t real;
	bno.getEvent(&real);

	sensors_event_t angularVectors;
	bno.getEvent(&angularVectors, Adafruit_BNO055::VECTOR_GYROSCOPE);
	
	sensors_event_t linearVectors;
    bno.getEvent(&linearVectors, Adafruit_BNO055::VECTOR_LINEARACCEL);

	array[0] = angularVectors.gyro.z * (180.0F / 3.14159F);
	array[1] = real.orientation.y; //+ offsetEuler.y();
	array[2] = real.orientation.z;
}

float getMeasuredYawVelocity()
{
	sensors_event_t event;
	bno.getEvent(&event, Adafruit_BNO055::VECTOR_GYROSCOPE);

	return event.gyro.z * (180.0F / 3.14159F);
}

void calibrateIMU()
{
	imu::Quaternion quat = bno.getQuat();
	offsetQuat = invertQuaternion(quat);
	offsetEuler = quat.toEuler();
	offsetEuler.toDegrees();
	Serial.printf("New offset: w[%.3f] x[%.3f] y[%.3f] z[%.3f]\n", offsetQuat.w(), offsetQuat.x(), offsetQuat.y(), offsetQuat.z());
}

imu::Quaternion getMeasuredQuaternionWithOffset()
{
	return offsetQuat * bno.getQuat();
}