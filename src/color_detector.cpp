#include "color_detector.hpp"

ColorDetector::ColorDetector()
{
  pinMode(COLOR_DETECTOR_S2, OUTPUT);
  pinMode(COLOR_DETECTOR_S3, OUTPUT);
  pinMode(COLOR_DETECTOR_SENSOR_OUT, INPUT);
}
ColorDetector::~ColorDetector() {}

int ColorDetector::getRed()
{
  digitalWrite(COLOR_DETECTOR_S2, LOW);
  digitalWrite(COLOR_DETECTOR_S3, LOW);
  return pulseIn(COLOR_DETECTOR_SENSOR_OUT, LOW);
}

int ColorDetector::getGreen()
{
  digitalWrite(COLOR_DETECTOR_S2, HIGH);
  digitalWrite(COLOR_DETECTOR_S3, HIGH);
  return pulseIn(COLOR_DETECTOR_SENSOR_OUT, LOW);
}

int ColorDetector::getBlue()
{
  digitalWrite(COLOR_DETECTOR_S2, LOW);
  digitalWrite(COLOR_DETECTOR_S3, HIGH);
  return pulseIn(COLOR_DETECTOR_SENSOR_OUT, LOW);
}

void ColorDetector::taskFn()
{
  int red = map(this->getRed(), this->MIN_RED, this->MAX_RED, 255, 0);
  delay(COLOR_DETECTOR_DELAY);

  int green = map(this->getGreen(), this->MIN_GREEN, this->MAX_GREEN, 255, 0);
  delay(COLOR_DETECTOR_DELAY);

  int blue = map(this->getBlue(), this->MIN_BLUE, this->MAX_BLUE, 255, 0);
  delay(COLOR_DETECTOR_DELAY);

  Serial.printf("Red: %d, Green: %d, Blue: %d", red, green, blue);
}

void ColorDetector::taskWrapper(void *pvParameter)
{
  ColorDetector *instance = static_cast<ColorDetector *>(pvParameter);
  while (true)
  {
    instance->taskFn();
    delay(1000);
  }
}