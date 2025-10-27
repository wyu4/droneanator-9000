#include <Arduino.h>
#include "Telemetry/Logger.h"

void setup()
{
  startLogger();
}

void loop()
{
  loggerLoop();
}
