#include <Arduino.h>
#include "line_follower.hpp"

LineFollower lf; // Stack allocation (better memory management)

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  xTaskCreatePinnedToCore(LineFollower::taskWrapper, "LINE_FOLOWER", 5120, &lf, 10, nullptr, 1);
}

void loop() {}
