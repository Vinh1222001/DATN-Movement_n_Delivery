#include "motor_driver.hpp"

MotorDriver::MotorDriver()
    : BaseModule(
          "MOTOR_DRIVER",
          MOTOR_DRIVER_TASK_PRIORITY,
          MOTOR_DRIVER_TASK_DELAY,
          MOTOR_DRIVER_TASK_STACK_DEPTH_LEVEL,
          MOTOR_DRIVER_TASK_PINNED_CORE_ID)
{
  pinMode(MOTOR_DRIVER_PIN_IN4, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN3, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN2, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN1, OUTPUT);

  this->currentState.value = MOTOR_DRIVER_MOVE_STOP_STATE_IDX;
  this->currentState.xMutex = xSemaphoreCreateMutex();

  this->speed.value.left = MOTOR_DRIVER_INIT_SPEED;
  this->speed.value.right = MOTOR_DRIVER_INIT_SPEED;
  this->speed.xMutex = xSemaphoreCreateMutex();
}

MotorDriver::~MotorDriver()
{
  vSemaphoreDelete(this->currentState.xMutex);
  vSemaphoreDelete(this->speed.xMutex);
  this->currentState.xMutex = nullptr;
  this->speed.xMutex = nullptr;
  this->currentState.value = 0;
}

void MotorDriver::taskFn()
{
  if (xSemaphoreTake(this->currentState.xMutex, portMAX_DELAY) == pdTRUE)
  {
    digitalWrite(MOTOR_DRIVER_PIN_IN1, this->state[this->currentState.value][0]);
    digitalWrite(MOTOR_DRIVER_PIN_IN2, this->state[this->currentState.value][1]);
    digitalWrite(MOTOR_DRIVER_PIN_IN3, this->state[this->currentState.value][2]);
    digitalWrite(MOTOR_DRIVER_PIN_IN4, this->state[this->currentState.value][3]);
    ++this->currentState.value;
    if (this->currentState.value > 4)
    {
      this->currentState.value = 0;
    }
    xSemaphoreGive(this->currentState.xMutex);
  }

  if (xSemaphoreTake(this->speed.xMutex, portMAX_DELAY) == pdTRUE)
  {
    analogWrite(MOTOR_DRIVER_PIN_ENA, this->speed.value.left);
    analogWrite(MOTOR_DRIVER_PIN_ENB, this->speed.value.right);
    xSemaphoreGive(this->speed.xMutex);
  }
}

void MotorDriver::writeState(const uint8_t val)
{
  if (xSemaphoreTake(this->currentState.xMutex, portMAX_DELAY) == pdTRUE)
  {
    if (val != this->currentState.value)
    {
      this->currentState.value = val;
    }
    xSemaphoreGive(this->currentState.xMutex);
  }
}

void MotorDriver::writeSpeed(const int left, const int right)
{
  if (xSemaphoreTake(this->speed.xMutex, portMAX_DELAY) == pdTRUE)
  {
    analogWrite(MOTOR_DRIVER_PIN_ENA, this->speed.value.left);
    analogWrite(MOTOR_DRIVER_PIN_ENB, this->speed.value.right);
    xSemaphoreGive(this->speed.xMutex);
  }
}

void MotorDriver::setSpeed(const int value)
{
  if (xSemaphoreTake(this->speed.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->speed.value.left = value;
    this->speed.value.right = value;
    xSemaphoreGive(this->speed.xMutex);
  }
}

void MotorDriver::moveForward()
{
  ESP_LOGI(this->NAME, "Move forward");
  this->writeState(MOTOR_DRIVER_MOVE_FORWARD_STATE_IDX);
  this->setSpeed(80);
}

void MotorDriver::moveBackward()
{
  ESP_LOGI(this->NAME, "Move backward");
  this->writeState(MOTOR_DRIVER_MOVE_BACKWARD_STATE_IDX);
  this->setSpeed(80);
}

void MotorDriver::moveLeft()
{
  ESP_LOGI(this->NAME, "Move left");
  this->writeState(MOTOR_DRIVER_MOVE_LEFT_STATE_IDX);
  this->setSpeed(80);
}

void MotorDriver::moveRight()
{
  ESP_LOGI(this->NAME, "Move right");
  this->writeState(MOTOR_DRIVER_MOVE_RIGHT_STATE_IDX);
  this->setSpeed(80);
}

void MotorDriver::stop()
{
  ESP_LOGI(this->NAME, "Stopped");
  this->writeState(MOTOR_DRIVER_MOVE_STOP_STATE_IDX);
  this->setSpeed(MOTOR_DRIVER_INIT_SPEED);
}
