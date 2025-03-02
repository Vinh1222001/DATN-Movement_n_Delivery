#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

class BaseTask
{
public:
  BaseTask();
  ~BaseTask();

  virtual void TaskFn() = 0;
  static void TaskWrapper();
};

#endif
