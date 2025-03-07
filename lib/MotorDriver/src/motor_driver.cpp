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
  const bool initialState[5][4] = {
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0},
      {0, 0, 0, 0}};
  memcpy(this->state, initialState, sizeof(state));
}

MotorDriver::~MotorDriver() {}

void MotorDriver::taskFn()
{
  // Do something
}

bool *MotorDriver::moveFoward()
{
  return this->state[MOTOR_DRIVER_MOVE_FORWARD_STATE_IDX];
}

bool *MotorDriver::moveBackward()
{
  return this->state[MOTOR_DRIVER_MOVE_BACKWARD_STATE_IDX];
}

bool *MotorDriver::moveLeft()
{
  return this->state[MOTOR_DRIVER_MOVE_LEFT_STATE_IDX];
}

bool *MotorDriver::moveRight()
{
  return this->state[MOTOR_DRIVER_MOVE_RIGHT_STATE_IDX];
}

bool *MotorDriver::stop()
{
  return this->state[MOTOR_DRIVER_MOVE_STOP_STATE_IDX];
}
