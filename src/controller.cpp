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
    this->motorDriver = new MotorDriver();
    this->lineFollower = new LineFollower(this->motorDriver);
    break;
  case CONTROLLER_STATE_MACHINE_START:

    this->lineFollower->createTask();
    this->motorDriver->createTask();

    this->state = CONTROLLER_STATE_MACHINE_PICKUP_TRANSIT;

    break;
  case CONTROLLER_STATE_MACHINE_PICKUP_TRANSIT:

    this->motorDriver->run();
    this->lineFollower->run();

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