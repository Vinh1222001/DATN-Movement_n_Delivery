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
    this->init();
    delay(5000);
    ESP_LOGI(this->NAME, "Components initialized.");
    this->state = START;

    break;
  case PREPARE:

    ESP_LOGI(this->NAME, "Creating component's tasks...");
    this->prepareTasks();

    break;
  case START:

    ESP_LOGI(this->NAME, "Starting component's tasks...");
    this->start();
    this->state = PICKUP_TRANSIT;

    break;
  case PICKUP_TRANSIT:

    if (this->colorDetector == nullptr &&
        this->colorDetector->getColor().color == NONE)
      break;

    if (this->colorDetector->getColor().color == nextArea)
    {
      this->state = PICKUP;
    }

    break;
  case DROPOFF_TRANSIT:

    if (this->colorDetector == nullptr)
    {
      break;
    }

    if (this->colorDetector->getColor() == NONE)
    {
      break;
    }
    this->state = DROPOFF;
    break;
  case PICKUP:
    this->state = CLASSIFY;
    break;

  case DROPOFF:
    this->state = PICKUP_TRANSIT;
    break;
  case CLASSIFY:
    this->state = DROPOFF_TRANSIT;
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

void Controller::init()
{
  this->state = INIT;
  this->monitor = new Monitor();
  this->colorDetector = new ColorDetector(this->monitor);
  this->motorDriver = new MotorDriver();
  this->lineFollower = new LineFollower(this->motorDriver);
}

void Controller::prepareTasks()
{
  this->state = START;
  this->monitor->createTask();
  delay(5000);
  this->colorDetector->createTask();
  delay(5000);
  this->lineFollower->createTask();
  delay(5000);
  this->motorDriver->createTask();
  delay(5000);
}

void Controller::start()
{
  ESP_LOGI(this->NAME, "Running component's tasks...");

  this->runComponent(this->monitor);
  this->runComponent(this->colorDetector);
  this->runComponent(this->lineFollower);
  this->runComponent(this->motorDriver);
}