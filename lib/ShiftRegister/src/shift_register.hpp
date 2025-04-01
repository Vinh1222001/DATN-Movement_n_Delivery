#pragma once
#ifndef SHIFT_REGISTER_HPP
#define SHIFT_REGISTER_HPP

#include <Arduino.h>
class ShiftRegister
{
private:
  int value;
  uint8_t genValue(const bool signals[8]);

public:
  ShiftRegister();
  ~ShiftRegister();

  void setShiftedValue(int value);
  void setShiftedValue(const bool states[8]);
  int getShiftedValue();

  void shift();
};

#endif
