#include "line_follower.hpp"

const char *TAG = "LINE_FOLLOWER";

LineFollower::LineFollower(MotorDriver *motorDriver)
    : BaseModule(
          "LINE_FOLLOWER",
          LINE_FOLLOWER_TASK_PRIORITY,
          LINE_FOLLOWER_TASK_DELAY,
          LINE_FOLLOWER_TASK_STACK_DEPTH_LEVEL,
          LINE_FOLLOWER_TASK_PINNED_CORE_ID)
{
  pinMode(LINE_SENSOR_PIN_LEFT_MOST, INPUT);
  pinMode(LINE_SENSOR_PIN_LEFT, INPUT);
  pinMode(LINE_SENSOR_PIN_CENTER, INPUT);
  pinMode(LINE_SENSOR_PIN_RIGHT, INPUT);
  pinMode(LINE_SENSOR_PIN_RIGHT_MOST, INPUT);

  this->signals.value = {
      .out1 = 0,
      .out2 = 0,
      .out3 = 0,
      .out4 = 0,
      .out5 = 0};

  this->signals.xMutex = xSemaphoreCreateMutex();

  this->isRobotArrived.xMutex = xSemaphoreCreateMutex();
  this->isRobotArrived.value = false;

  this->motorDriver = motorDriver;
}

LineFollower::~LineFollower() {}

void LineFollower::taskFn()
{
  LineFollowerSensorValues values = {0, 0, 0, 0, 0};

  if (xSemaphoreTake(this->signals.xMutex, portMAX_DELAY) == pdTRUE)
  {
    /**
     * @brief Out[number] = 1 => sensor catched the line, otherwise, Out[number] = 0
     */
    this->signals.value.out1 = digitalRead(LINE_SENSOR_PIN_LEFT_MOST);
    this->signals.value.out2 = digitalRead(LINE_SENSOR_PIN_LEFT);
    this->signals.value.out3 = digitalRead(LINE_SENSOR_PIN_CENTER);
    this->signals.value.out4 = digitalRead(LINE_SENSOR_PIN_RIGHT);
    this->signals.value.out5 = digitalRead(LINE_SENSOR_PIN_RIGHT_MOST);

    values.out1 = this->signals.value.out1;
    values.out2 = this->signals.value.out2;
    values.out3 = this->signals.value.out3;
    values.out4 = this->signals.value.out4;
    values.out5 = this->signals.value.out5;

    xSemaphoreGive(this->signals.xMutex);
  }
  else
  {
    ESP_LOGE(this->NAME, "Can't access signals");
  }

  ESP_LOGI(this->NAME, "Inputs: [%d, %d, %d, %d, %d]",
           values.out1,
           values.out2,
           values.out3,
           values.out4,
           values.out5);

  if (values.out1 == 1 &&
      values.out2 == 1 &&
      values.out3 == 1 &&
      values.out4 == 1 &&
      values.out5 == 1)
  {
    this->motorDriver->stop();
    return;
  }

  bool left = (values.out1 + values.out2) > 0;
  bool center = values.out3 > 0;
  bool right = (values.out4 + values.out5) > 0;

  int decision =
      values.out1 * this->factors[0] +
      values.out2 * this->factors[1] +
      values.out3 * this->factors[2] +
      values.out4 * this->factors[3] +
      values.out5 * this->factors[4];

  // ESP_LOGI(TAG, "Signals: [%d, %d, %d]", left, center, right);

  if (this->motorDriver == nullptr)
  {
    ESP_LOGE(this->NAME, "Can't find MOTOR DRIVER");
    return;
  }

  if (decision == -1 || decision == 2 || decision == 5)
  {
    this->motorDriver->moveFoward();
    return;
  }

  if (decision < 0)
  {
    this->motorDriver->moveLeft();
    return;
  }

  if (decision > 1)
  {
    this->motorDriver->moveRight();
    return;
  }

  this->motorDriver->stop();
}

bool LineFollower::isArrived()
{
  bool res = false;
  if (xSemaphoreTake(this->isRobotArrived.xMutex, portMAX_DELAY) == pdTRUE)
  {
    res = this->isRobotArrived.value;
    xSemaphoreGive(this->isRobotArrived.xMutex);
  }

  return res;
}

LineFollowerSensorValues LineFollower::getLineFollowerValues()
{

  LineFollowerSensorValues values = {
      .out1 = -1,
      .out2 = -1,
      .out3 = -1,
      .out4 = -1,
      .out5 = -1};

  if (xSemaphoreTake(this->signals.xMutex, portMAX_DELAY) == pdTRUE)
  {
    ESP_LOGI(this->NAME, "Get signal values");
    values = this->signals.value;
    xSemaphoreGive(this->signals.xMutex);
  }
  else
  {
    ESP_LOGE(this->NAME, "Failed to take mutex for sensor values!");
  }

  return values;
}