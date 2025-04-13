#pragma once
#ifndef MOTOR_DRIVER_HPP
#define MOTOR_DRIVER_HPP

#include <Arduino.h>
#include "types.hpp"
#include "base_module.hpp"
#include "PID.hpp"
#include "PID.cpp"
#include "mpu_reader.hpp"

#define MOTOR_DRIVER_PIN_IN4 GPIO_NUM_32
#define MOTOR_DRIVER_PIN_IN3 GPIO_NUM_25
#define MOTOR_DRIVER_PIN_IN2 GPIO_NUM_26
#define MOTOR_DRIVER_PIN_IN1 GPIO_NUM_27

#define MOTOR_DRIVER_PIN_ENA GPIO_NUM_13
#define MOTOR_DRIVER_PIN_ENB GPIO_NUM_12

#define MOTOR_DRIVER_INIT_SPEED 0

using MotorState = Types::SemaphoreMutexData<uint8_t>;

struct Speed
{
  int left;
  int right;
};

using MotorSpeed = Types::SemaphoreMutexData<Speed>;

class MotorDriver : public BaseModule
{
private:
  const int maxSpeed = 255;
  const int minSpeed = 0;
  const uint8_t minState = 0;
  const uint8_t maxState = 4;

  MotorState currentState;

  MotorSpeed speed;

  const uint8_t state[5][4] = {
      {0, 0, 0, 0},
      {0, 1, 1, 0},
      {1, 0, 0, 1},
      {1, 0, 1, 0},
      {0, 1, 0, 1}};

  void taskFn() override;
  void writeState(const uint8_t val);
  void writeSpeed(const int left, const int right);

public:
  MotorDriver();
  ~MotorDriver();

  void setSpeed(const int value);

  void moveForward();
  void moveBackward();
  void moveLeft();
  void moveRight();
  void stop();
};

#endif