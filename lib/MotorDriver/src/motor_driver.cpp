#include "motor_driver.hpp"

MotorDriver::MotorDriver()
{
  // Define PINS
  const bool initialState[5][4] = {
      {0, 0, 0, 0},
      {1, 0, 1, 0},
      {0, 1, 0, 1},
      {1, 0, 0, 1},
      {0, 1, 1, 0}};
  memcpy(this->state, initialState, sizeof(state));

  pinMode(MOTOR_DRIVER_PIN_IN1, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN2, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN3, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN4, OUTPUT);

  pinMode(MOTOR_DRIVER_PIN_ENA, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_ENB, OUTPUT);

  analogWrite(MOTOR_DRIVER_PIN_ENA, 255);
  analogWrite(MOTOR_DRIVER_PIN_ENB, 255);
}

MotorDriver::~MotorDriver() {}

// void MotorDriver::taskFn()
// {
//   // Do something
// }

void MotorDriver::writeState(const bool state[4])
{
  digitalWrite(MOTOR_DRIVER_PIN_IN1, state[0]);
  digitalWrite(MOTOR_DRIVER_PIN_IN2, state[1]);
  digitalWrite(MOTOR_DRIVER_PIN_IN3, state[2]);
  digitalWrite(MOTOR_DRIVER_PIN_IN4, state[3]);
}

void MotorDriver::moveFoward()
{
  ESP_LOGI("MOTOR_DRIVER", "Move forward");
  this->writeState(this->state[MOTOR_DRIVER_MOVE_FORWARD_STATE_IDX]);
}

void MotorDriver::moveBackward()
{
  ESP_LOGI("MOTOR_DRIVER", "Move backward");
  this->writeState(this->state[MOTOR_DRIVER_MOVE_BACKWARD_STATE_IDX]);
}

void MotorDriver::moveLeft()
{
  ESP_LOGI("MOTOR_DRIVER", "Move left");
  this->writeState(this->state[MOTOR_DRIVER_MOVE_LEFT_STATE_IDX]);
}

void MotorDriver::moveRight()
{
  ESP_LOGI("MOTOR_DRIVER", "Move right");
  this->writeState(this->state[MOTOR_DRIVER_MOVE_RIGHT_STATE_IDX]);
}

void MotorDriver::stop()
{
  ESP_LOGI("MOTOR_DRIVER", "Stopped");
  this->writeState(this->state[MOTOR_DRIVER_MOVE_STOP_STATE_IDX]);
}
