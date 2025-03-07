#include "base_module.hpp"

BaseModule::BaseModule(
    const char *name,
    int priority,
    int taskDelay,
    int stackDepthLevel,
    int cpuCore) : NAME(name), priority(&priority), TASK_DELAY(&taskDelay), stackDepthLevel(&stackDepthLevel), cpuCore(&cpuCore)
{
}

BaseModule::~BaseModule()
{
}

void BaseModule::taskFn()
{
  ESP_LOGI(this->NAME, "Running taskFn of Base Module\n");
}

void BaseModule::taskWrapper(void *pvParameter)
{
  BaseModule *instance = static_cast<BaseModule *>(pvParameter);
  if (instance == nullptr)
  {
    ESP_LOGE(instance->NAME, "taskWrapper received a null instance\n");
    vTaskDelete(nullptr);
    return;
  }

  while (true)
  {
    instance->taskFn();
    delay(*instance->TASK_DELAY); // Delay theo giá trị của TASK_DELAY
  }
}

void BaseModule::run()
{
  if (xTaskCreatePinnedToCore(taskWrapper, this->NAME, *this->stackDepthLevel * CONFIG_ESP32_CORE_DUMP_STACK_SIZE, this, *this->priority, nullptr, *this->cpuCore) == pdPASS)
  {
    ESP_LOGI(this->NAME, "created task SUCCESSFULLY\n");
  }
  else
  {
    ESP_LOGE(this->NAME, "created task FAILED\n");
    while (true)
    {
    }
  }
}