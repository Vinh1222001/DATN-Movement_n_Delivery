#include "controller.hpp"

Controller::Controller()
    : BaseModule(
          "MAIN_CONTROLLER",
          CONTROLLER_TASK_PRIORITY,
          CONTROLLER_TASK_DELAY,
          CONTROLLER_TASK_STACK_DEPTH_LEVEL,
          CONTROLLER_TASK_PINNED_CORE_ID)
{
  this->state = CONTROLLER_STATE_MACHINE_INIT;
}

Controller::~Controller() {}

void Controller::stateMachine()
{
  switch (this->state)
  {
  case CONTROLLER_STATE_MACHINE_INIT:
    ESP_LOGI(this->NAME, "Initializing components...");

    // this->motorDriver = new MotorDriver();
    // this->lineFollower = new LineFollower(this->motorDriver);
    this->colorDetector = new ColorDetector();

    this->state = CONTROLLER_STATE_MACHINE_START;
    break;
  case CONTROLLER_STATE_MACHINE_START:
    ESP_LOGI(this->NAME, "Creating component's tasks...");

    // this->lineFollower->createTask();
    // delay(5000);
    // this->motorDriver->createTask();
    // delay(5000);
    this->colorDetector->createTask();
    delay(5000);

    this->state = CONTROLLER_STATE_MACHINE_PICKUP_TRANSIT;

    break;
  case CONTROLLER_STATE_MACHINE_PICKUP_TRANSIT:
    ESP_LOGI(this->NAME, "Running component's tasks...");

    // if (this->lineFollower == nullptr)
    // {
    //   ESP_LOGI(this->NAME, "LINE FOLLOWER is NULL");
    //   break;
    // }
    // this->lineFollower->run();

    // if (this->motorDriver == nullptr)
    // {
    //   ESP_LOGI(this->NAME, "MOTOR DRIVER is NULL");
    //   break;
    // }
    // this->motorDriver->run();

    if (this->colorDetector == nullptr)
    {
      ESP_LOGI(this->NAME, "COLOR DETECTOR is NULL");
      break;
    }
    this->colorDetector->run();

    this->state = 0;
    break;
  case CONTROLLER_STATE_MACHINE_DELIVERY:
    break;
  case CONTROLLER_STATE_MACHINE_PICKUP:
    break;
  case CONTROLLER_STATE_MACHINE_DROP_DOWN:
    break;
  case CONTROLLER_STATE_MACHINE_CLASSIFY:
    break;
  }
}

void Controller::taskFn()
{
  this->stateMachine();
}

void Controller::setState(int state)
{
  if (state != this->state)
  {
    this->state = state;
  }
}