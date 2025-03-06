#pragma once
#ifndef LINE_COLOR_TRACKER_HPP
#define LINE_COLOR_TRACKER_HPP

#include <Arduino.h>
#include "base_module.hpp"

// Define pins
/**
 * @deprecated
 */
#define LINE_COLOR_TRACKER_S0 1 // Select frequency pins
/**
 * @deprecated
 */
#define LINE_COLOR_TRACKER_S1 1 // Select frequency pins

#define LINE_COLOR_TRACKER_S2 25 // Filter color pins
#define LINE_COLOR_TRACKER_S3 26 // Filter color pins
#define LINE_COLOR_TRACKER_SENSOR_OUT 27

#define LINE_COLOR_TRACKER_DELAY 200 // MS

class LineColorTracker : public BaseModule
{
private:
  const int MIN_RED = 5;
  const int MAX_RED = 38;
  const int MIN_GREEN = 4;
  const int MAX_GREEN = 42;
  const int MIN_BLUE = 4;
  const int MAX_BLUE = 35;

  int priority;

  int getRed();
  int getGreen();
  int getBlue();

  void printColor(int red, int green, int blue);

  void taskFn() override;
  // static void taskWrapper(void *pvParameter);

public:
  LineColorTracker(int priority = DEFAULT_TASK_PRIORITY);
  ~LineColorTracker();

  // void run();
};

extern LineColorTracker *lineColorTracker;
#endif
