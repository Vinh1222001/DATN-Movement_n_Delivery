#pragma once
#ifndef GLOBAL_HPP
#define GLOBAL_HPP

#include <Arduino.h>

template <typename T>
struct SemaphoreMutexData
{
  T value;
  SemaphoreHandle_t xMutex;
};

template <typename T>
struct SemaphoreQueueData
{
  T sendData;
  T recieveDate;
  SemaphoreHandle_t xQueue;
};

#endif
