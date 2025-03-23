#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "base_module.hpp"
#include "types.hpp"

#define ACCEL_THRESHOLD 0.02
#define SIMULATE_FRICTION 0.98

struct Velocity
{
  float x;
  float y;
  float z;
};
class MPUReader : public BaseModule
{
private:
  Adafruit_MPU6050 *sensor = nullptr;
  Adafruit_Sensor *accel = nullptr;
  Adafruit_Sensor *gyro = nullptr;

  unsigned long lastTime = 0;

  Velocity velocity = {0, 0, 0};

  void setup();
  void init();
  void taskFn() override;

  void computeVelocity();

public:
  MPUReader();
  ~MPUReader();
};

#endif
