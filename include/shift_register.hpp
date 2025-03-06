#pragma once
#ifndef SHIFT_REGISTER_HPP
#define SHIFT_REGISTER_HPP

#include <Arduino.h>

#define LATCH_PIN 19
#define CLOCK_PIN 18
#define DS_PIN 23
class ShiftRegister
{
private:
  int priority;

  void taskFn(uint8_t value);
  static void taskWrapper(void *pvParameter);

public:
  ShiftRegister(int priority = DEFAULT_TASK_PRIORITY);
  ~ShiftRegister();

  uint8_t genValue(const bool signals[8]);

  void run();
};

extern ShiftRegister *shiftRegister;

#endif
