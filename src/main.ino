#include <Arduino.h>
#include "Telemetry/Logger.h"
#include "Receiver/Receiver.h"
#include "Motor/MotorController.h"

TaskHandle_t TelemetryTaskHandle = NULL;

/**
 * @brief The telemetry thread. This is completely cut off from the main thread to minimize stalling on the flight-controller's main functions
 *
 * @param parameter
 */
void TelemetryTask(void *parameter)
{
  for (;;)
  {
    updateLogger();
  }
}

void setup()
{
  setupLogger();

  print("Setting up receiver...\n");
  setupReceiver();
  print(">>> Successfully set up receiver.\n");

  print("Setting up motor controllers...\n");
  setupMotorControllers();

  xTaskCreatePinnedToCore(
      TelemetryTask,        // Task function
      "TelemetryTask",      // Task name
      10000,                // Stack size (bytes)
      NULL,                 // Parameters
      2,                    // Priority
      &TelemetryTaskHandle, // Task handle
      2                     // Core
  );
}

void loop()
{
}
