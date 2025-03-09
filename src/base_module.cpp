#include "base_module.hpp"

BaseModule::BaseModule(
    const char *name,
    int priority,
    int taskDelay,
    int stackDepthLevel,
    int cpuCore) : NAME(name), priority(priority), TASK_DELAY(taskDelay), stackDepthLevel(stackDepthLevel), cpuCore(cpuCore)
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

  const uint32_t taskDelay = instance->TASK_DELAY;

  while (true)
  {
    instance->taskFn();
    delay(taskDelay); // Delay theo giá trị của TASK_DELAY
  }
}

void BaseModule::run()
{
  ESP_LOGI(this->NAME, "Stack deep level: %d, Prioriry: %d, CPU core: %d", this->stackDepthLevel, this->priority, this->cpuCore);

  const int stackDeep = this->stackDepthLevel * CONFIG_ESP32_CORE_DUMP_STACK_SIZE;
  const int priority = this->priority;
  const int cpuCore = this->cpuCore;

  if (xTaskCreatePinnedToCore(taskWrapper, this->NAME, stackDeep, this, priority, nullptr, cpuCore) == pdPASS)
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