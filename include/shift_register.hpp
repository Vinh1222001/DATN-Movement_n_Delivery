#pragma once
#ifndef SHIFT_REGISTER_HPP
#define SHIFT_REGISTER_HPP

#include <Arduino.h>

#define LATCH_PIN   19
#define CLOCK_PIN   18
#define DS_PIN      23

/**
 * @brief Using for initializing shift register
 */

void shift_register_init(void);

/**
 * @brief This function is responsible for shifting the given value
 */
void shift_register_shift(uint8_t value);

#endif
