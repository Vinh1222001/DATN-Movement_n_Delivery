#pragma once
#ifndef COLOR_DETECTOR_HPP
#define COLOR_DETECTOR_HPP

#include <Arduino.h>
#include <algorithm>
#include "base_module.hpp"

#define COLOR_DETECTOR_PIN_S0 GPIO_NUM_0
#define COLOR_DETECTOR_PIN_S1 GPIO_NUM_0
#define COLOR_DETECTOR_PIN_S2 GPIO_NUM_0
#define COLOR_DETECTOR_PIN_S3 GPIO_NUM_0
#define COLOR_DETECTOR_PIN_SENSOR_OUT GPIO_NUM_0

class ColorDetector : public BaseModule
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
  ColorDetector();
  ~ColorDetector();
};

#endif
