#include <Arduino.h>
#include "line_follower.hpp"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  LineFollower lf;

  xTaskCreatePinnedToCore(LineFollower::TaskWrapper, "LINE_FOLOWER", 5120, nullptr, 10, nullptr, 1);
}

void loop() {}
