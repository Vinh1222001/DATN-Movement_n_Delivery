#include "controller.hpp"

Controller::Controller()
    : BaseModule("MAIN_CONTROLLER")
{
  this->state = CONTROLLER_STATE_MACHINE_INIT;
}

Controller::~Controller() {}

void Controller::stateMachine()
{
  switch (this->state)
  {
  case CONTROLLER_STATE_MACHINE_INIT:
    break;
  case CONTROLLER_STATE_MACHINE_START:
    break;
  case CONTROLLER_STATE_MACHINE_PICKUP_TRANSIT:
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