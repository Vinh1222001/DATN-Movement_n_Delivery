#include "ultra_sonic_reader.hpp"

UltraSonicReader::UltraSonicReader(int priority) : BaseModule("ULTRA_SONIC_READER", priority)
{
  this->value = {
      .duration = 0,
      .distance = 0};

  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

UltraSonicReader::~UltraSonicReader() {}

void UltraSonicReader::taskFn()
{
  digitalWrite(TRIGGER_PIN, LOW);
  delayMicroseconds(2);
  // Sets the TRIGGER_PIN on HIGH state for 10 micro seconds
  digitalWrite(TRIGGER_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIGGER_PIN, LOW);

  // Reads the ECHO_PIN, returns the sound wave travel time in microseconds
  this->value.duration = pulseIn(ECHO_PIN, HIGH);

  // Calculate the distance in cm
  this->value.distance = this->value.duration * SOUND_SPEED / 2;
}

UltraSonicReader *ultraSonicReader;