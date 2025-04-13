#pragma once
#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "base_module.hpp"
#include "line_follower.hpp"
#include "motor_driver.hpp"
#include "monitor.hpp"
#include "web_server.hpp"
#include "color_detector.hpp"
#include "mpu_reader.hpp"

class Controller : public BaseModule
{
private:
  int state;

  MotorDriver *motorDriver;
  LineFollower *lineFollower;
  RWebSocketClient *webServer;

  ColorDetector *colorDetector;

  Monitor *monitor;

  void stateMachine();
  void taskFn() override;

public:
  Controller();
  ~Controller();

  void setState(int state);
};

#endif
