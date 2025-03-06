#include "motor_driver.hpp"

MotorDriver::MotorDriver(int priority) : BaseModule("MOTOR_DRIVER", priority)
{
  // Define PINS
}

MotorDriver::~MotorDriver() {}

void MotorDriver::taskFn()
{
  // Do something
}

MotorDriver *motorDriver;