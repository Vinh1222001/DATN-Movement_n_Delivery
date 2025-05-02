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

  this->robotStateStr = SetUtils::initMutexData<String>("INIT");

  this->monitor->begin();
  this->monitor->setFont(MONITOR_FONT);

  this->ui.value.fontHeight = this->monitor->getMaxCharHeight();
  this->ui.value.fontWidth = this->monitor->getMaxCharWidth();
  this->ui.value.numOflines = MONITOR_SCREEN_HEIGHT / (this->ui.value.fontHeight + MONITOR_LINE_SPACING);
  this->ui.value.buffer = std::vector<std::string>(this->ui.value.numOflines, "");

  pinMode(MONITOR_POWER_PIN, INPUT);
  analogReadResolution(12);

  ESP_LOGI(this->NAME, "Monitor initialized with %d lines", this->ui.value.numOflines);
}

Monitor::~Monitor()
{
  delete this->monitor;
}

void Monitor::taskFn()
{
  this->displaySystemLine();
  Screen ui;
  if (xSemaphoreTake(this->ui.xMutex, portMAX_DELAY) == pdTRUE)
  {
    ui = this->ui.value;
    xSemaphoreGive(this->ui.xMutex);
  }
  this->monitor->clearBuffer();
  this->monitor->setFont(MONITOR_FONT);

  for (int i = 0; i < ui.numOflines; i++)
  {
    this->monitor->setCursor(0, (i + 1) * (ui.fontHeight + MONITOR_LINE_SPACING));
    this->monitor->print(ui.buffer[i].c_str());
  }

  this->monitor->sendBuffer();
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

void Monitor::setRobotState(String state)
{
  SetUtils::setMutexData<String>(this->robotStateStr, state);
}

void Monitor::displaySystemLine()
{
  String controllerStateStr;
  GetUtils::getMutexData<String>(
      this->robotStateStr,
      [&](String value)
      {
        controllerStateStr = value;
      });
  int supplyLevel = analogRead(GPIO_NUM_34);

  String supplyLevelStr(map(supplyLevel, 0, 4096, 0, 100));
  supplyLevelStr = "B:" + supplyLevelStr + "%";
  uint32_t controllerStateStrLen = this->monitor->getStrWidth(controllerStateStr.c_str());
  uint32_t supplyLevelStrLen = this->monitor->getStrWidth(supplyLevelStr.c_str());
  uint32_t numOfSpace = (this->monitor->getDisplayWidth() - controllerStateStrLen - supplyLevelStrLen) / this->monitor->getStrWidth(" ");

  String content = controllerStateStr;
  for (uint32_t i = 0; i < numOfSpace - 1; i++)
  {
    content += " ";
  }
  content += supplyLevelStr;

  this->display(0, "%s", content.c_str());
}