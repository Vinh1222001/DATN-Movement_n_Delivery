#pragma once
#ifndef CONTROLLER_HPP
#define CONTROLLER_HPP

#include "base_module.hpp"

class Controller : public BaseModule
{
private:
  int state;

  void stateMachine();
  void taskFn() override;

public:
  Controller();
  ~Controller();
};

#endif
