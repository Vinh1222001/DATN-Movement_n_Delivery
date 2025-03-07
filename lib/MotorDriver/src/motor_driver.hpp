#pragma once
#ifndef MOTOR_DRIVER_HPP
#define MOTOR_DRIVER_HPP

#include <Arduino.h>
#include "base_module.hpp"

class MotorDriver : public BaseModule
{
private:
  bool state[5][4];
  void taskFn() override;

public:
  MotorDriver();
  ~MotorDriver();

  bool *moveFoward();
  bool *moveBackward();
  bool *moveLeft();
  bool *moveRight();
  bool *stop();
};

#endif