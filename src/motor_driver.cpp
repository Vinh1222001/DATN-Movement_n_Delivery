#include "motor_driver.hpp"

int step = 0;

MotorDriver::MotorDriver()
    : BaseModule(
          "MOTOR_DRIVER",
          MOTOR_DRIVER_TASK_PRIORITY,
          MOTOR_DRIVER_TASK_DELAY,
          MOTOR_DRIVER_TASK_STACK_DEPTH_LEVEL,
          MOTOR_DRIVER_TASK_PINNED_CORE_ID)
{
  // Define PINS
  pinMode(MOTOR_DRIVER_PIN_IN4, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN3, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN2, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN1, OUTPUT);

  this->currentState.value = MOTOR_DRIVER_MOVE_STOP_STATE_IDX;
  this->currentState.xMutex = xSemaphoreCreateMutex();

  this->speed.value = MOTOR_DRIVER_INIT_SPEED;
  this->speed.xMutex = xSemaphoreCreateMutex();
}

MotorDriver::~MotorDriver() {}

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
    analogWrite(MOTOR_DRIVER_PIN_ENA, this->speed.value);
    analogWrite(MOTOR_DRIVER_PIN_ENB, this->speed.value);
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

void MotorDriver::setSpeed(const int value)
{
  if (xSemaphoreTake(this->speed.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->speed.value = value;
    xSemaphoreGive(this->speed.xMutex);
  }
}

void MotorDriver::moveFoward()
{
  ESP_LOGI("MOTOR_DRIVER", "Move forward");
  this->writeState(MOTOR_DRIVER_MOVE_FORWARD_STATE_IDX);
}

void MotorDriver::moveBackward()
{
  ESP_LOGI("MOTOR_DRIVER", "Move backward");
  this->writeState(MOTOR_DRIVER_MOVE_BACKWARD_STATE_IDX);
}

void MotorDriver::moveLeft()
{
  ESP_LOGI("MOTOR_DRIVER", "Move left");
  this->writeState(MOTOR_DRIVER_MOVE_LEFT_STATE_IDX);
}

void MotorDriver::moveRight()
{
  ESP_LOGI("MOTOR_DRIVER", "Move right");
  this->writeState(MOTOR_DRIVER_MOVE_RIGHT_STATE_IDX);
}

void MotorDriver::stop()
{
  ESP_LOGI("MOTOR_DRIVER", "Stopped");
  this->writeState(MOTOR_DRIVER_MOVE_STOP_STATE_IDX);
}
