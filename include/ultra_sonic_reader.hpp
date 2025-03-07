#pragma once
#ifndef ULTRA_SONIC_READER_HPP
#define ULTRA_SONIC_READER_HPP

#include <Arduino.h>
#include "base_module.hpp"

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
  UltraSonicReader();
  ~UltraSonicReader();
};

extern UltraSonicReader *ultraSonicReader;

#endif
