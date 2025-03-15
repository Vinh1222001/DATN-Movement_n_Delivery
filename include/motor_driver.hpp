#pragma once
#ifndef MOTOR_DRIVER_HPP
#define MOTOR_DRIVER_HPP

#include <Arduino.h>
#include "base_module.hpp"

class MotorDriver : public BaseModule
{
private:
  const int maxSpeed = 255;
  const int minSpeed = 0;
  const uint8_t minState = 0;
  const uint8_t maxState = 4;

  SemaphoreHandle_t xBinaryState;

  const uint8_t state[5][4] = {
      {0, 0, 0, 0},
      {1, 0, 1, 0},
      {0, 1, 0, 1},
      {1, 0, 0, 1},
      {0, 1, 1, 0}};

  uint8_t currentState;

  void taskFn() override;
  void writeState(const uint8_t val);

public:
  MotorDriver();
  ~MotorDriver();

  void moveFoward();
  void moveBackward();
  void moveLeft();
  void moveRight();
  void stop();
};

#endif