#include "color_detector.hpp"

ColorDetector::ColorDetector()
    : BaseModule(
          "COLOR_DETECTOR",
          LINE_COLOR_TRACKER_TASK_PRIORITY,
          LINE_COLOR_TRACKER_TASK_DELAY,
          LINE_COLOR_TRACKER_TASK_STACK_DEPTH_LEVEL,
          LINE_COLOR_TRACKER_TASK_PINNED_CORE_ID)
{

  this->color.value = {0, 0, 0};
  this->color.xMutex = xSemaphoreCreateMutex();

  pinMode(COLOR_DETECTOR_PIN_S0, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_S1, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_S2, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_S3, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_SENSOR_OUT, INPUT);

  digitalWrite(COLOR_DETECTOR_PIN_S0, HIGH);
  digitalWrite(COLOR_DETECTOR_PIN_S1, LOW);
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

void ColorDetector::printColor()
{
  if (xSemaphoreTake(this->color.xMutex, portMAX_DELAY) == pdTRUE)
  {
    int maxValue = std::max(
        {this->color.value.red,
         this->color.value.green,
         this->color.value.blue});

    const char *color = (maxValue == this->color.value.red)
                            ? "Red"
                        : (maxValue == this->color.value.green)
                            ? "Green"
                            : "Blue";

    // Print colored output
    ESP_LOGI(
        this->NAME,
        "Color:%s, RGB code: %d, %d, %d\n",
        color,
        this->color.value.red,
        this->color.value.green,
        this->color.value.blue);

    xSemaphoreGive(this->color.xMutex);
  }
}

void ColorDetector::taskFn()
{

  int rawRed = constrain(this->getRed(), this->MIN_RED, this->MAX_RED);
  int red = map(rawRed, this->MIN_RED, this->MAX_RED, 255, 0);
  delay(LINE_COLOR_TRACKER_FILTER_DELAY);

  int rawGreen = constrain(this->getGreen(), this->MIN_GREEN, this->MAX_GREEN);
  int green = map(rawGreen, this->MIN_GREEN, this->MAX_GREEN, 255, 0);
  delay(LINE_COLOR_TRACKER_FILTER_DELAY);

  int rawBlue = constrain(this->getBlue(), this->MIN_BLUE, this->MAX_BLUE);
  int blue = map(rawBlue, this->MIN_BLUE, this->MAX_BLUE, 255, 0);
  delay(LINE_COLOR_TRACKER_FILTER_DELAY);

  if (xSemaphoreTake(this->color.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->color.value = {
        .red = red,
        .green = green,
        .blue = blue};
    xSemaphoreGive(this->color.xMutex);
  }

  this->printColor();
}