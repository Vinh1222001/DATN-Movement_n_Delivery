#include "line_color_tracker.hpp"

LineColorTracker::LineColorTracker(int priority) : priority(priority)
{
  pinMode(LINE_COLOR_TRACKER_S2, OUTPUT);
  pinMode(LINE_COLOR_TRACKER_S3, OUTPUT);
  pinMode(LINE_COLOR_TRACKER_SENSOR_OUT, INPUT);
}
LineColorTracker::~LineColorTracker() {}

int LineColorTracker::getRed()
{
  digitalWrite(LINE_COLOR_TRACKER_S2, LOW);
  digitalWrite(LINE_COLOR_TRACKER_S3, LOW);
  return pulseIn(LINE_COLOR_TRACKER_SENSOR_OUT, LOW);
}

int LineColorTracker::getGreen()
{
  digitalWrite(LINE_COLOR_TRACKER_S2, HIGH);
  digitalWrite(LINE_COLOR_TRACKER_S3, HIGH);
  return pulseIn(LINE_COLOR_TRACKER_SENSOR_OUT, LOW);
}

int LineColorTracker::getBlue()
{
  digitalWrite(LINE_COLOR_TRACKER_S2, LOW);
  digitalWrite(LINE_COLOR_TRACKER_S3, HIGH);
  return pulseIn(LINE_COLOR_TRACKER_SENSOR_OUT, LOW);
}

void LineColorTracker::printColor(int red, int green, int blue)
{
  const char *colorCode;
  if (red > green && red > blue)
  {
    colorCode = "Red";
  }
  else if (green > red && green > blue)
  {
    colorCode = "Green";
  }
  else
  {
    colorCode = "Blue";
  }

  // Print colored output
  Serial.printf("Color:%s, RGB code: %d, %d, %d\n", colorCode, red, green, blue);
}

void LineColorTracker::taskFn()
{
  int rawRed = constrain(this->getRed(), this->MIN_RED, this->MAX_RED);
  int red = map(rawRed, this->MIN_RED, this->MAX_RED, 255, 0);
  delay(LINE_COLOR_TRACKER_DELAY);

  int rawGreen = constrain(this->getGreen(), this->MIN_GREEN, this->MAX_GREEN);
  int green = map(rawGreen, this->MIN_GREEN, this->MAX_GREEN, 255, 0);
  delay(LINE_COLOR_TRACKER_DELAY);

  int rawBlue = constrain(this->getBlue(), this->MIN_BLUE, this->MAX_BLUE);
  int blue = map(rawBlue, this->MIN_BLUE, this->MAX_BLUE, 255, 0);
  delay(LINE_COLOR_TRACKER_DELAY);

  // Serial.printf("Red: %d, Green: %d, Blue: %d\n\n", red, green, blue);
  this->printColor(red, green, blue);
}

void LineColorTracker::taskWrapper(void *pvParameter)
{
  LineColorTracker *instance = static_cast<LineColorTracker *>(pvParameter);
  if (instance == nullptr)
  {
    Serial.println("Error: taskWrapper received a null instance");
    vTaskDelete(nullptr);
  }
  while (true)
  {
    instance->taskFn();
    delay(1000);
  }
}

void LineColorTracker::run()
{
  if (xTaskCreatePinnedToCore(taskWrapper, "LINE_COLOR_TRACKER", BASE_STACK_DEEP * 4, this, this->priority, nullptr, 0) == pdPASS)
  {
    Serial.printf("LINE COLOR TRACKER: created task SUCCESSFULLY\n");
  }
  else
  {
    Serial.printf("LINE COLOR TRACKER: created task FAILED\n");
    while (true)
    {
    }
  }
}

LineColorTracker *lineColorTracker;