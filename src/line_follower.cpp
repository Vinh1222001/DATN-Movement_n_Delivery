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

#if LINE_FOLLOWER_VERSION == 1
  pinMode(LINE_SENSOR_PIN_CENTER, INPUT);
  pinMode(LINE_SENSOR_PIN_RIGHT, INPUT);
  pinMode(LINE_SENSOR_PIN_RIGHT_MOST, INPUT);
#endif

  this->values = {
      .out1 = 0,
      .out2 = 0,
#if LINE_FOLLOWER_VERSION == 1
      .out3 = 0,
      .out4 = 0,
      .out5 = 0
#endif
  };

  this->isRobotArrived.xMutex = xSemaphoreCreateMutex();
  this->isRobotArrived.value = false;

  this->motorDriver = motorDriver;
}

LineFollower::~LineFollower() {}

void LineFollower::taskFn()
{
  /**
   * @brief Out[number] = 1 => sensor catched the line, otherwise, Out[number] = 0
   */
  this->values.out1 = digitalRead(LINE_SENSOR_PIN_LEFT_MOST);
  this->values.out2 = digitalRead(LINE_SENSOR_PIN_LEFT);
#if LINE_FOLLOWER_VERSION == 1
  this->values.out3 = digitalRead(LINE_SENSOR_PIN_CENTER);
  this->values.out4 = digitalRead(LINE_SENSOR_PIN_RIGHT);
  this->values.out5 = digitalRead(LINE_SENSOR_PIN_RIGHT_MOST);
#endif

#if LINE_FOLLOWER_VERSION == 2
  ESP_LOGI(TAG, "Left: %d, Right: %d", this->values.out1, this->values.out2);
#endif

#if LINE_FOLLOWER_VERSION == 1
  // ESP_LOGI(TAG, "Inputs: [%d, %d, %d, %d, %d]",
  //          this->values.out1,
  //          this->values.out2,
  //          this->values.out3,
  //          this->values.out4,
  //          this->values.out5);
#endif
  if (this->values.out1 == 1 &&
      this->values.out2 == 1 &&
      this->values.out3 == 1 &&
      this->values.out4 == 1 &&
      this->values.out5 == 1)
  {
    this->motorDriver->stop();
    this->motorDriver->setSpeed(0);
    return;
  }
  bool left = (this->values.out1 + this->values.out2) > 0;
  bool center = this->values.out3 > 0;
  bool right = (this->values.out4 + this->values.out5) > 0;

  int decision = this->values.out1 * this->factors[0] + this->values.out1 * this->factors[1] + this->values.out3 * this->factors[2] + this->values.out4 * this->factors[3] + this->values.out5 * this->factors[4];

  // ESP_LOGI(TAG, "Signals: [%d, %d, %d]", left, center, right);

  if (decision == -1 || decision == 2 || decision == 5)
  {
    this->motorDriver->moveFoward();
    this->motorDriver->setSpeed(120);
    return;
  }

  if (decision < 0)
  {
    this->motorDriver->moveLeft();
    this->motorDriver->setSpeed(120);
    return;
  }

  if (decision > 1)
  {
    this->motorDriver->moveRight();
    this->motorDriver->setSpeed(120);
    return;
  }

  this->motorDriver->stop();
  this->motorDriver->setSpeed(0);
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