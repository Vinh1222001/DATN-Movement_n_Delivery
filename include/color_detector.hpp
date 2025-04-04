#pragma once
#ifndef COLOR_DETECTOR_HPP
#define COLOR_DETECTOR_HPP

#include <Arduino.h>
#include <algorithm>
#include "base_module.hpp"
#include "types.hpp"
#include "monitor.hpp"

#define COLOR_DETECTOR_PIN_S0 GPIO_NUM_4
#define COLOR_DETECTOR_PIN_S1 GPIO_NUM_5
#define COLOR_DETECTOR_PIN_S2 GPIO_NUM_18
#define COLOR_DETECTOR_PIN_S3 GPIO_NUM_19
#define COLOR_DETECTOR_PIN_SENSOR_OUT GPIO_NUM_14

struct ColorRGB
{
  int red;
  int green;
  int blue;
};

using ColorDetectorValue = SemaphoreMutexData<ColorRGB>;

class ColorDetector : public BaseModule
{
private:
  static constexpr int MIN_RED = 0, MAX_RED = 255;
  static constexpr int MIN_GREEN = 0, MAX_GREEN = 255;
  static constexpr int MIN_BLUE = 0, MAX_BLUE = 255;

  ColorDetectorValue color;

  Monitor *monitor;

  int getRed();
  int getGreen();
  int getBlue();

  void printColor();

  void taskFn() override;

public:
  ColorDetector(Monitor *monitor);
  ~ColorDetector();

  ColorRGB getColor();
};

#endif
