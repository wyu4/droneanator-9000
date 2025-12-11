#include <Arduino.h>
#include "Telemetry/Logger.h"
#include "Receiver/Receiver.h"
#include "IMU/IMU.h"
#include "Motor/MotorController.h"
#include "FeedbackLoop/PID.h"

const boolean hoverOnly = true;

/*
  motor1 output = throttle - roll - pitch - yaw
  motor2 output = throttle - roll + pitch + yaw
  motor3 output = throttle + roll + pitch - yaw
  motor4 output = throttle + roll - pitch + yaw
*/
MotorController motor1(2);  // Front right (clockwise)
MotorController motor2(42); // Back right (counter-clockwise)
MotorController motor3(41); // Back left (clockwise)
MotorController motor4(1);  // Front left (counter-clockwise)
int output1 = 0;
int output2 = 0;
int output3 = 0;
int output4 = 0;

PID pitchController(1, 0, 0);
PID rollController(1, 0, 0);
PID yawController(1, 0, 0);
float pidPitchOutput = 0;
float pidYawOutput = 0;
float pidRollOutput = 0;
unsigned long lastTime = 0;  // Time since boot in microseconds
unsigned long deltaTime = 1; // Time in microseconds

float desiredThrottle = 0;
float desiredRoll = 0;
float desiredPitch = 0;
double desiredYaw = 0;
int desiredArm = 0;

imu::Vector<3> measuredEuler(0, 0, 0);
float measuredRoll = 0;
float measuredPitch = 0;
float measuredYaw = 0;

bool preventThrottle = true;

/**
 * @brief Stop all motors
 *
 */
inline void stop()
{
  motor1.stop();
  motor2.stop();
  motor3.stop();
  motor4.stop();
  // Serial.println("Stopped");
  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nFL: %d\tFR: %d\nBL: %d\tBR: %d\n", 1000, 1000, 1000, 1000);
}

void setup()
{
  setupLogger();
  setPairingMode(true);

  println("Setting up receiver...");
  setupReceiver();
  println(">>> Successfully set up receiver.");

  println("Setting up IMU...");
  if (!setupIMU())
  {
    while (true)
    {
      println(">> Could not set up IMU. Please reboot.");
      delay(1000);
    }
  }
  else
  {
    println(">> Successfully set up IMU...");
    calibrateIMU();
  }

  println("Setting up motor controllers...");

  pitchController.errorSumClamp = 400;
  rollController.errorSumClamp = 400;
  yawController.errorSumClamp = 400;
  pitchController.outputClamp = 60;
  rollController.outputClamp = 60;
  yawController.outputClamp = 60;

  setupMotorControllers();
  stop();
  println(">> Successfully set up motor controllers...");
}

void loop()
{
  delay(20);

  const unsigned long currentTime = micros();
  deltaTime = currentTime - lastTime;
  if (deltaTime == 0)
    deltaTime = 1;
  if (lastTime == 0)
  { // If lastTime is invalid
    lastTime = currentTime;
    return;
  }
  lastTime = currentTime;

  updateReceiver();
  // updateLogger();

  desiredThrottle = getDesiredThrottle();
  desiredArm = getDesiredArm();

  if (preventThrottle)
  {
    if (desiredThrottle < CONTROLLER_MIN_RATE)
    {
      println("Waiting for valid throttle value.");
      delay(250);
      return;
    }

    if (desiredThrottle > CONTROLLER_MIN_RATE + 25)
    {
      println("Please set the throttle stick to the lowest position.");
      delay(250);
      return;
    }
    preventThrottle = false;
  }

  measuredEuler = getMeasuredQuaternionWithOffset().toEuler();
  measuredEuler.toDegrees();
  measuredRoll = measuredEuler.y();
  measuredPitch = -measuredEuler.z();
  measuredYaw = measuredEuler.x();

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nRoll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", measuredRoll, measuredPitch, measuredYaw);

  if (!hoverOnly)
  {
    desiredRoll = getDesiredRoll();
    desiredPitch = getDesiredPitch();
    desiredYaw += (getDesiredYaw() * deltaTime * 0.00001);
    while (desiredYaw > 180)
      desiredYaw -= 360;
    while (desiredYaw < -180)
      desiredYaw += 360;
  }

  Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nThrottle: %0.2f; Roll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", desiredThrottle, desiredRoll, desiredPitch, desiredYaw);

  // Make sure motors stop (ignoring PID controllers) when throttle is at a low state or arming is toggled off
  if (desiredThrottle < CONTROLLER_MIN_RATE + 50 || desiredArm > CONTROLLER_MIN_RATE + 500)
  {
    stop();
    pitchController.reset();
    rollController.reset();
    yawController.reset();
    desiredYaw = measuredYaw;
    return;
  }

  pitchController.setpoint = desiredPitch;
  rollController.setpoint = desiredRoll;
  yawController.setpoint = desiredYaw;

  pidPitchOutput = pitchController.calculate(measuredPitch, deltaTime);
  pidYawOutput = rollController.calculate(measuredYaw, deltaTime);
  pidRollOutput = yawController.calculate(measuredRoll, deltaTime);

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nRoll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", pidRollOutput, pidPitchOutput, pidYawOutput);

  output1 = (int)round(desiredThrottle - pidRollOutput - pidPitchOutput - pidYawOutput); // FR
  output2 = (int)round(desiredThrottle - pidRollOutput + pidPitchOutput + pidYawOutput); // BR
  output3 = (int)round(desiredThrottle + pidRollOutput + pidPitchOutput - pidYawOutput); // BL
  output4 = (int)round(desiredThrottle + pidRollOutput - pidPitchOutput + pidYawOutput); // FL

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nFL: %d\tFR: %d\nBL: %d\tBR: %d\n", output4, output1, output3, output2);

  motor1.set(output1);
  motor2.set(output2);
  motor3.set(output3);
  motor4.set(output4);

  // Serial.println(desiredArm);
}