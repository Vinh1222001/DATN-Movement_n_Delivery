#pragma once
#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "base_module.hpp"
#include "line_follower.hpp"
#include "monitor.hpp"
#include "web_server.hpp"
#include "color_detector.hpp"
#include "mpu_reader.hpp"
#include "communicate.hpp"

enum RobotState
{
  INIT = 200,
  SETUP,
  READY,
  PING,
  START,
  PICKUP_TRANSIT,
  PICKUP,
  DROPOFF_TRANSIT,
  DROPOFF,
  CLASSIFY,
  IDLE,
};

enum ProductClassProps
{
  LABEL = 0,
  ACCURATION,
  X,
  Y,
  WIDTH,
  HEIGHT,
};

class Controller : public BaseModule
{
private:
  bool isClassifying;
  bool isMoving;
  ColorSet nextArea;
  RobotState state;

  RWebSocketClient *webSocketClient = nullptr;
  Communicate *communicate = nullptr;
  Monitor *monitor = nullptr;

  LineFollower *lineFollower = nullptr;
  ColorDetector *colorDetector = nullptr;

  MPUReader *mpuReader = nullptr;

  bool init();
  bool setup();
  bool ready();
  bool ping();
  bool start();
  bool pickupTransit();
  bool pickup();
  bool dropoffTransit();
  bool dropoff();
  bool classify();
  bool idle();
  bool setNextArea(ColorSet area);

  void runComponent(BaseModule *component);
  void stateMachine();
  void taskFn() override;

public:
  Controller();
  ~Controller();

  void setState(RobotState state, bool extCondition = true);
  RobotState getState();
};

#endif
