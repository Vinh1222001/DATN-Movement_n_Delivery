#include "main.hpp"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(CONFIG_MONITOR_BAUD);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  lineColorTracker = new LineColorTracker();
  lineFollower = new LineFollower();
  mpuReader = new MPUReader();
  ultraSonicReader = new UltraSonicReader();
}

void loop() {}
