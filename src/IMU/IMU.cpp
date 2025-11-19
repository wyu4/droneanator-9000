#include <Wire.h>
#include "IMU.h"

Adafruit_BNO055 bno = Adafruit_BNO055(55, 0x28, &Wire);

float rawX = 0;
float rawY = 0;
float rawZ = 0;

bool setupIMU()
{
    Wire.begin(19, 20, 100000);
    pinMode(19, PULLUP);
    pinMode(20, PULLUP);

    if (!bno.begin())
    {
        return false;
    }
    delay(5000);
    bno.setExtCrystalUse(true);
    return true;
}

float getIMURawAxis(int axis)
{
    if (axis == 0)
    {
        return rawX;
    }
    else if (axis == 1)
    {
        return rawY;
    }
    else if (axis == 2)
    {
        return rawZ;
    }
    return 0;
}

void updateIMU()
{
    sensors_event_t event;
    bno.getEvent(&event);

    rawX = event.orientation.x;
    rawY = event.orientation.y;
    rawZ = event.orientation.z;
}

imu::Vector<3> getMeasuredEuler()
{
    return bno.getVector(Adafruit_BNO055::VECTOR_EULER);
}

imu::Quaternion getMeasuredQuaternion()
{
    return bno.getQuat();
}