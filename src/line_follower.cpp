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
  pinMode(LINE_FOLLOWER_PIN_OUT1, INPUT);
  pinMode(LINE_FOLLOWER_PIN_OUT2, INPUT);

#if LINE_FOLLOWER_VERSION == 1
  pinMode(LINE_FOLLOWER_PIN_OUT3, INPUT);
  pinMode(LINE_FOLLOWER_PIN_OUT4, INPUT);
  pinMode(LINE_FOLLOWER_PIN_OUT5, INPUT);
#endif

  this->line_reader = {
      .out1 = 0,
      .out2 = 0,
#if LINE_FOLLOWER_VERSION == 1
      .out3 = 0,
      .out4 = 0,
      .out5 = 0
#endif
  };

  this->xMutextIsArrived = xSemaphoreCreateMutex();

  this->motorDriver = motorDriver;
}

LineFollower::~LineFollower() {}

void LineFollower::taskFn()
{

  /**
   * @brief Out[number] = 1 => sensor catched the line, otherwise, Out[number] = 0
   */
  this->line_reader.out1 = !digitalRead(LINE_FOLLOWER_PIN_OUT1);
  this->line_reader.out2 = !digitalRead(LINE_FOLLOWER_PIN_OUT2);
#if LINE_FOLLOWER_VERSION == 1
  this->line_reader.out3 = !digitalRead(LINE_FOLLOWER_PIN_OUT3);
  this->line_reader.out4 = !digitalRead(LINE_FOLLOWER_PIN_OUT4);
  this->line_reader.out5 = !digitalRead(LINE_FOLLOWER_PIN_OUT5);
#endif

#if LINE_FOLLOWER_VERSION == 2
  ESP_LOGI(TAG, "Left: %d, Right: %d", this->line_reader.out1, this->line_reader.out2);
#endif

#if LINE_FOLLOWER_VERSION == 1
  ESP_LOGI(TAG, "Inputs: [%d, %d, %d, %d, %d]",
           this->line_reader.out1,
           this->line_reader.out2,
           this->line_reader.out3,
           this->line_reader.out4,
           this->line_reader.out5);
#endif

  bool left = (this->line_reader.out1 * 2 + this->line_reader.out2) > 0;
  bool center = this->line_reader.out3 > 0;
  bool right = (this->line_reader.out4 + this->line_reader.out5 * 2) > 0;

  ESP_LOGI(TAG,
           "Signals: [%d, %d, %d,]",
           left,
           center,
           right);

  if (!left && center && !right)
  {
    this->motorDriver->moveFoward();
  }
  else if (left && !center && !right)
  {
    this->motorDriver->moveLeft();
  }
  else if (!left && !center && right)
  {
    this->motorDriver->moveRight();
  }
  else if (!left && !center && !right)
  {
    this->motorDriver->moveLeft();
  }
  else
  {
    this->motorDriver->stop();
  }
}

bool LineFollower::isArrived()
{
  bool res = false;
  if (xSemaphoreTake(this->xMutextIsArrived, portMAX_DELAY) == pdTRUE)
  {
    res = this->isItArrived;
    xSemaphoreGive(this->xMutextIsArrived);
  }

  return res;
}