#pragma once
#ifndef BASE_MODULE_HPP
#define BASE_MODULE_HPP

#include <Arduino.h>

class BaseModule
{
protected:
  const char *NAME;    // Tên module
  int TASK_DELAY;      // Thời gian delay của task
  int priority;        // Độ ưu tiên của task
  int stackDepthLevel; // Độ sâu stack
  int cpuCore;         // Core chạy task
private:
public:
  BaseModule(const char *name, int priority = DEFAULT_TASK_PRIORITY, int taskDelay = 1000, int stackDepthLevel = 1, int cpuCore = 0);
  virtual ~BaseModule();

  virtual void taskFn() = 0;
  static void taskWrapper(void *pvParameter);
  void run();
};

#endif
