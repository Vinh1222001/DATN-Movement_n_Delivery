#include "color_detector.hpp"

ColorDetector::ColorDetector(
    Monitor *monitor,
    RWebSocketClient *webSocketClient)
    : BaseModule(
          "COLOR_DETECTOR",
          COLOR_DETECTOR_TASK_PRIORITY,
          COLOR_DETECTOR_TASK_DELAY,
          COLOR_DETECTOR_TASK_STACK_DEPTH_LEVEL,
          COLOR_DETECTOR_TASK_PINNED_CORE_ID),
      monitor(monitor),
      webSocketClient(webSocketClient)
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
  ColorRGB colorElement;
  if (xSemaphoreTake(this->color.xMutex, portMAX_DELAY) == pdTRUE)
  {
    colorElement = this->color.value;
    xSemaphoreGive(this->color.xMutex);
  }
  ColorSet colorCode = this->detectColor(
      colorElement.red,
      colorElement.green,
      colorElement.blue);

  String color = this->colorToString(colorCode);
  this->monitor->display(MONITOR_LINE_COLOR_DETECTOR,
                         "Color: %s (R%d, G%d, B%d)",
                         color.c_str(),
                         colorElement.red,
                         colorElement.green,
                         colorElement.blue);

  ESP_LOGI(
      this->NAME,
      "Color:%s, RGB code: %d, %d, %d",
      color.c_str(),
      colorElement.red,
      colorElement.green,
      colorElement.blue);
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

  ColorSet color = this->detectColor(red, green, blue);

  if (xSemaphoreTake(this->color.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->color.value = {
        .red = red,
        .green = green,
        .blue = blue,
        .color = color};
    xSemaphoreGive(this->color.xMutex);
  }

  if (this->webSocketClient != nullptr)
  {
    ColorDetectorData colorDetectorData = {
        .red = red,
        .green = green,
        .blue = blue,
        .color = this->colorToString(color)};

    this->webSocketClient->setColorDetectorData(colorDetectorData);
  }

  this->printColor();
}

ColorRGB ColorDetector::getColor()
{
  ColorRGB color;
  if (xSemaphoreTake(this->color.xMutex, portMAX_DELAY) == pdTRUE)
  {
    color = this->color.value;
    xSemaphoreGive(this->color.xMutex);
  }

  return color;
}

ColorSet ColorDetector::detectColor(int r, int g, int b)
{
  if (
      CompareUtils::isInConstraint<int>(r, 150, 256) &&
      CompareUtils::isInConstraint<int>(g, 0, 50) &&
      CompareUtils::isInConstraint<int>(b, 0, 100))
  {
    return RED;
  }
  else if (
      CompareUtils::isInConstraint<int>(r, 150, 200) &&
      CompareUtils::isInConstraint<int>(g, 150, 256) &&
      CompareUtils::isInConstraint<int>(b, 130, 160))
  {
    return GREEN;
  }
  else if (
      CompareUtils::isInConstraint<int>(r, 0, 50) &&
      CompareUtils::isInConstraint<int>(g, 0, 150) &&
      CompareUtils::isInConstraint<int>(b, 190, 256))
  {
    return BLUE;
  }
  else if (
      CompareUtils::isInConstraint<int>(r, 200, 256) &&
      CompareUtils::isInConstraint<int>(g, 200, 256) &&
      CompareUtils::isInConstraint<int>(b, 150, 200))
  {
    return YELLOW;
  }
  else
  {
    return NONE;
  }
}

String ColorDetector::colorToString(ColorSet color)
{
  switch (color)
  {
  case RED:
    return String("RED");
  case GREEN:
    return String("GREEN");
  case BLUE:
    return String("BLUE");
  case YELLOW:
    return String("YELLOW");
  default:
    return String("NONE");
  }
}
