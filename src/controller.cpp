#include "controller.hpp"

Controller::Controller()
    : BaseModule(
          "MAIN_CONTROLLER",
          CONTROLLER_TASK_PRIORITY,
          CONTROLLER_TASK_DELAY,
          CONTROLLER_TASK_STACK_DEPTH_LEVEL,
          CONTROLLER_TASK_PINNED_CORE_ID),
      isClassifying(false)
{
  this->state = INIT;
}

Controller::~Controller() {}

void Controller::stateMachine()
{
  switch (this->state)
  {
  case INIT:
    this->init();
    delay(2000);
    break;

  case SETUP:
    this->setup();
    delay(2000);
    break;

  case READY:
    this->ready();
    delay(2000);
    break;

  case PING:
    this->ping();
    delay(2000);
    break;

  case START:
    !this->start();
    delay(2000);
    break;

  case PICKUP_TRANSIT:
    this->pickupTransit();
    break;

  case DROPOFF_TRANSIT:

    this->dropoffTransit();
    break;

  case PICKUP:
    this->pickup();
    break;

  case DROPOFF:
    this->dropoff();
    break;

  case CLASSIFY:
    this->classify();
    break;

  case IDLE:
    this->idle();
    break;
  }
}

void Controller::taskFn()
{
  this->stateMachine();
}

