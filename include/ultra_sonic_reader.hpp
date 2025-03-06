#pragma once
#ifndef ULTRA_SONIC_READER_HPP
#define ULTRA_SONIC_READER_HPP

#include <Arduino.h>
#include "base_module.hpp"

#define TRIGGER_PIN 12
#define ECHO_PIN 34

#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

#define DELAY_FOR_READ_VALUE 500

typedef struct
{
  volatile long duration;
  float distance;
} t_distance_values;

class UltraSonicReader : public BaseModule
{
private:
  t_distance_values value;
  void taskFn() override;

public:
  UltraSonicReader(int priority = DEFAULT_TASK_PRIORITY);
  ~UltraSonicReader();
};

extern UltraSonicReader *ultraSonicReader;

#endif
