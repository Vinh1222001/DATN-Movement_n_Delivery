#pragma once
#ifndef CLASSIFYING_COMMUNICATE_HPP
#define CLASSIFYING_COMMUNICATE_HPP

#include <Arduino.h>
#include "base_module.hpp"

#define CLASSIFYING_COMMUNICATE_PIN_RXD GPIO_NUM_16
#define CLASSIFYING_COMMUNICATE_PIN_TXD GPIO_NUM_17

class ClassifyingCommunicate : public BaseModule
{
private:
  HardwareSerial *communicate;
  void taskFn() override;

public:
  ClassifyingCommunicate();
  ~ClassifyingCommunicate();
};

#endif
