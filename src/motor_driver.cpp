#include "motor_driver.hpp"

MotorDriver::MotorDriver()
    : BaseModule(
          "MOTOR_DRIVER",
          MOTOR_DRIVER_TASK_PRIORITY,
          MOTOR_DRIVER_TASK_DELAY,
          MOTOR_DRIVER_TASK_STACK_DEPTH_LEVEL,
          MOTOR_DRIVER_TASK_PINNED_CORE_ID)
{
  // Define PINS
  pinMode(MOTOR_DRIVER_PIN_IN1, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN2, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN3, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN4, OUTPUT);

  pinMode(MOTOR_DRIVER_PIN_ENA, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_ENB, OUTPUT);

  analogWrite(MOTOR_DRIVER_PIN_ENA, this->maxSpeed);
  analogWrite(MOTOR_DRIVER_PIN_ENB, this->maxSpeed);

  this->xBinaryState = xSemaphoreCreateBinary();
  this->currentState = MOTOR_DRIVER_MOVE_STOP_STATE_IDX;
}

MotorDriver::~MotorDriver() {}

void MotorDriver::taskFn()
{
  if (this->xBinaryState != NULL)
  {
    if (xSemaphoreTake(this->xBinaryState, portMAX_DELAY) == pdTRUE)
    {
      digitalWrite(MOTOR_DRIVER_PIN_IN1, this->state[this->currentState][0]);
      digitalWrite(MOTOR_DRIVER_PIN_IN2, this->state[this->currentState][1]);
      digitalWrite(MOTOR_DRIVER_PIN_IN3, this->state[this->currentState][2]);
      digitalWrite(MOTOR_DRIVER_PIN_IN4, this->state[this->currentState][3]);
    }
  }
  else
  {
    ESP_LOGE(this->NAME, "xBinaryState is NULL");
  }
}

void MotorDriver::writeState(const uint8_t val)
{
  if (this->xBinaryState != NULL)
  {
    if (val != this->currentState)
    {
      this->currentState = val;
      xSemaphoreGive(this->xBinaryState);
    }
  }
  else
  {
    ESP_LOGE(this->NAME, "xBinaryState is NULL");
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
