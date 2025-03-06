#include "shift_register.hpp"

ShiftRegister::ShiftRegister(int priority) : BaseModule("SHIFT_REGISTER", priority)
{
	this->value = 0;
	pinMode(LATCH_PIN, OUTPUT);
	pinMode(CLOCK_PIN, OUTPUT);
	pinMode(DS_PIN, OUTPUT);
}

ShiftRegister::~ShiftRegister()
{
}

uint8_t ShiftRegister::genValue(const bool signals[8])
{
	uint8_t value = 0;

	for (int i = 0; i < 8; i++)
	{
		value |= (signals[i] ? 1 : 0) << i; // Set each bit based on the input array
	}

	return value;
}

void ShiftRegister::taskFn()
{
	digitalWrite(LATCH_PIN, LOW);
	shiftOut(DS_PIN, CLOCK_PIN, MSBFIRST, this->value);
	digitalWrite(LATCH_PIN, HIGH);
}

void ShiftRegister::setShiftedValue(int value)
{
	this->value = value;
}

void ShiftRegister::setShiftedValue(bool *states)
{
	this->value = genValue(states);
}

int ShiftRegister::getShiftedValue()
{
	return this->value;
}

// void ShiftRegister::taskWrapper(void *pvParameter)
// {
// 	ShiftRegister *instance = static_cast<ShiftRegister *>(pvParameter);
// 	if (instance == nullptr)
// 	{
// 		Serial.println("Error: taskWrapper received a null instance");
// 		vTaskDelete(nullptr); // Terminate task safely
// 	}
// 	bool value[4][8] = {
// 			{0, 0, 0, 0, 0, 0, 0, 0},
// 			{0, 0, 0, 0, 0, 0, 0, 1},
// 			{0, 0, 0, 0, 0, 0, 1, 0},
// 			{0, 0, 0, 0, 0, 0, 1, 1}};
// 	int index = 0;
// 	while (true)
// 	{
// 		uint8_t shiftValue = instance->genValue(value[index]);
// 		Serial.printf("value: %d\n", shiftValue);
// 		instance->taskFn(shiftValue);
// 		index = (index < 3) ? index + 1 : 0;
// 		Serial.printf("index: %d\n", index);
// 		delay(1000);
// 	}
// }

// void ShiftRegister::run()
// {
// 	if (xTaskCreatePinnedToCore(taskWrapper, "SHIFT_REGISTER", BASE_STACK_DEEP * 4, this, this->priority, nullptr, 0) == pdPASS)
// 	{
// 		Serial.printf("SHIFT REGISTER: created task SUCCESSFULLY\n");
// 	}
// 	else
// 	{
// 		Serial.printf("SHIFT REGISTER: created task FAILED\n");
// 		while (true)
// 		{
// 		}
// 	}
// }

ShiftRegister *shiftRegister;