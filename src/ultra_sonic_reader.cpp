#include "ultra_sonic_reader.hpp"

UltraSonicReader::UltraSonicReader()
    : BaseModule(
          "ULTRA_SONIC_READER",
          ULTRA_SONIC_READER_TASK_PRIORITY,
          ULTRA_SONIC_READER_TASK_DELAY,
          ULTRA_SONIC_READER_TASK_STACK_DEPTH_LEVEL,
          ULTRA_SONIC_READER_TASK_PINNED_CORE_ID)
{
  this->value = {
      .duration = 0,
      .distance = 0};

  pinMode(ULTRA_SONIC_READER_PIN_TRIGGER, OUTPUT);
  pinMode(ULTRA_SONIC_READER_PIN_ECHO, INPUT);
}

UltraSonicReader::~UltraSonicReader() {}

void UltraSonicReader::taskFn()
{
  digitalWrite(ULTRA_SONIC_READER_PIN_TRIGGER, LOW);
  delayMicroseconds(2);
  digitalWrite(ULTRA_SONIC_READER_PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(ULTRA_SONIC_READER_PIN_TRIGGER, LOW);

  this->value.duration = pulseIn(ULTRA_SONIC_READER_PIN_ECHO, HIGH);
  this->value.distance = this->value.duration * ULTRA_SONIC_READER_SOUND_SPEED / 2;
}

UltraSonicReader *ultraSonicReader;