#include <Arduino.h>
#include "line_follower.hpp"
#include "line_color_tracker.hpp"

// LineFollower lf;
LineColorTracker lct;

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  // xTaskCreatePinnedToCore(LineFollower::taskWrapper, "LINE_FOLOWER", 5120, &lf, 10, nullptr, 1);
  xTaskCreatePinnedToCore(LineColorTracker::taskWrapper, "LINE_COLOR_TRACKER", 5120, &lct, 10, nullptr, 1);
}

void loop() {}
