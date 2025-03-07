#include "motor_driver.hpp"

MotorDriver::MotorDriver()
    : BaseModule(
          "MOTOR_DRIVER",
          MOTOR_DRIVER_TASK_PRIORITY,
          MOTOR_DRIVER_TASK_DELAY,
          MOTOR_DRIVER_TASK_STACK_DEPTH_LEVEL,
          MOTOR_DRIVER_TASK_PINNED_CORE_ID)
{
  // Define PINS
}

MotorDriver::~MotorDriver() {}

void MotorDriver::taskFn()
{
  // Do something
}

MotorDriver *motorDriver;