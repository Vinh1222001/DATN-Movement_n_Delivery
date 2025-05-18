#include "monitor.hpp"

Monitor::Monitor()
    : BaseModule(
          "MONITOR",
          MONITOR_TASK_PRIORITY,
          // MONITOR_TASK_DELAY,
          25,
          MONITOR_TASK_STACK_DEPTH_LEVEL,
          0)
{
  this->monitor = new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, U8X8_PIN_NONE);

  this->ui.xMutex = xSemaphoreCreateMutex();

  this->robotStateStr = SetUtils::initMutexData<String>("INIT");

  this->monitor->begin();
  this->monitor->setFont(MONITOR_FONT);

  this->displayWidth = this->monitor->getDisplayWidth();
  this->fontHeight = this->monitor->getMaxCharHeight();
  this->fontWidth = this->monitor->getMaxCharWidth();
  this->numOflines = MONITOR_SCREEN_HEIGHT / (this->fontHeight + MONITOR_LINE_SPACING);

  for (uint32_t i = 0; i < this->numOflines; i++)
  {
    this->ui.value.push_back("");
  }

  /**
   * @brief Robot State Queue
   */
  this->robotStateSend = "UNKNOW";
  this->robotStateRecv = "INIT";
  this->robotStateQueue = xQueueCreate(10, sizeof(String));

  pinMode(MONITOR_POWER_PIN, INPUT);
  analogReadResolution(12);

  ESP_LOGI(this->NAME, "Monitor initialized with %d lines", this->numOflines);
}

Monitor::~Monitor()
{
  delete this->monitor;
}

void Monitor::taskFn()
{
  this->displaySystemLine();
  std::vector<String> buffer;
  if (xSemaphoreTake(this->ui.xMutex, pdMS_TO_TICKS(10)) == pdTRUE)
  {
    buffer = this->ui.value;
    xSemaphoreGive(this->ui.xMutex);

    this->monitor->clearBuffer();
    this->monitor->setFont(MONITOR_FONT);

    for (int i = 0; i < this->numOflines; i++)
    {
      this->monitor->setCursor(0, (i + 1) * (this->fontHeight + MONITOR_LINE_SPACING));
      this->monitor->print(buffer[i].c_str());
    }

    this->monitor->sendBuffer();
  }
}

void Monitor::display(int line, const char *format, ...)
{
  if (line < 0 || line >= this->numOflines)
    return;

  char buffer[64];
  va_list args;
  va_start(args, format);
  vsnprintf(buffer, sizeof(buffer), format, args);
  va_end(args);

  if (xSemaphoreTake(this->ui.xMutex, pdMS_TO_TICKS(10)) == pdTRUE)
  {
    this->ui.value[line] = String(buffer);
    xSemaphoreGive(this->ui.xMutex);
  };
}

void Monitor::setRobotState(String state)
{
  this->robotStateSend = state;
  if (xQueueSend(this->robotStateQueue, &this->robotStateSend, pdMS_TO_TICKS(1000)) == pdFALSE)
  {
    ESP_LOGE(this->NAME, "Failed to set state: %s", this->robotStateSend.c_str());
  }
}

void Monitor::displaySystemLine()
{
  uint32_t startTime = millis();
  xQueueReceive(this->robotStateQueue, &this->robotStateRecv, pdMS_TO_TICKS(1000));
  int supplyLevel = analogRead(GPIO_NUM_34);
  String supplyLevelStr(map(supplyLevel, 0, 4096, 0, 100));
  supplyLevelStr = "B:" + supplyLevelStr + "%";
  uint16_t controllerStateStrLen = this->monitor->getStrWidth(this->robotStateRecv.c_str());
  uint16_t supplyLevelStrLen = this->monitor->getStrWidth(supplyLevelStr.c_str());
  uint16_t numOfSpace = (uint16_t)((this->displayWidth - controllerStateStrLen - supplyLevelStrLen) / this->monitor->getStrWidth(" "));
  String content = this->robotStateRecv;
  if (numOfSpace > 0)
  {
    for (uint16_t i = 0; i < numOfSpace - 1; i++)
    {
      content += " ";
    }
  }
  content += supplyLevelStr;

  this->display(0, "%s", content.c_str());
}