#include "base_module.hpp"

BaseModule::BaseModule(
    const char *name,
    int priority,
    int taskDelay,
    int stackDepthLevel,
    int cpuCore) : NAME(name), priority(priority), TASK_DELAY(taskDelay), stackDepthLevel(stackDepthLevel), cpuCore(cpuCore)
{
  this->taskHandler = NULL; // Khởi tạo taskHandler tránh NULL reference
}

void BaseModule::errorPending()
{
  ESP_LOGE(this->NAME, "Task got an error. Please fix it");
  // if (this->taskHandler != NULL)
  // {
  //   ESP_LOGE(this->NAME, "Deleting Task");
  //   vTaskDelete(this->taskHandler);
  //   this->taskHandler = NULL;
  // }
  while (true)
  {
    ESP_LOGI(this->NAME, "The system will restart after 30s");
    delay(30000);
    // ESP.restart();
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
    ESP_LOGE("BaseModule", "taskWrapper received a null instance\n");
    vTaskDelete(nullptr);
    return;
  }

  const uint32_t taskDelay = instance->TASK_DELAY;

  while (true)
  {
    instance->taskFn();
    vTaskDelay(pdMS_TO_TICKS(taskDelay)); // Dùng FreeRTOS delay
  }
}

void BaseModule::run()
{
  ESP_LOGI(this->NAME, "Running task...");
  if (this->taskHandler == NULL)
  {
    ESP_LOGE(this->NAME, "Task can't be run. Task Handler is Null");
    this->errorPending();
  }

  if (this->isTaskSuspended() || this->isTaskBlocked())
  {
    ESP_LOGI(this->NAME, "Resuming task...");
    vTaskResume(this->taskHandler);
  }
  else if (this->isTaskRunning())
  {
    ESP_LOGE(this->NAME, "Task is already running");
  }
  else
  {
    ESP_LOGI(this->NAME, "Task State: %d", eTaskGetState(this->taskHandler));
    delay(5000);
    ESP_LOGE(this->NAME, "Task is not in suspended state");
    this->errorPending();
  }
}

void BaseModule::createTask()
{
  if (this->taskHandler == NULL)
  {
    ESP_LOGI(this->NAME, "Stack Depth: %d, Priority: %d, CPU Core: %d", this->stackDepthLevel, this->priority, this->cpuCore);

    const int stackDeep = this->stackDepthLevel * CONFIG_ESP32_CORE_DUMP_STACK_SIZE;
    const int priority = this->priority;
    const int cpuCore = this->cpuCore;

    if (xTaskCreatePinnedToCore(taskWrapper, this->NAME, stackDeep, this, priority, &(this->taskHandler), cpuCore) == pdPASS)
    {
      ESP_LOGI(this->NAME, "Task created SUCCESSFULLY\n");
      while (this->taskHandler != NULL && !this->isTaskSuspended())
      {
        ESP_LOGI(this->NAME, "Suspending task\n");
        vTaskSuspend(this->taskHandler);
        delay(1000);
      }
      ESP_LOGI(this->NAME, "Task is suspendded");
    }
    else
    {
      ESP_LOGE(this->NAME, "Task creation FAILED\n");
      this->errorPending();
    }
  }
  else
  {
    ESP_LOGE(this->NAME, "Task has already been created!");
  }
}

TaskHandle_t BaseModule::getTaskHandler()
{
  return this->taskHandler;
}

bool BaseModule::isTaskBlocked()
{
  return (this->taskHandler != NULL) && (eTaskGetState(this->taskHandler) == eBlocked);
}

bool BaseModule::isTaskRunning()
{
  return (this->taskHandler != NULL) && (eTaskGetState(this->taskHandler) == eRunning);
}

bool BaseModule::isTaskReady()
{
  return (this->taskHandler != NULL) && (eTaskGetState(this->taskHandler) == eReady);
}

bool BaseModule::isTaskSuspended()
{
  return (this->taskHandler != NULL) && (eTaskGetState(this->taskHandler) == eSuspended);
}

bool BaseModule::isTaskDeleted()
{
  return this->taskHandler == NULL; // Không thể kiểm tra bằng eTaskGetState()
}
