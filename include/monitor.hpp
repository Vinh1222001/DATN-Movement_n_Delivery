#pragma once
#ifndef MONITOR_HPP
#define MONITOR_HPP

#include <Arduino.h>
#include <vector>
#include <string>
#include <U8g2lib.h>
#include "base_module.hpp"
#include "types.hpp"
#include "utils/assign.hpp"

#define MONITOR_SCREEN_WIDTH 128
#define MONITOR_SCREEN_HEIGHT 64
#define MONITOR_LINE_SPACING 1

#define MONITOR_FONT u8g2_font_ncenR08_tr

#define MONITOR_POWER_PIN GPIO_NUM_34

#define MONITOR_LINE_COLOR_DETECTOR 1
#define MONITOR_LINE_LINE_FOLLOWER 2
#define MONITOR_LINE_MPU_READER 3

struct Screen
{
  uint8_t fontHeight;
  uint8_t fontWidth;
  uint8_t numOflines;
  std::vector<std::string> buffer;
};

using UI = Types::SemaphoreMutexData<Screen>;
using RobotStateStr = Types::SemaphoreMutexData<String>;

class Monitor : public BaseModule
{
private:
  U8G2_SH1106_128X64_NONAME_F_HW_I2C *monitor = nullptr;
  RobotStateStr robotStateStr;

  UI ui;

  void displaySystemLine();
  void taskFn() override;

public:
  Monitor();
  ~Monitor();

  void display(int line, const char *format, ...);
  void setRobotState(String state);
};

#endif
