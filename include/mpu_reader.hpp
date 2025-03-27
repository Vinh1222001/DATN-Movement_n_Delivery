#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

#include "base_module.hpp"
#include "types.hpp"

#include "monitor.hpp"

#define MPU_READER_ACCELERATION_X_THRESHOLD 1.11
#define MPU_READER_ACCELERATION_Y_THRESHOLD 0.13
#define MPU_READER_ACCELERATION_Z_THRESHOLD 0
#define MPU_READER_VELOCITY_X_THRESHOLD 0
#define MPU_READER_VELOCITY_Y_THRESHOLD 0
#define MPU_READER_VELOCITY_Z_THRESHOLD 0
#define MPU_READER_GYROSCOPE_X_THRESHOLD 0.09
#define MPU_READER_GYROSCOPE_Y_THRESHOLD 0.09
#define MPU_READER_GYROSCOPE_Z_THRESHOLD 0

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

  template <typename T>
  T setWithThreshold(T value, T threshold);
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
