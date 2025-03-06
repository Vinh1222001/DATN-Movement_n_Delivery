#include <Arduino.h>
#include "line_follower.hpp"
#include "line_color_tracker.hpp"
#include "shift_register.hpp"

// LineFollower lf;
// LineColorTracker lct;
ShiftRegister sr;
void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  lineColorTracker = new LineColorTracker();
  lineFollower = new LineFollower();
  shiftRegister = new ShiftRegister();
}

void loop() {}
