#include <Arduino.h>
#include "Controller.h"

void setup()
{
  Controllerinit();//ESP32mother Initialize
}
void loop()
{
  ControllerUpdate();
}