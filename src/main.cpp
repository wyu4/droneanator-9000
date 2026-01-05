#include <Arduino.h>
#include "Telemetry/Logger.h"
#include "Receiver/Receiver.h"
#include "IMU/IMU.h"
#include "Motor/MotorController.h"
#include "FeedbackLoop/PID.h"

/**
 * @brief Maps any numerical value to a range
 *
 */
#define mapNumber(input, min, max, outMin, outMax) ((input - min) / (max - min) * (outMax - outMin) + outMin);

const boolean hoverOnly = false;
const float maxThrottle = 1400; // Microseconds

MotorController motor1(41, 1); // Front right (clockwise)
MotorController motor2(1, 2);  // Back right (counter-clockwise)
MotorController motor3(42, 3); // Back left (clockwise)
MotorController motor4(2, 4);  // Front left (counter-clockwise)
int output1 = 0;
int output2 = 0;
int output3 = 0;
int output4 = 0;

PID pitchController(1, 0, 0);
PID rollController(1, 0, 0);
PID yawController(4, 0, 0);
float pidPitchOutput = 0;
float pidYawOutput = 0;
float pidRollOutput = 0;
unsigned long lastTime = 0;  // Time since boot in microseconds
unsigned long lastTimeTelemetry = 0; // Time since last telemetry send in microseconds
unsigned long deltaTime = 1; // Time in microseconds
const unsigned long telemetryRate = 100000; // Time between each telemetry send in microseconds

float desiredThrottle = 0;
float desiredRoll = 0;
float desiredPitch = 0;
float desiredYawVelocity = 0;
int desiredArm = 0;

imu::Quaternion measuredQuaternion(1, 0, 0, 0);
imu::Vector<3> measuredEuler(0, 0, 0);
float rawReadings[] = {0, 0, 0};
float measuredRoll = 0;
float measuredPitch = 0;
float measuredYawVelocity = 0;

bool preventThrottle = true;
bool disarmed = false;

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

/**
 * @brief Send telemetry data over WiFi
 * 
 * @param currentTime Current time since boot
 */
void sendTelemetry(const unsigned long &currentTime)
{
  if ((currentTime - lastTimeTelemetry) < telemetryRate) return;
  writeUDPF("$data %d %d %d %d %d %0.2f %0.2f %0.2f %lu %0.2f %0.2f %0.2f %0.2f %d %0.2f %0.2f %0.2f %d %d", output1, output2, output3, output4, hoverOnly, pidPitchOutput, pidRollOutput, pidYawOutput, deltaTime, desiredThrottle, desiredPitch, desiredRoll, desiredYawVelocity, desiredArm, measuredPitch, measuredRoll, measuredYawVelocity, preventThrottle, disarmed);
  lastTimeTelemetry = currentTime;
}

void setup()
{
  // stop();

  setupLogger();
  setPairingMode(true);

  Serial.println("Setting up receiver...");
  setupReceiver();
  Serial.println(">>> Successfully set up receiver.");

  Serial.println("Setting up IMU...");
  if (!setupIMU())
  {
    while (true)
    {
      Serial.println(">> Could not set up IMU. Please reboot.");
      delay(1000);
    }
  }
  else
  {
    Serial.println(">> Successfully set up IMU...");
    calibrateIMU();
  }

  Serial.println("Setting up motor controllers...");

  pitchController.errorSumClamp = 400;
  rollController.errorSumClamp = 400;
  yawController.errorSumClamp = 400;
  pitchController.outputClamp = 120;
  rollController.outputClamp = 120;
  yawController.outputClamp = 200;
  stop();
  delay(1000);
  Serial.println(">> Successfully set up motor controllers...");
}

