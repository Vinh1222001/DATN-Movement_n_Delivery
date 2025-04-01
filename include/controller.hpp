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

enum RobotState
{
  INIT = 200,
  START,
  PICKUP_TRANSIT,
  PICKUP,
  DROPOFF_TRANSIT,
  DROPOFF,
  IDLE,
};

class Controller : public BaseModule
{
private:
  RobotState state;

  MotorDriver *motorDriver;
  LineFollower *lineFollower;
  RWebServer *webServer;

  ColorDetector *colorDetector;

  Monitor *monitor;

  void init();
  void start();
  void pickupTransit();
  void pickup();
  void dropoffTransit();
  void dropoff();
  void idle();

  void runComponent(BaseModule *component);
  void stateMachine();
  void taskFn() override;

public:
  Controller();
  ~Controller();

  void setState(RobotState state);
  RobotState getState();
};

#endif
