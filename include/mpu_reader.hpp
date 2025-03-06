#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "base_module.hpp"

class MPUReader : public BaseModule
{
private:
  Adafruit_MPU6050 *sensor;

  void setup();
  void init();
  void taskFn() override;

public:
  MPUReader(int priority = DEFAULT_TASK_PRIORITY);
  ~MPUReader();
};

extern MPUReader *mpuReader;

#endif
