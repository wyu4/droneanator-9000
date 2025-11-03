#include <Arduino.h>
#include "Telemetry/Logger.h"
#include "Receiver/Receiver.h"
#include "Motor/MotorController.h"
#include <IBusBM.h>

/*
frontLeft output = throttle - roll - pitch - yaw
frontRight output = throttle - roll + pitch + yaw
backLeft output = throttle + roll + pitch - yaw
backRight output = throttle + roll - pitch + yaw
*/
MotorController frontLeft(1);
MotorController frontRight(2);
MotorController backLeft(42);
MotorController backRight(41);

void setup()
{
  setupLogger();
  setPairingMode(true);

  println("Setting up receiver...");
  setupReceiver();
  println(">>> Successfully set up receiver.");

  println("Setting up motor controllers...");
  setupMotorControllers();
  frontLeft.stop();
  frontRight.stop();
  backLeft.stop();
  backRight.stop();
  println(">> Successfully set up motor controllers...");
}

void loop()
{
  updateReceiver();
  updateLogger();

  const int desiredRoll = getDesiredRoll();
  const int desiredPitch = getDesiredPitch();
  const int desiredYaw = getDesiredYaw();

  printformat("Roll: %s, Pitch: %s, Yaw: %s", desiredRoll, desiredPitch, desiredYaw);
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
