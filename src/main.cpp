#include <Arduino.h>
#include "line_follower.hpp"
#include "color_detector.hpp"

LineFollower lf;
// ColorDetector cd;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  xTaskCreatePinnedToCore(LineFollower::taskWrapper, "LINE_FOLOWER", 5120, &lf, 10, nullptr, 1);
  // xTaskCreatePinnedToCore(ColorDetector::taskWrapper, "COLOR_DETECTOR", 5120, &cd, 10, nullptr, 1);
}

void loop() {}
