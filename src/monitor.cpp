#include "monitor.hpp"

Monitor::Monitor()
    : BaseModule(
          "MONITOR",
          MONITOR_TASK_PRIORITY,
          MONITOR_TASK_DELAY,
          MONITOR_TASK_STACK_DEPTH_LEVEL,
          MONITOR_TASK_PINNED_CORE_ID)
{
  this->monitor = new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);

  this->ui.xMutex = xSemaphoreCreateMutex();

  this->monitor->begin();
  this->monitor->setFont(MONITOR_FONT);

  this->ui.value.fontHeight = this->monitor->getMaxCharHeight();
  this->ui.value.fontWidth = this->monitor->getMaxCharWidth();
  this->ui.value.numOflines = MONITOR_SCREEN_HEIGHT / (this->ui.value.fontHeight + MONITOR_LINE_SPACING);
  this->ui.value.buffer = std::vector<std::string>(this->ui.value.numOflines, "");

  ESP_LOGI(this->NAME, "Monitor initialized with %d lines", this->ui.value.numOflines);
}

Monitor::~Monitor()
{
  delete this->monitor;
}

void Monitor::taskFn()
{
  if (xSemaphoreTake(this->ui.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->monitor->clearBuffer();
    this->monitor->setFont(MONITOR_FONT);

    for (int i = 0; i < this->ui.value.numOflines; i++)
    {
      this->monitor->setCursor(0, (i + 1) * (this->ui.value.fontHeight + MONITOR_LINE_SPACING));
      this->monitor->print(this->ui.value.buffer[i].c_str());
    }

    xSemaphoreGive(this->ui.xMutex);
    this->monitor->sendBuffer();
  }
}

void Monitor::display(int line, const char *format, ...)
{
  if (line < 0 || line >= this->ui.value.numOflines)
    return;

  char buffer[64];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if (xSemaphoreTake(this->ui.xMutex, portMAX_DELAY) == pdTRUE)
  {
    if (this->ui.value.buffer[line] != buffer)
    {
      this->ui.value.buffer[line] = std::string(buffer);
    }
    xSemaphoreGive(this->ui.xMutex);
  };
}