#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "base_module.hpp"
#include "types.hpp"

#include "monitor.hpp"

#define ACCEL_THRESHOLD 0.02
#define SIMULATE_FRICTION 0.98

struct VectorElements
{
  float x = 0;
  float y = 0;
  float z = 0;

  float resultant() const
  {
    return sqrt(x * x + y * y + z * z);
  }
};

using Velocity = VectorElements;
using Acceleration = VectorElements;
using Gyroscope = VectorElements;

struct MotionState
{
  unsigned long lastTime = 0;
  Velocity velocity;
  Acceleration acceleration;
  Gyroscope gyroscope;
};

using MpuReaderData = SemaphoreMutexData<MotionState>;

class MPUReader : public BaseModule
{
private:
  Adafruit_MPU6050 *sensor = nullptr;

  Monitor *monitor = nullptr;

  MpuReaderData data;

  void setup();
  void init();
  void taskFn() override;

  void computeVelocity();

  void setData();

public:
  MPUReader(Monitor *monitor);
  ~MPUReader();

  MotionState getMpuReaderData();
  Gyroscope getGyroData();
  Acceleration getAccelerationData();
  Velocity getVelocity();
};

#endif
