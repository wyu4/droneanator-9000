#include <Arduino.h>
#include "Telemetry/Logger.h"
#include "Receiver/Receiver.h"
#include "Motor/MotorController.h"

void setup()
{
  setupLogger();

  println("Setting up receiver...");
  setupReceiver();
  println(">>> Successfully set up receiver.");

  println("Setting up motor controllers...");
  setupMotorControllers();
}

void loop()
{
  updateLogger();
}
