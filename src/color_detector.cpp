#include "color_detector.hpp"

ColorDetector::ColorDetector(Monitor *monitor)
    : BaseModule(
          "COLOR_DETECTOR",
          COLOR_DETECTOR_TASK_PRIORITY,
          COLOR_DETECTOR_TASK_DELAY,
          COLOR_DETECTOR_TASK_STACK_DEPTH_LEVEL,
          COLOR_DETECTOR_TASK_PINNED_CORE_ID)
{
  pinMode(COLOR_DETECTOR_PIN_S2, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_S3, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_SENSOR_OUT, INPUT);

  this->monitor = monitor;
}
ColorDetector::~ColorDetector() {}

int ColorDetector::getRed()
{
  digitalWrite(COLOR_DETECTOR_PIN_S2, LOW);
  digitalWrite(COLOR_DETECTOR_PIN_S3, LOW);
  return pulseIn(COLOR_DETECTOR_PIN_SENSOR_OUT, LOW);
}

int ColorDetector::getGreen()
{
  digitalWrite(COLOR_DETECTOR_PIN_S2, HIGH);
  digitalWrite(COLOR_DETECTOR_PIN_S3, HIGH);
  return pulseIn(COLOR_DETECTOR_PIN_SENSOR_OUT, LOW);
}

int ColorDetector::getBlue()
{
  digitalWrite(COLOR_DETECTOR_PIN_S2, LOW);
  digitalWrite(COLOR_DETECTOR_PIN_S3, HIGH);
  return pulseIn(COLOR_DETECTOR_PIN_SENSOR_OUT, LOW);
}

void ColorDetector::printColor(int red, int green, int blue)
{
  int maxValue = std::max({red, green, blue});

  const char *color = (maxValue == red) ? "Red" : (maxValue == green) ? "Green"
                                                                      : "Blue";

  // Print colored output
  ESP_LOGI(this->NAME, "Color:%s, RGB code: %d, %d, %d\n", color, red, green, blue);
  this->monitor->display(0, "Color: %s", color);
  this->monitor->display(1, "RGB: %d, %d, %d", red, green, blue);
}

void ColorDetector::taskFn()
{
  int rawRed = constrain(this->getRed(), this->MIN_RED, this->MAX_RED);
  int red = map(rawRed, this->MIN_RED, this->MAX_RED, 255, 0);
  delay(COLOR_DETECTOR_FILTER_DELAY);

  int rawGreen = constrain(this->getGreen(), this->MIN_GREEN, this->MAX_GREEN);
  int green = map(rawGreen, this->MIN_GREEN, this->MAX_GREEN, 255, 0);
  delay(COLOR_DETECTOR_FILTER_DELAY);

  int rawBlue = constrain(this->getBlue(), this->MIN_BLUE, this->MAX_BLUE);
  int blue = map(rawBlue, this->MIN_BLUE, this->MAX_BLUE, 255, 0);
  delay(COLOR_DETECTOR_FILTER_DELAY);

  // Serial.printf("Red: %d, Green: %d, Blue: %d\n\n", red, green, blue);
  this->printColor(red, green, blue);
}