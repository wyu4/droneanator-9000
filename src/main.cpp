#include <Arduino.h>
#include "Telemetry/Logger.h"
#include "Receiver/Receiver.h"
#include "Motor/MotorController.h"
#include <IBusBM.h>

/*
motor1 output = throttle - roll - pitch - yaw
motor2 output = throttle - roll + pitch + yaw
motor3 output = throttle + roll + pitch - yaw
motor4 output = throttle + roll - pitch + yaw
*/
MotorController motor1(1);  // Front right (clockwise)
// MotorController motor2(2);  // Back right (counter-clockwise)
// MotorController motor3(42); // Back left (clockwise)
// MotorController motor4(41); // Front left (counter-clockwise)

bool preventThrottle = true;

void setup()
{
  setupLogger();
  setPairingMode(true);

  println("Setting up receiver...");
  setupReceiver();
  println(">>> Successfully set up receiver.");

  println("Setting up motor controllers...");
  setupMotorControllers();
  motor1.stop();
  // motor2.stop();
  // motor3.stop();
  // motor4.stop();
  println(">> Successfully set up motor controllers...");
}

void loop()
{
  updateReceiver();
  updateLogger();

  const int desiredThrottle = getDesiredThrottle();
  const int desiredRoll = getDesiredRoll();
  const int desiredPitch = getDesiredPitch();
  const int desiredYaw = getDesiredYaw();

  if (preventThrottle)
  {
    if (desiredThrottle < CONTROLLER_MIN_RATE)
    {
      println("Waiting for valid throttle value.");
      return;
    }

    if (desiredThrottle > CONTROLLER_MID_RATE)
    {
      println("Please set the throttle stick to the lowest position.");
      return;
    }
    preventThrottle = false;
  }

  printformat("Roll: %d, Pitch: %d, Yaw: %d, Throttle: %d", desiredRoll, desiredPitch, desiredYaw, desiredThrottle);
  motor1.set(desiredThrottle);
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
