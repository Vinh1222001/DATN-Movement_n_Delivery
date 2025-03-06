#include <Arduino.h>
#include "line_follower.hpp"
#include "line_color_tracker.hpp"
#include "shift_register.hpp"
#include "motor_driver.hpp"
#include "mpu_reader.hpp"
#include "ultra_sonic_reader.hpp"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  lineColorTracker = new LineColorTracker();
  lineFollower = new LineFollower();
  shiftRegister = new ShiftRegister();
  motorDriver = new MotorDriver();
  mpuReader = new MPUReader();
  ultraSonicReader = new UltraSonicReader();
}

void loop() {}
