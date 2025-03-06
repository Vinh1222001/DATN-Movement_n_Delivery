#pragma once
#ifndef LINE_FOLLOWER_HPP
#define LINE_FOLLOWER_HPP

#include <Arduino.h>

#include "base_module.hpp"

#ifndef LINE_FOLLOWER_VERSION
#define LINE_FOLLOWER_VERSION 1
#endif

#define LINE_FOLLOWER_OUT1_PIN 25
#define LINE_FOLLOWER_OUT2_PIN 26
#if LINE_FOLLOWER_VERSION == 1
#define LINE_FOLLOWER_OUT3_PIN 32
#define LINE_FOLLOWER_OUT4_PIN 33
#define LINE_FOLLOWER_OUT5_PIN 27
#endif

// #define LINE_FOLLOWER_OUT1_PIN_V1 32
// #define LINE_FOLLOWER_OUT2_PIN_V1 33
// #define LINE_FOLLOWER_OUT3_PIN_V1 25
// #define LINE_FOLLOWER_OUT4_PIN_V1 26
// #define LINE_FOLLOWER_OUT5_PIN_V1 27

// #define LINE_FOLLOWER_OUT1_PIN_V2 25
// #define LINE_FOLLOWER_OUT2_PIN_V2 26

#define THRESHOLE 2000

typedef struct
{
  int out1;
  int out2;
#if LINE_FOLLOWER_VERSION == 1
  int out3;
  int out4;
  int out5;
#endif
} t_lineFollowerValues;

extern t_lineFollowerValues line_reader;

class LineFollower : public BaseModule
{
private:
  t_lineFollowerValues line_reader;

  void taskFn() override;

public:
  LineFollower(int priority = DEFAULT_TASK_PRIORITY);
  ~LineFollower();
};

extern LineFollower *lineFollower;

#endif
