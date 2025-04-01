#include "shift_register.hpp"

ShiftRegister::ShiftRegister()
{
	this->value = 0;
	pinMode(SHIFT_REGISTER_PIN_LATCH, OUTPUT);
	pinMode(SHIFT_REGISTER_PIN_CLOCK, OUTPUT);
	pinMode(SHIFT_REGISTER_PIN_DS, OUTPUT);
}

ShiftRegister::~ShiftRegister()
{
}

uint8_t ShiftRegister::genValue(const bool signals[8])
{
	uint8_t value = 0;

	for (int i = 0; i < 8; i++)
	{
		value |= (signals[i] ? 1 : 0) << i;
	}

	return value;
}

void ShiftRegister::setShiftedValue(int value)
{
	this->value = value;
}

void ShiftRegister::setShiftedValue(const bool states[8])
{
	this->value = genValue(states);
}

int ShiftRegister::getShiftedValue()
{
	return this->value;
}

void ShiftRegister::shift()
{
	digitalWrite(SHIFT_REGISTER_PIN_LATCH, LOW);
	shiftOut(SHIFT_REGISTER_PIN_DS, SHIFT_REGISTER_PIN_CLOCK, MSBFIRST, this->value);
	digitalWrite(SHIFT_REGISTER_PIN_LATCH, HIGH);
}
ShiftRegister *shiftRegister;