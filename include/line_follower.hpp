#pragma once
#ifndef LINE_FOLLOWER_HPP
#define LINE_FOLLOWER_HPP

#include <Arduino.h>

#include "base_module.hpp"
#include "motor_driver.hpp"

#define THRESHOLE 2000

typedef struct
{
  int out1;
  int out2;
#if LINE_FOLLOWER_VERSION == 1
  int out3;
  int out4;
  int out5;
#endif
} t_lineFollowerValues;

extern t_lineFollowerValues line_reader;

class LineFollower : public BaseModule
{
private:
  t_lineFollowerValues line_reader;
  MotorDriver *motorDriver = nullptr;

  SemaphoreHandle_t xMutextIsArrived;
  bool isItArrived = false;

  void taskFn() override;

public:
  LineFollower(MotorDriver *motorDriver = nullptr);
  ~LineFollower();

  bool isArrived();
};

#endif
