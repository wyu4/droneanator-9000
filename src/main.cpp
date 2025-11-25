#include <Arduino.h>
#include "Telemetry/Logger.h"
#include "Receiver/Receiver.h"
#include "IMU/IMU.h"
#include "Motor/MotorController.h"
#include "FeedbackLoop/PID.h"

/*
  motor1 output = throttle - roll - pitch - yaw
  motor2 output = throttle - roll + pitch + yaw
  motor3 output = throttle + roll + pitch - yaw
  motor4 output = throttle + roll - pitch + yaw
*/
MotorController motor1(1);  // Front right (clockwise)
MotorController motor2(2);  // Back right (counter-clockwise)
MotorController motor3(42); // Back left (clockwise)
MotorController motor4(41); // Front left (counter-clockwise)
int output1 = 0;
int output2 = 0;
int output3 = 0;
int output4 = 0;

PID pitchController(0.001, 0, 0);
PID rollController(0.001, 0, 0);
PID yawController(0.001, 0, 0);
float pidPitchOutput = 0;
float pidYawOutput = 0;
float pidRollOutput = 0;

int desiredThrottle = 0;
int desiredRoll = 0;
int desiredPitch = 0;
int desiredYaw = 0;

imu::Vector<3> measuredEuler(0, 0, 0);
float measuredRoll = 0;
float measuredPitch = 0;
float measuredYaw = 0;

bool preventThrottle = true;

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

  pitchController.errorSumClamp = 1000;
  rollController.errorSumClamp = 1000;
  yawController.errorSumClamp = 1000;
  pitchController.outputClamp = 60;
  rollController.outputClamp = 60;
  yawController.outputClamp = 60;

  setupMotorControllers();
  motor1.stop();
  motor2.stop();
  motor3.stop();
  motor4.stop();
  println(">> Successfully set up motor controllers...");
}

void loop()
{
  updateReceiver();
  // updateLogger();

  desiredThrottle = getDesiredThrottle();

  if (preventThrottle)
  {
    if (desiredThrottle < CONTROLLER_MIN_RATE)
    {
      println("Waiting for valid throttle value.");
      delay(500);
      return;
    }

    if (desiredThrottle > CONTROLLER_MID_RATE)
    {
      println("Please set the throttle stick to the lowest position.");
      delay(500);
      return;
    }
    preventThrottle = false;
  }

  desiredRoll = getDesiredRoll();
  desiredPitch = getDesiredPitch();
  desiredYaw = getDesiredYaw();
  measuredEuler = getMeasuredQuaternionWithOffset().toEuler();
  measuredEuler.toDegrees();

  measuredRoll = measuredEuler.y();
  measuredPitch = -measuredEuler.z();
  measuredYaw = measuredEuler.x();

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nRoll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", measuredRoll, measuredPitch, measuredYaw);

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nThrottle: %d; Roll: %d; Pitch: %d; Yaw: %d;\n", desiredThrottle, desiredRoll, desiredPitch, desiredYaw);

  pitchController.setSetpoint(desiredPitch);
  rollController.setSetpoint(desiredRoll);
  yawController.setSetpoint(desiredYaw);

  pidPitchOutput = pitchController.calculate(measuredPitch);
  pidYawOutput = rollController.calculate(measuredYaw);
  pidRollOutput = yawController.calculate(measuredRoll);

  // Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nRoll: %0.2f; Pitch: %0.2f; Yaw: %0.2f;\n", pidRollOutput, pidPitchOutput, pidYawOutput);

  output1 = desiredThrottle - pidRollOutput - pidPitchOutput - pidYawOutput; // FR
  output2 = desiredThrottle - pidRollOutput + pidPitchOutput + pidYawOutput; // BR
  output3 = desiredThrottle + pidRollOutput + pidPitchOutput - pidYawOutput; // BL
  output4 = desiredThrottle + pidRollOutput - pidPitchOutput + pidYawOutput; // FL

  Serial.printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\nFL: %d\tFR: %d\nBL: %d\tBR: %d\n", output4, output1, output3, output2);

  motor1.set(output1);
  motor2.set(output2);
  motor3.set(output3);
  motor4.set(output4);
  delay(20);
}

// IBusBM IBus; // Create an IBusBM object

// void setup() {
//   Serial.begin(115200);
//   Serial.println("ESP32 IBus-BM Test Started");

//   IBus.begin(Serial2, IBUSBM_NOTIMER); // disables the timer interrupt but you need to call loop() yourself
// }

// void loop() {
//   // Call the IBus loop function to process incoming data, updates channel values
//   IBus.loop();

//   // Read and print channel values with no(?) delay
//   Serial.print("Ch1: ");
//   Serial.print(IBus.readChannel(0));
//   Serial.print(" Ch2: ");
//   Serial.print(IBus.readChannel(1));
//   Serial.print(" Ch3: ");
//   Serial.print(IBus.readChannel(2));
//   Serial.print(" Ch4: ");
//   Serial.print(IBus.readChannel(3));
//   Serial.print(" Ch5: ");
//   Serial.print(IBus.readChannel(4));
//   Serial.print(" Ch6: ");
//   Serial.println(IBus.readChannel(5));
// }
