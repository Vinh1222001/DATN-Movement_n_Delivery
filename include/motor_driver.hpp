#pragma once
#ifndef MOTOR_DRIVER_HPP
#define MOTOR_DRIVER_HPP

#include <Arduino.h>
#include "base_module.hpp"

class MotorDriver : public BaseModule
{
private:
  void taskFn() override;

public:
  MotorDriver(int priority = DEFAULT_TASK_PRIORITY);
  ~MotorDriver();
};

extern MotorDriver *motorDriver;
#endif