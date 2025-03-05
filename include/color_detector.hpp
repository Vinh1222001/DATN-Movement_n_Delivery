#pragma once
#ifndef COLOR_DETECTOR_HPP
#define COLOR_DETECTOR_HPP

#include <Arduino.h>

// Define pins
#define COLOR_DETECTOR_S2 1
#define COLOR_DETECTOR_S3 1
#define COLOR_DETECTOR_SENSOR_OUT 1

#define COLOR_DETECTOR_DELAY 200 // MS

class ColorDetector
{
private:
  const int MIN_RED = 5;
  const int MAX_RED = 38;
  const int MIN_GREEN = 4;
  const int MAX_GREEN = 42;
  const int MIN_BLUE = 4;
  const int MAX_BLUE = 35;

  int getRed();
  int getGreen();
  int getBlue();

public:
  ColorDetector();
  ~ColorDetector();

  void taskFn();
  static void taskWrapper(void *pvParameter);
};
#endif
