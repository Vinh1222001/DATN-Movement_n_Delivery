#pragma once
#ifndef SHIFT_REGISTER_HPP
#define SHIFT_REGISTER_HPP

#include <Arduino.h>

#include "base_module.hpp"

#define LATCH_PIN 19
#define CLOCK_PIN 18
#define DS_PIN 23

class ShiftRegister : public BaseModule
{
private:
  int value;
  void taskFn() override;
  uint8_t genValue(const bool signals[8]);

public:
  ShiftRegister(int priority = DEFAULT_TASK_PRIORITY);
  ~ShiftRegister();

  void setShiftedValue(int value);
  void setShiftedValue(bool states[8]);
  int getShiftedValue();
};

extern ShiftRegister *shiftRegister;

#endif
