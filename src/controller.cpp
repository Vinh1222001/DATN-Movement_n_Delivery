#include "controller.hpp"

Controller::Controller()
    : BaseModule(
          "MAIN_CONTROLLER",
          CONTROLLER_TASK_PRIORITY,
          CONTROLLER_TASK_DELAY,
          CONTROLLER_TASK_STACK_DEPTH_LEVEL,
          CONTROLLER_TASK_PINNED_CORE_ID)
{
  this->state = INIT;
}

Controller::~Controller() {}

void Controller::stateMachine()
{
  switch (this->state)
  {
  case INIT:

    ESP_LOGI(this->NAME, "Initializing components...");
    if (!this->init())
    {
      break;
    }
    delay(3000);
    ESP_LOGI(this->NAME, "Components initialized.");
    this->state = START;

    break;
  case PREPARE:

    ESP_LOGI(this->NAME, "Creating component's tasks...");
    if (!this->prepareTasks())
    {
      ESP_LOGE(this->NAME, "Failed to create tasks");
      break;
    }
    ESP_LOGI(this->NAME, "Component's tasks created.");
    this->state = START;
    delay(3000);

    break;
  case START:

    ESP_LOGI(this->NAME, "Starting component's tasks...");
    if (!this->start())
    {
      ESP_LOGE(this->NAME, "Failed to start tasks");
      break;
    }
    this->state = PICKUP_TRANSIT;

    break;
  case PICKUP_TRANSIT:
    if (this->pickupTransit())
    {
      this->state = PICKUP;
    }
    break;
  case DROPOFF_TRANSIT:

    if (this->dropoffTransit())
    {
      this->state = DROPOFF;
    }
    break;
  case PICKUP:
    if (this->pickup())
    {
      this->state = CLASSIFY;
    }
    break;

  case DROPOFF:
    if (this->dropoff())
    {
      this->state = PICKUP_TRANSIT;
    }
    break;
  case CLASSIFY:
    if (this->classify())
    {
      this->state = DROPOFF_TRANSIT;
    }
    break;
  }
}

void Controller::taskFn()
{
  this->stateMachine();
}

void Controller::setState(RobotState state)
{
  if (state != this->state)
  {
    this->state = state;
  }
}

RobotState Controller::getState()
{
  ESP_LOGI(this->NAME, "Current state: %d", this->state);
  return this->state;
}

void Controller::runComponent(BaseModule *component)
{
  if (component == nullptr)
  {
    ESP_LOGI(this->NAME, "%s is NULL", component->getName());
    return;
  }
  component->run();
}

bool Controller::init()
{
  this->monitor = new Monitor();
  this->colorDetector = new ColorDetector(this->monitor);
  this->motorDriver = new MotorDriver();
  this->lineFollower = new LineFollower(this->motorDriver);

  if (this->monitor == nullptr ||
      this->colorDetector == nullptr ||
      this->motorDriver == nullptr ||
      this->lineFollower == nullptr)
  {
    ESP_LOGE(this->NAME, "One or more components are NULL");
    return false;
  }

  return true;
}

bool Controller::prepareTasks()
{
  this->monitor->createTask();
  delay(5000);
  this->colorDetector->createTask();
  delay(5000);
  this->lineFollower->createTask();
  delay(5000);
  this->motorDriver->createTask();
  delay(5000);

  return true;
}

bool Controller::start()
{
  ESP_LOGI(this->NAME, "Running component's tasks...");

  this->runComponent(this->monitor);
  this->runComponent(this->colorDetector);
  this->runComponent(this->lineFollower);
  this->runComponent(this->motorDriver);

  return true;
}

bool Controller::pickup()
{
  ESP_LOGI(this->NAME, "Pickup");
  if (this->colorDetector == nullptr &&
      this->colorDetector->getColor().color == NONE)
    return false;

  return this->colorDetector->getColor().color == this->nextArea;
}

bool Controller::dropoff()
{
  ESP_LOGI(this->NAME, "Dropoff");
  return true;
}

bool Controller::classify()
{
  ESP_LOGI(this->NAME, "Classify");
  return true;
}

bool Controller::idle()
{
  ESP_LOGI(this->NAME, "Idle");
  return true;
}

bool Controller::pickupTransit()
{
  ESP_LOGI(this->NAME, "Pickup Transit");
  return true;
}

bool Controller::dropoffTransit()
{
  ESP_LOGI(this->NAME, "Dropoff Transit");
  if (this->colorDetector == nullptr)
  {
    return false;
  }
  return this->colorDetector->getColor().color != NONE;
}

bool Controller::setNextArea(ColorSet area)
{
  this->nextArea = area;
  return true;
}