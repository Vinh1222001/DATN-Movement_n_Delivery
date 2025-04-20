#include "line_follower.hpp"

LineFollower::LineFollower(
    MotorDriver *motorDriver,
    RWebSocketClient *webSocketClient)
    : BaseModule(
          "LINE_FOLLOWER",
          LINE_FOLLOWER_TASK_PRIORITY,
          LINE_FOLLOWER_TASK_DELAY,
          LINE_FOLLOWER_TASK_STACK_DEPTH_LEVEL,
          LINE_FOLLOWER_TASK_PINNED_CORE_ID),
      motorDriver(motorDriver),
      webSocketClient(webSocketClient)
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

  this->enable.xMutex = xSemaphoreCreateMutex();
  this->enable.value = false;
}

LineFollower::~LineFollower() {}

void LineFollower::taskFn()
{
  if (this->enable.value)
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
      return;
    }

    ESP_LOGI(this->NAME, "Inputs: [%d, %d, %d, %d, %d]",
             values.out1,
             values.out2,
             values.out3,
             values.out4,
             values.out5);

    LineFollowerDecision decision = this->decide(values);

    if (this->webSocketClient != nullptr)
    {
      LineFollowerData lineFollowerData = {
          .leftMost = values.out1,
          .left = values.out2,
          .center = values.out3,
          .right = values.out4,
          .rightMost = values.out5,
          .decision = this->decide2String(decision)};
      this->webSocketClient->setLineFollowerData(lineFollowerData);
    }

    this->driveMotor(decision);

    if (values.out1 == 1 &&
        values.out2 == 1 &&
        values.out3 == 1 &&
        values.out4 == 1 &&
        values.out5 == 1)
    {
      this->motorDriver->stop();
      return;
    }

    int decision =
        values.out1 * this->factors[0] +
        values.out2 * this->factors[1] +
        values.out3 * this->factors[2] +
        values.out4 * this->factors[3] +
        values.out5 * this->factors[4];

    if (this->motorDriver == nullptr)
    {
      ESP_LOGE(this->NAME, "Can't find MOTOR DRIVER");
      return;
    }

    if (decision == -1 || decision == 2 || decision == 5)
    {
      this->motorDriver->moveForward();
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
}

bool LineFollower::getEnable()
{
  bool res = false;
  if (xSemaphoreTake(this->enable.xMutex, portMAX_DELAY) == pdTRUE)
  {
    res = this->enable.value;
    xSemaphoreGive(this->enable.xMutex);
  }

  return res;
}

void LineFollower::setEnable(bool value)
{
  if (xSemaphoreTake(this->enable.xMutex, portMAX_DELAY) == pdTRUE)
  {
    if (!value)
    {
      this->motorDriver->stop();
    }
    this->enable.value = value;
    xSemaphoreGive(this->enable.xMutex);
  }
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

LineFollowerDecision LineFollower::decide(LineFollowerSensorValues values)
{
  if (values.out1 == 1 &&
      values.out2 == 1 &&
      values.out3 == 1 &&
      values.out4 == 1 &&
      values.out5 == 1)
  {
    return STOP;
  }

  int decision = values.out1 * this->factors[0] +
                 values.out2 * this->factors[1] +
                 values.out3 * this->factors[2] +
                 values.out4 * this->factors[3] +
                 values.out5 * this->factors[4];

  if (decision == -1 || decision == 2 || decision == 5)
  {
    return FORWARD;
  }

  if (decision < 0)
  {
    return LEFT;
  }

  if (decision > 1)
  {
    return RIGHT;
  }

  return STOP;
}

String LineFollower::decide2String(LineFollowerDecision decision)
{
  switch (decision)
  {
  case STOP:
    return String("STOP");
  case LEFT:
    return String("TURN LEFT");
  case RIGHT:
    return String("TURN RIGHT");
  case FORWARD:
    return String("MOVE FORWARD");
  case BACKWARD:
    return String("MOVE BACKWARD");
  default:
    return String("IDK");
  }
}

void LineFollower::driveMotor(LineFollowerDecision decision)
{
  if (this->motorDriver != nullptr)
  {
    switch (decision)
    {
    case LEFT:
      this->motorDriver->moveLeft();
      break;
    case RIGHT:
      this->motorDriver->moveRight();
      break;
    case FORWARD:
      this->motorDriver->moveForward();
      break;
    case BACKWARD:
      this->motorDriver->moveBackward();
      break;
    case STOP:
    default:
      this->motorDriver->stop();
      break;
    }
  }
  else
  {
    ESP_LOGE(this->NAME, "The motorDriver is NULL");
  }
}