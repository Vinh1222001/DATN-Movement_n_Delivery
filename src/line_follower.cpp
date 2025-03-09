#include "line_follower.hpp"

const char *TAG = "LINE_FOLLOWER";

LineFollower::LineFollower()
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

  motorDriver = new MotorDriver();
}

LineFollower::~LineFollower() {}

void LineFollower::taskFn()
{
  this->line_reader.out1 = digitalRead(LINE_FOLLOWER_PIN_OUT1);
  this->line_reader.out2 = digitalRead(LINE_FOLLOWER_PIN_OUT2);
#if LINE_FOLLOWER_VERSION == 1
  this->line_reader.out3 = digitalRead(LINE_FOLLOWER_PIN_OUT3);
  this->line_reader.out4 = digitalRead(LINE_FOLLOWER_PIN_OUT4);
  this->line_reader.out5 = digitalRead(LINE_FOLLOWER_PIN_OUT5);
#endif
  ESP_LOGI(TAG, "Left: %d, Right: %d", this->line_reader.out1, this->line_reader.out2);
#if LINE_FOLLOWER_VERSION == 1
  ESP_LOGI(TAG, "line 1: %d\nline 2: %d\nline 3: %d\nline 4: %d\nline 5: %d\n\n", this->line_reader.out1, this->line_reader.out2, this->line_reader.out3, this->line_reader.out4, this->line_reader.out5);
#endif

  if (this->line_reader.out1 == 0 && this->line_reader.out2 == 0)
  {
    motorDriver->moveFoward();
  }

  if (this->line_reader.out1 == 1 && this->line_reader.out2 == 0)
  {
    motorDriver->moveRight();
    delay(700);
  }

  if (this->line_reader.out1 == 0 && this->line_reader.out2 == 1)
  {
    motorDriver->moveLeft();
    delay(700);
  }

  if (this->line_reader.out1 == 1 && this->line_reader.out2 == 1)
  {
    motorDriver->stop();
  }
}

LineFollower *lineFollower;