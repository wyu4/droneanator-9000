#include <Wire.h>
#include "IMU.h"

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

bool setupIMU()
{
    Wire.begin(19, 20);
    
    if (!bno.begin(adafruit_bno055_opmode_t::OPERATION_MODE_IMUPLUS))
    {
        return false;
    }
    delay(1000);
    // bno.setExtCrystalUse(true);
    return true;
}

imu::Vector<3> getMeasuredEuler() {
    return bno.getVector(Adafruit_BNO055::VECTOR_EULER);
}

imu::Quaternion getMeasuredQuaternion() {
    return bno.getQuat();
}