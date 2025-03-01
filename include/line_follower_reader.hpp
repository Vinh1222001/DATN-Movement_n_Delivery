#pragma once
#ifndef LINE_FOLLOWER_READER_HPP
#define LINE_FOLLOWER_READER_HPP

#include <Arduino.h>

#define LINE_FOLLOWER_OUT1_PIN  32
#define LINE_FOLLOWER_OUT2_PIN  33
#define LINE_FOLLOWER_OUT3_PIN  25
#define LINE_FOLLOWER_OUT4_PIN  26
#define LINE_FOLLOWER_OUT5_PIN  27

#define THRESHOLE           2000

typedef struct line_follow_values{
    int out1;
    int out2;
    int out3;
    int out4;
    int out5;
} t_line_follow_values;

extern t_line_follow_values line_reader;

/**
 * @brief Using for initializing all is pin at input mode
 */
void line_follower_reader_init(void);

/**
 * @brief using as a task. This function is responsible for reading data from sensors.
 */
void line_follower_read(void* arg);

#endif
