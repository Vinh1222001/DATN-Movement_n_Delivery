#pragma once
#ifndef ULTRA_SONIC_READER_HPP
#define ULTRA_SONIC_READER_HPP

#include <Arduino.h>

#define TRIGGER_PIN 12
#define ECHO_PIN    34

#define SOUND_SPEED 0.034
#define CM_TO_INCH  0.393701

#define DELAY_FOR_READ_VALUE 500

typedef struct ultra_sonic{
    volatile long duration;
    float distance;
} distance_reader;

extern distance_reader ultra_sonic_reader;

/**
 * @brief Using for initializing ultra sonic reader
 */
void ultra_sonic_reader_init(void);

/**
 * @brief Using as a task. This function is responsible for reading data from sensor
 */
void ultra_sonic_read(void* arg);

#endif