void loop()
{
  const unsigned long currentTime = micros();
  deltaTime = currentTime - lastTime;
  if (deltaTime == 0)
    deltaTime = 1;
  if (lastTime == 0)
  { // If lastTime is invalid
    lastTime = currentTime;
    stop();
    return;
  }
  lastTime = currentTime;

  updateReceiver();
  updateLogger();

  sendTelemetry(currentTime);

  if (readChannel(0) < CONTROLLER_MIN_RATE)
  {
    stop();
    Serial.println("Waiting for valid controller value.");
    delay(100);
    return;
  }

  desiredThrottle = mapNumber(readChannel(THROTTLE_CHANNEL), 1000, 2000, 1000, maxThrottle);
  desiredArm = readChannel(ARM_CHANNEL);

  if (!hoverOnly)
  {
    desiredRoll = mapNumber(readChannel(ROLL_CHANNEL), 1000, 2000, -10, 10);
    desiredPitch = mapNumber(readChannel(PITCH_CHANNEL), 1000, 2000, -10, 10);
    desiredYawVelocity = mapNumber(readChannel(YAW_CHANNEL), 1000, 2000, -10, 10);
  }

  if (desiredArm > CONTROLLER_MIN_RATE + 10)
  {
    disarmed = true;
    stop();
    pitchController.reset();
    rollController.reset();
    yawController.reset();
    return;
  }

  // This check makes it so that as soon as the drone is re-armed, the user will have to lower the throttle stick first before it does anything
  if (disarmed == true)
  {
    disarmed = false;
    preventThrottle = true;
    return;
  }

  if (preventThrottle)
  {
    stop();
    if (desiredThrottle > CONTROLLER_MIN_RATE + 50)
    {
      Serial.println("Please set the throttle stick to the lowest position.");
      delay(100);
      return;
    }
    preventThrottle = false;
  }

  getRawAxis(rawReadings);

  measuredEuler = getMeasuredQuaternionWithOffset().toEuler();
  measuredEuler.toDegrees();
  measuredRoll = measuredEuler.y();
  measuredPitch = measuredEuler.z();
  measuredYawVelocity = getMeasuredYawVelocity();

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nRoll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", measuredRoll, measuredPitch, measuredYawVelocity);

  if (desiredThrottle < CONTROLLER_MIN_RATE + 50)
  {
    stop();
    pitchController.reset();
    rollController.reset();
    yawController.reset();
    // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nThrottle: %0.2f; Roll: %0.2f; Pitch: %0.2f; Yaw: %0.2f; (stopped)\n", desiredThrottle, desiredRoll, desiredPitch, desiredYawVelocity);
    return;
  }

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nThrottle: %0.2f; Roll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", desiredThrottle, desiredRoll, desiredPitch, desiredYawVelocity);

  // Make sure motors stop (ignoring PID controllers) when throttle is at a low state or arming is toggled off

  pitchController.setpoint = desiredPitch;
  rollController.setpoint = desiredRoll;
  yawController.setpoint = desiredYawVelocity;

  pidPitchOutput = pitchController.calculate(measuredPitch, deltaTime);
  pidYawOutput = yawController.calculate(measuredYawVelocity, deltaTime);
  pidRollOutput = rollController.calculate(measuredRoll, deltaTime);

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nRoll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", pidRollOutput, pidPitchOutput, pidYawOutput);

  output1 = (int)round(desiredThrottle - pidRollOutput - pidPitchOutput + pidYawOutput); // FR CW
  output2 = (int)round(desiredThrottle - pidRollOutput + pidPitchOutput - pidYawOutput); // BR CCW
  output3 = (int)round(desiredThrottle + pidRollOutput + pidPitchOutput + pidYawOutput); // BL CW
  output4 = (int)round(desiredThrottle + pidRollOutput - pidPitchOutput - pidYawOutput); // FL CCW

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nFL: %d\tFR: %d\nBL: %d\tBR: %d\n", output4, output1, output3, output2);

  motor1.set(output1);
  motor2.set(output2);
  motor3.set(output3);
  motor4.set(output4);

  // Serial.println(desiredArm);
}