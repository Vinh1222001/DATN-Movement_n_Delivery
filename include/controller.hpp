#pragma once
#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "base_module.hpp"
#include "line_follower.hpp"
#include "motor_driver.hpp"
#include "mpu_reader.hpp"
class Controller : public BaseModule
{
private:
  int state;

  MotorDriver *motorDriver;
  LineFollower *lineFollower;
  MPUReader *mpuReader;

  void stateMachine();
  void taskFn() override;

public:
  Controller();
  ~Controller();

  void setState(int state);
};

#endif
