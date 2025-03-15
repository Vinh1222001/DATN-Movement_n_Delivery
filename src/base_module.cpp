#include "base_module.hpp"

BaseModule::BaseModule(
    const char *name,
    int priority,
    int taskDelay,
    int stackDepthLevel,
    int cpuCore) : NAME(name), priority(priority), TASK_DELAY(taskDelay), stackDepthLevel(stackDepthLevel), cpuCore(cpuCore)
{
}

void BaseModule::errorPending()
{
  ESP_LOGE(this->NAME, "Task got an error. Please fix it");
  if (this->getTaskHandler() != NULL)
  {
    vTaskDelete(this->taskHandler);
  }
  while (true)
  {
    ESP_LOGI(this->NAME, "The system will be retart after 30s");
    delay(30000);
    ESP.restart();
  }
}

BaseModule::~BaseModule() {}

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
  ESP_LOGI(this->NAME, "Running task...");
  if (this->getTaskHandler() == NULL)
  {
    ESP_LOGE(this->NAME, "Task can't be run. Task Handler is Null");
    this->errorPending();
  }

  if (this->isTaskSuspended())
  {
    vTaskResume(this->getTaskHandler());
  }
  else if (this->isTaskRunning())
  {
    ESP_LOGE(this->NAME, "Task was be running");
  }
  else
  {
    ESP_LOGE(this->NAME, "Task was not be suspended");
    this->errorPending();
  }
}

void BaseModule::createTask()
{
  if (this->getTaskHandler() == NULL)
  {
    ESP_LOGI(this->NAME, "Stack deep level: %d, Prioriry: %d, CPU core: %d", this->stackDepthLevel, this->priority, this->cpuCore);

    const int stackDeep = this->stackDepthLevel * CONFIG_ESP32_CORE_DUMP_STACK_SIZE;
    const int priority = this->priority;
    const int cpuCore = this->cpuCore;

    if (xTaskCreatePinnedToCore(taskWrapper, this->NAME, stackDeep, this, priority, &(this->taskHandler), cpuCore) == pdPASS)
    {
      ESP_LOGI(this->NAME, "created task SUCCESSFULLY\n");
      vTaskSuspend(this->taskHandler);
    }
    else
    {
      ESP_LOGE(this->NAME, "created task FAILED\n");
      this->errorPending();
    }
  }
  else
  {
    ESP_LOGE(this->NAME, "Task had already created!");
  }
}

TaskHandle_t BaseModule::getTaskHandler()
{
  return this->taskHandler;
}

bool BaseModule::isTaskBlocked()
{
  return eTaskGetState(this->taskHandler) == eBlocked;
}

bool BaseModule::isTaskRunning()
{
  return eTaskGetState(this->taskHandler) == eRunning;
}

bool BaseModule::isTaskReady()
{
  return eTaskGetState(this->taskHandler) == eReady;
}

bool BaseModule::isTaskSuspended()
{
  return eTaskGetState(this->taskHandler) == eSuspended;
}

bool BaseModule::isTaskDeleted()
{
  return eTaskGetState(this->taskHandler) == eBlocked;
}
