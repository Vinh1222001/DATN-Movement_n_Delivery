#pragma once
#ifndef LINE_FOLLOWER_HPP
#define LINE_FOLLOWER_HPP

#include <Arduino.h>

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

typedef struct line_follow_values
{
  int out1;
  int out2;
#if LINE_FOLLOWER_VERSION == 1
  int out3;
  int out4;
  int out5;
#endif
} t_line_follow_values;

extern t_line_follow_values line_reader;

class LineFollower
{
private:
  t_line_follow_values line_reader;

public:
  LineFollower();
  ~LineFollower();
  void taskFn();
  static void taskWrapper(void *pvParameter);
};

#endif
