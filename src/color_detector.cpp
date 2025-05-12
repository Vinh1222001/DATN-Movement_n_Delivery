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
  this->color = SetUtils::initMutexData<ColorRGB>({0, 0, 0, ColorSet::NONE});

  // pinMode(COLOR_DETECTOR_PIN_S0, OUTPUT);
  // pinMode(COLOR_DETECTOR_PIN_S1, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_S2, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_S3, OUTPUT);
  pinMode(COLOR_DETECTOR_PIN_SENSOR_OUT, INPUT);

  // digitalWrite(COLOR_DETECTOR_PIN_S0, HIGH);
  // digitalWrite(COLOR_DETECTOR_PIN_S1, LOW);
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

void ColorDetector::printColor(ColorRGB color, Monitor *monitor)
{
  String colorStr = ColorDetector::colorToString(color.color);
  // monitor->display(MONITOR_LINE_COLOR_DETECTOR,
  //                  "C:%s[%d,%d,%d]",
  //                  colorStr.c_str(),
  //                  constrain(color.red, 0, 255),
  //                  constrain(color.green, 0, 255),
  //                  constrain(color.blue, 0, 255));

  ESP_LOGI(
      "PRINT_COLOR",
      "Color:%s, RGB code: %d, %d, %d",
      colorStr.c_str(),
      color.red,
      color.green,
      color.blue);
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

  ColorRGB color = ColorDetector::detectColor(red, green, blue);

  SetUtils::setMutexData<ColorRGB>(this->color, color);
  IS_NULL(this->monitor);
  this->printColor(color, this->monitor);

  IS_NULL(this->webSocketClient);
  ColorDetectorData colorDetectorData = {
      .red = red,
      .green = green,
      .blue = blue,
      .color = this->colorToString(color.color)};

  this->webSocketClient->setColorDetectorData(colorDetectorData);
}

ColorRGB ColorDetector::getColor()
{
  ColorRGB color;

  GetUtils::getMutexData<ColorRGB>(
      this->color,
      [&](ColorRGB value)
      {
        color = value;
      });

  return color;
}

ColorRGB ColorDetector::detectColor(int r, int g, int b)
{
  ColorRGB color = {
      .red = r,
      .green = g,
      .blue = b,
      .color = ColorSet::NONE};
  if (
      CompareUtils::isInConstraint<int>(r, 150, 256) &&
      CompareUtils::isInConstraint<int>(g, 0, 50) &&
      CompareUtils::isInConstraint<int>(b, 0, 100))
  {
    color.color = ColorSet::RED;
  }
  else if (
      CompareUtils::isInConstraint<int>(r, 150, 200) &&
      CompareUtils::isInConstraint<int>(g, 150, 256) &&
      CompareUtils::isInConstraint<int>(b, 130, 160))
  {
    color.color = ColorSet::GREEN;
  }
  else if (
      CompareUtils::isInConstraint<int>(r, 0, 60) &&
      CompareUtils::isInConstraint<int>(g, 0, 160) &&
      CompareUtils::isInConstraint<int>(b, 190, 256))
  {
    color.color = ColorSet::BLUE;
  }
  else if (
      CompareUtils::isInConstraint<int>(r, 200, 256) &&
      CompareUtils::isInConstraint<int>(g, 200, 256) &&
      CompareUtils::isInConstraint<int>(b, 150, 200))
  {
    color.color = ColorSet::YELLOW;
  }

  return color;
}

String ColorDetector::colorToString(ColorSet color)
{
  switch (color)
  {
  case ColorSet::RED:
    return String("RED");
  case ColorSet::GREEN:
    return String("GREEN");
  case ColorSet::BLUE:
    return String("BLUE");
  case ColorSet::YELLOW:
    return String("YELLOW");
  default:
    return String("NONE");
  }
}
