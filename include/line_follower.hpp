#pragma once
#ifndef LINE_FOLLOWER_HPP
#define LINE_FOLLOWER_HPP

#include <Arduino.h>
#include "types.hpp"
#include "utils/assign.hpp"
#include "base_module.hpp"
#include "motor_driver.hpp"
#include "web_server.hpp"
#include "utils/compare.hpp"

#define LINE_SENSOR_PIN_LEFT_MOST GPIO_NUM_33
#define LINE_SENSOR_PIN_LEFT GPIO_NUM_39
#define LINE_SENSOR_PIN_CENTER GPIO_NUM_36
#define LINE_SENSOR_PIN_RIGHT GPIO_NUM_35
#define LINE_SENSOR_PIN_RIGHT_MOST GPIO_NUM_4

struct LineFollowerSensorValues
{
  int out1;
  int out2;
  int out3;
  int out4;
  int out5;
};

enum LineFollowerDecision
{
  STOP = 600,
  LEFT,
  RIGHT,
  FORWARD,
  BACKWARD
};

using LineFollowerSignals = Types::SemaphoreMutexData<LineFollowerSensorValues>;

class LineFollower : public BaseModule
{
private:
  const int factors[5] = {-4, -3, 2, 3, 4};

  LineFollowerSignals signals;
  Types::SemaphoreMutexData<bool> enable;

  MotorDriver *motorDriver = nullptr;
  RWebSocketClient *webSocketClient = nullptr;

  LineFollowerDecision decide(LineFollowerSensorValues values);
  String decide2String(LineFollowerDecision decision);
  void driveMotor(LineFollowerDecision decision);

  void taskFn() override;

public:
  LineFollower(
      // MotorDriver *motorDriver = nullptr,
      RWebSocketClient *webSocketClient = nullptr);
  ~LineFollower();

  bool getEnable();
  void setEnable(bool value);
  LineFollowerSensorValues getLineFollowerValues();
};

#endif
