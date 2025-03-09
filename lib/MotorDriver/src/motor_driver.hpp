#pragma once
#ifndef MOTOR_DRIVER_HPP
#define MOTOR_DRIVER_HPP

#include <Arduino.h>
// #include "base_module.hpp"

class MotorDriver /* : public BaseModule*/
{
private:
  bool state[5][4];
  // void taskFn() override;
  void writeState(const bool state[4]);

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