#pragma once
#ifndef LINE_COLOR_TRACKER_HPP
#define LINE_COLOR_TRACKER_HPP

#include <Arduino.h>
#include <algorithm>
#include "base_module.hpp"

class LineColorTracker : public BaseModule
{
private:
  static constexpr int MIN_RED = 5, MAX_RED = 38;
  static constexpr int MIN_GREEN = 4, MAX_GREEN = 42;
  static constexpr int MIN_BLUE = 4, MAX_BLUE = 35;

  int getRed();
  int getGreen();
  int getBlue();

  void printColor(int red, int green, int blue);

  void taskFn() override;

public:
  LineColorTracker();
  ~LineColorTracker();
};

extern LineColorTracker *lineColorTracker;
#endif
