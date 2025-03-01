#pragma once
#ifndef MPU_READER_HPP
#define MPU_READER_HPP

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

extern Adafruit_MPU6050* mpu_reader;

/**
 * @brief Using for initializing MPU sensor and ready for reading data
 */
void mpu_reader_init(void);

/**
 * @brief Using  as a task. This function is responsible for reading data from the sensor
 */
void mpu_read(void* arg); 

#endif
