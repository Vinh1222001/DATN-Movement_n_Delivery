#pragma once
#ifndef LINE_FOLLOWER_HPP
#define LINE_FOLLOWER_HPP

#include <Arduino.h>
#include "types.hpp"
#include "base_module.hpp"
#include "motor_driver.hpp"

#define LINE_SENSOR_PIN_LEFT_MOST GPIO_NUM_33
#define LINE_SENSOR_PIN_LEFT GPIO_NUM_39
#define LINE_SENSOR_PIN_CENTER GPIO_NUM_36
#define LINE_SENSOR_PIN_RIGHT GPIO_NUM_35
#define LINE_SENSOR_PIN_RIGHT_MOST GPIO_NUM_34

#define THRESHOLE 2000

struct SensorValues
{
  int out1;
  int out2;
#if LINE_FOLLOWER_VERSION == 1
  int out3;
  int out4;
  int out5;
#endif
};

using LineFollowerSignals = SemaphoreMutexData<SensorValues>;

class LineFollower : public BaseModule
{
private:
  const int factors[5] = {-4, -3, 2, 3, 4};

  LineFollowerSignals signals;
  SemaphoreMutexData<bool> isRobotArrived;

  MotorDriver *motorDriver = nullptr;

  void taskFn() override;

public:
  LineFollower(MotorDriver *motorDriver = nullptr);
  ~LineFollower();

  bool isArrived();
  SensorValues getLineFollowerValues();
};

#endif
