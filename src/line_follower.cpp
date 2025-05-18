#include "line_follower.hpp"

LineFollower::LineFollower(
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

  this->signals = SetUtils::initMutexData<LineFollowerSensorValues>({0, 0, 0, 0, 0});

  this->enable = SetUtils::initMutexData<bool>(false);
  this->motorDriver = new MotorDriver();
}

LineFollower::~LineFollower() {}

void LineFollower::taskFn()
{
  IS_NULL(this->motorDriver);
  ESP_LOGI(this->NAME, "Line Follower is working....");
  bool enable = true;

  GetUtils::getMutexData<bool>(
      this->enable,
      [&](bool value)
      {
        enable = value;
      });

  if (!enable)
  {
    this->driveMotor(STOP);
    return;
  }

  LineFollowerSensorValues values = {0, 0, 0, 0, 0};

  /**
   * @brief Out[number] = 1 => sensor catched the line, otherwise, Out[number] = 0
   */
  values.out1 = digitalRead(LINE_SENSOR_PIN_LEFT_MOST);
  values.out2 = digitalRead(LINE_SENSOR_PIN_LEFT);
  values.out3 = digitalRead(LINE_SENSOR_PIN_CENTER);
  values.out4 = digitalRead(LINE_SENSOR_PIN_RIGHT);
  values.out5 = digitalRead(LINE_SENSOR_PIN_RIGHT_MOST);

  SetUtils::setMutexData<LineFollowerSensorValues>(this->signals, values);

  ESP_LOGI(this->NAME, "Inputs: [%d, %d, %d, %d, %d]",
           values.out1,
           values.out2,
           values.out3,
           values.out4,
           values.out5);

  LineFollowerDecision decision = this->decide(values);

  this->driveMotor(decision);

  IS_NULL(this->webSocketClient);
  LineFollowerData lineFollowerData = {
      .leftMost = values.out1,
      .left = values.out2,
      .center = values.out3,
      .right = values.out4,
      .rightMost = values.out5,
      .decision = this->decide2String(decision)};
  this->webSocketClient->setLineFollowerData(lineFollowerData);
}

bool LineFollower::getEnable()
{
  bool res = false;
  GetUtils::getMutexData<bool>(
      this->enable,
      [&](bool value)
      { res = value; });

  return res;
}

void LineFollower::setEnable(bool value)
{
  SetUtils::setMutexData<bool>(this->enable, value);
}

LineFollowerSensorValues LineFollower::getLineFollowerValues()
{

  LineFollowerSensorValues values = {
      .out1 = -1,
      .out2 = -1,
      .out3 = -1,
      .out4 = -1,
      .out5 = -1};

  GetUtils::getMutexData<LineFollowerSensorValues>(
      this->signals,
      [&](LineFollowerSensorValues v)
      {
        values = v;
      });

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
  IS_NULL(this->motorDriver);

  switch (decision)
  {
  case LEFT:
    this->motorDriver->moveLeftSync(80);
    break;
  case RIGHT:
    this->motorDriver->moveRightSync(80);
    break;
  case FORWARD:
    this->motorDriver->moveForwardSync(80);
    break;
  case BACKWARD:
    this->motorDriver->moveBackwardSync(70);
    break;
  case STOP:
  default:
    this->motorDriver->stopSync();
    break;
  }
}