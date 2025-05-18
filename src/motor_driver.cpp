#include "motor_driver.hpp"

MotorDriver::MotorDriver(Monitor *monitor)
    : BaseModule(
          "MOTOR_DRIVER",
          MOTOR_DRIVER_TASK_PRIORITY,
          MOTOR_DRIVER_TASK_DELAY,
          MOTOR_DRIVER_TASK_STACK_DEPTH_LEVEL,
          MOTOR_DRIVER_TASK_PINNED_CORE_ID),
      monitor(monitor)
{
  pinMode(MOTOR_DRIVER_PIN_IN4, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN3, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN2, OUTPUT);
  pinMode(MOTOR_DRIVER_PIN_IN1, OUTPUT);

  this->mpuReader = new MPUReader();

  this->currentState = SetUtils::initMutexData<uint8_t>(MOTOR_DRIVER_MOVE_STOP_STATE_IDX);
  this->speed = SetUtils::initMutexData<Speed>({MOTOR_DRIVER_INIT_SPEED, MOTOR_DRIVER_INIT_SPEED});
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
  uint8_t state;
  GetUtils::getMutexData<uint8_t>(
      this->currentState,
      [&](uint8_t v)
      {
        state = constrain(v, 0, 4);
      });

  Speed speed;
  GetUtils::getMutexData<Speed>(
      this->speed,
      [&](Speed v)
      {
        speed.left = constrain(v.left, 0, 255);
        speed.right = constrain(v.right, 0, 255);
      });

  this->commit(state, speed);
}

void MotorDriver::writeState(const uint8_t val)
{
  SetUtils::setMutexData<uint8_t>(this->currentState, val);
}

void MotorDriver::setSpeed(const int value)
{
  SetUtils::setMutexData<Speed>(this->speed, Speed{value, value});
}

void MotorDriver::commit(uint8_t state, Speed speed)
{
  if (speed.left != 0 && speed.right != 0)
  {
    this->computeSpeed();
  }
  Speed commitSpeed = {this->currentSpeed, this->currentSpeed};
  ESP_LOGI(this->NAME, "commitSpeed: %d %d", commitSpeed.left, commitSpeed.right);
  digitalWrite(MOTOR_DRIVER_PIN_IN1, this->state[state][0]);
  digitalWrite(MOTOR_DRIVER_PIN_IN2, this->state[state][1]);
  digitalWrite(MOTOR_DRIVER_PIN_IN3, this->state[state][2]);
  digitalWrite(MOTOR_DRIVER_PIN_IN4, this->state[state][3]);

  analogWrite(MOTOR_DRIVER_PIN_ENA, commitSpeed.left);
  analogWrite(MOTOR_DRIVER_PIN_ENB, commitSpeed.right);
}

void MotorDriver::moveForward()
{
  ESP_LOGI(this->NAME, "Move forward");
  this->writeState(MOTOR_DRIVER_MOVE_FORWARD_STATE_IDX);
  this->setSpeed(90);
}

void MotorDriver::moveBackward()
{
  ESP_LOGI(this->NAME, "Move backward");
  this->writeState(MOTOR_DRIVER_MOVE_BACKWARD_STATE_IDX);
  this->setSpeed(90);
}

void MotorDriver::moveLeft()
{
  ESP_LOGI(this->NAME, "Move left");
  this->writeState(MOTOR_DRIVER_MOVE_LEFT_STATE_IDX);
  this->setSpeed(100);
}

void MotorDriver::moveRight()
{
  ESP_LOGI(this->NAME, "Move right");
  this->writeState(MOTOR_DRIVER_MOVE_RIGHT_STATE_IDX);
  this->setSpeed(100);
}

void MotorDriver::stop()
{
  ESP_LOGI(this->NAME, "Stopped");
  this->writeState(MOTOR_DRIVER_MOVE_STOP_STATE_IDX);
  this->setSpeed(0);
}

void MotorDriver::moveForwardSync(int speed)
{
  this->commit(MOTOR_DRIVER_MOVE_FORWARD_STATE_IDX, Speed{speed, speed});
}

void MotorDriver::moveBackwardSync(int speed)
{
  this->commit(MOTOR_DRIVER_MOVE_BACKWARD_STATE_IDX, Speed{speed, speed});
}

void MotorDriver::moveLeftSync(int speed)
{
  this->commit(MOTOR_DRIVER_MOVE_LEFT_STATE_IDX, Speed{speed, speed});
}

void MotorDriver::moveRightSync(int speed)
{
  this->commit(MOTOR_DRIVER_MOVE_RIGHT_STATE_IDX, Speed{speed, speed});
}

void MotorDriver::stopSync()
{
  if (this->currentSpeed != 0)
  {
    this->currentSpeed = 0;
  }
  this->commit(MOTOR_DRIVER_MOVE_STOP_STATE_IDX, Speed{0, 0});
}

void MotorDriver::computeSpeed()
{
  Vector3D velocity3d = this->mpuReader->getVelocity3D();
  ESP_LOGI(this->NAME, "velocity3d: %.2f %.2f %.2f", velocity3d.x, velocity3d.y, velocity3d.z);
  float velocity = sqrtf((velocity3d.x * velocity3d.x) + (velocity3d.y * velocity3d.y) + (velocity3d.z * velocity3d.z));
  ESP_LOGI(this->NAME, "velocity: %.2f", velocity);
  if (velocity < MOTORE_SPEED_THRESHOLD)
  {
    this->currentSpeed++;
    ESP_LOGI(this->NAME, "computeSpeed.currentSpeed: %d", this->currentSpeed);
  }
  IS_NULL(this->monitor);
  this->monitor->display(2, "Velocity: %.2f", velocity);
}