void Controller::setState(RobotState state, bool extCondition)
{
  if (extCondition)
  {
    if (state != this->state)
    {
      this->state = state;
    }
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
  ESP_LOGI(this->NAME, "Initialize components...");

  this->communicate = new ClassifyingCommunicate();
  if (this->communicate == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Communicate");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Communicate success");

  this->webSocketClient = new RWebSocketClient();
  if (this->webSocketClient == nullptr || !this->webSocketClient->isConnected())
  {
    ESP_LOGE(this->NAME, "Failed to init Web Socket Client");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Web Socket Client success");

  this->monitor = new Monitor();
  if (this->monitor == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Monitor");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Monitor success");

  this->colorDetector = new ColorDetector(this->monitor, this->webSocketClient);
  if (this->colorDetector == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Color Detector");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Color Detector success");

  this->motorDriver = new MotorDriver();
  if (this->motorDriver == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Motor Driver");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Motor Driver success");

  this->lineFollower = new LineFollower(this->motorDriver, this->webSocketClient);
  if (this->lineFollower == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Line Follower");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Line Follower success");

  ESP_LOGI(this->NAME, "All components have initalized!");
  this->setState(SETUP);
  return true;
}

bool Controller::setup()
{
  ESP_LOGI(this->NAME, "Creating component's tasks...");

  if (!this->communicate)
  {
    ESP_LOGE(this->NAME, "Communicate is not initialized");
    return false;
  }
  delay(2000);
  ESP_LOGI(this->NAME, "Connecting to ESP32 CAM");

  if (!this->webSocketClient)
  {
    ESP_LOGE(this->NAME, "WebSocketClient is not initialized");
    return false;
  }
  ESP_LOGI(this->NAME, "Creating WebSocketClient's task...");
  this->webSocketClient->createTask();
  delay(2000);
  ESP_LOGI(this->NAME, "Created WebSocketClient's task successfully");

  if (!this->monitor)
  {
    ESP_LOGE(this->NAME, "Monitor is not initialized");
    return false;
  }
  ESP_LOGI(this->NAME, "Creating Monitor's task...");
  this->monitor->createTask();
  delay(2000);
  ESP_LOGI(this->NAME, "Created Monitor's task successfully");

  if (!this->colorDetector)
  {
    ESP_LOGE(this->NAME, "ColorDetector is not initialized");
    return false;
  }
  ESP_LOGI(this->NAME, "Creating ColorDetector's task...");
  this->colorDetector->createTask();
  delay(2000);
  ESP_LOGI(this->NAME, "Created ColorDetector's task successfully");

  if (!this->lineFollower)
  {
    ESP_LOGE(this->NAME, "LineFollower is not initialized");
    return false;
  }
  ESP_LOGI(this->NAME, "Creating LineFollower's task...");
  this->lineFollower->createTask();
  delay(2000);
  ESP_LOGI(this->NAME, "Created LineFollower's task successfully");

  if (!this->motorDriver)
  {
    ESP_LOGE(this->NAME, "MotorDriver is not initialized");
    return false;
  }
  ESP_LOGI(this->NAME, "Creating MotorDriver's task...");
  this->motorDriver->createTask();
  delay(2000);
  ESP_LOGI(this->NAME, "Created MotorDriver's task successfully");

  ESP_LOGI(this->NAME, "All component tasks have been created successfully!");
  this->state = READY;

  return true;
}

bool Controller::ready()
{
  ESP_LOGI(this->NAME, "Run important components...");
  if (!this->webSocketClient)
  {
    ESP_LOGE(this->NAME, "webSocketClient is not initialized");
    return false;
  }
  this->runComponent(this->webSocketClient);
  ESP_LOGI(this->NAME, "Run webSocketClient's task successfully");

  if (!this->monitor)
  {
    ESP_LOGE(this->NAME, "Monitor is not initialized");
    return false;
  }
  this->runComponent(this->monitor);
  ESP_LOGI(this->NAME, "Run Monitor's task successfully");

  ESP_LOGI(this->NAME, "All important components have run!");
  this->setState(PING);

  return true;
}

bool Controller::ping()
{
  std::vector<String> pingMsg;
  pingMsg.push_back("PING");
  this->communicate->send(pingMsg);

  String response = this->communicate->getReceiveMsg();
  if (response.compareTo("OK") == 0)
  {
    ESP_LOGI(this->NAME, "Connection is OK! Received message: %s", response.c_str());
    // this->setState(START);
    this->setState(CLASSIFY);
    delay(1000);
    return true;
  }

  ESP_LOGE(this->NAME, "Connection failed! Received message: %s", response.c_str());
  delay(1000);
  return false;
}

bool Controller::start()
{
  ESP_LOGI(this->NAME, "Running component's tasks...");

  if (!this->colorDetector)
  {
    ESP_LOGE(this->NAME, "colorDetector is not initialized");
    return false;
  }
  this->runComponent(this->colorDetector);
  ESP_LOGI(this->NAME, "Run colorDetector's task successfully");

  if (!this->lineFollower)
  {
    ESP_LOGE(this->NAME, "lineFollower is not initialized");
    return false;
  }
  this->runComponent(this->lineFollower);
  ESP_LOGI(this->NAME, "Run lineFollower's task successfully");

  if (!this->motorDriver)
  {
    ESP_LOGE(this->NAME, "motorDriver is not initialized");
    return false;
  }
  this->runComponent(this->motorDriver);
  ESP_LOGI(this->NAME, "Run motorDriver's task successfully");

  ESP_LOGI(this->NAME, "All components have run!");
  this->setState(PICKUP_TRANSIT);
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
  ESP_LOGI(this->NAME, "Classifing...");
  if (this->communicate == nullptr)
  {
    ESP_LOGE(this->NAME, "Communicate is still null");
    return false;
  }

  if (!this->isClassifying)
  {
    std::vector<String> msg;
    msg.push_back("CLASSIFY");
    this->communicate->send(msg);
    this->isClassifying = true;
    return false;
  }

  String response = this->communicate->getReceiveMsg();
  if (response.compareTo("TRUE") == 0)
  {
    std::vector<String> msg;
    msg.push_back("STOP_CLASSIFY");
    this->communicate->send(msg);
    this->setState(IDLE);

    ESP_LOGI(this->NAME, "Classify is %s", response.c_str());

    delay(2000);
    return true;
  }
  ESP_LOGE(this->NAME, "Failed to classify! Message is: %s", response.c_str());
  return false;
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