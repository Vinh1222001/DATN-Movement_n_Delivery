#include "controller.hpp"
#include "utils/stringUtils.hpp"

Controller::Controller()
    : BaseModule(
          "MAIN_CONTROLLER",
          CONTROLLER_TASK_PRIORITY,
          CONTROLLER_TASK_DELAY,
          CONTROLLER_TASK_STACK_DEPTH_LEVEL,
          CONTROLLER_TASK_PINNED_CORE_ID),
      isClassifying(false),
      isMoving(false)
{
  this->state = RobotState::INIT;
}

Controller::~Controller() {}

void Controller::stateMachine()
{
  switch (this->state)
  {
  case RobotState::INIT:
    this->init();
    delay(2000);
    break;

  case RobotState::SETUP:
    IS_NULL(this->communicate);
    IS_NULL(this->monitor);
    // IS_NULL(this->webSocketClient);
    IS_NULL(this->colorDetector);
    IS_NULL(this->lineFollower);
    this->setup();
    delay(2000);
    break;

  case RobotState::READY:
    /**
     * Connect to Web Socket and Display Monitor
     */
    // IS_NULL(this->webSocketClient);
    IS_NULL(this->monitor);
    this->ready();
    delay(2000);
    break;

  case RobotState::PING:
    /**
     * Testing connection to ESP32 CAM
     */
    // IS_NULL(this->communicate);
    IS_NULL(this->monitor);
    this->ping();
    delay(2000);
    break;

  case RobotState::START:
    IS_NULL(this->colorDetector);
    IS_NULL(this->lineFollower);
    IS_NULL(this->monitor);
    this->start();
    delay(2000);
    break;

  case RobotState::PICKUP_TRANSIT:
    IS_NULL(this->lineFollower);
    IS_NULL(this->colorDetector);
    IS_NULL(this->monitor);
    this->pickupTransit();
    break;

  case RobotState::DROPOFF_TRANSIT:
    IS_NULL(this->lineFollower);
    IS_NULL(this->colorDetector);
    IS_NULL(this->monitor);
    this->dropoffTransit();
    break;

  case RobotState::PICKUP:
    IS_NULL(this->monitor);
    this->pickup();
    break;

  case RobotState::DROPOFF:
    IS_NULL(this->monitor);
    this->dropoff();
    break;

  case RobotState::CLASSIFY:
    IS_NULL(this->communicate);
    IS_NULL(this->monitor);
    // IS_NULL(this->webSocketClient);
    this->classify();
    break;

  case RobotState::IDLE:
    IS_NULL(this->colorDetector);
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
  uint8_t mac[6] = {0x08, 0xd1, 0xf9, 0x38, 0xa8, 0xac};
  this->communicate = new Communicate(mac);
  if (this->communicate == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Communicate");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Communicate success");

  this->monitor = new Monitor();
  if (this->monitor == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Monitor");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Monitor success");

  // this->webSocketClient = new RWebSocketClient();
  // if (this->webSocketClient == nullptr || !this->webSocketClient->isConnected())
  // {
  //   ESP_LOGE(this->NAME, "Failed to init Web Socket Client");
  //   return false;
  // }
  // ESP_LOGI(this->NAME, "Init Web Socket Client success");

  this->mpuReader = new MPUReader(this->monitor);

  this->colorDetector = new ColorDetector(this->monitor, this->webSocketClient);
  if (this->colorDetector == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Color Detector");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Color Detector success");

  this->lineFollower = new LineFollower(this->webSocketClient);
  if (this->lineFollower == nullptr)
  {
    ESP_LOGE(this->NAME, "Failed to init Line Follower");
    return false;
  }
  ESP_LOGI(this->NAME, "Init Line Follower success");

  ESP_LOGI(this->NAME, "All components have initalized!");
  this->setState(RobotState::SETUP);
  return true;
}

bool Controller::setup()
{
  ESP_LOGI(this->NAME, "Creating component's tasks...");

  ESP_LOGI(this->NAME, "Creating Monitor's task...");
  this->monitor->createTask();
  delay(1000);

  // ESP_LOGI(this->NAME, "Creating WebSocketClient's task...");
  // this->webSocketClient->createTask();
  // delay(1000);
  // ESP_LOGI(this->NAME, "Created WebSocketClient's task successfully");

  ESP_LOGI(this->NAME, "Creating MPUReader's task...");
  this->mpuReader->createTask();
  delay(1000);

  ESP_LOGI(this->NAME, "Creating ColorDetector's task...");
  this->colorDetector->createTask();
  delay(1000);
  ESP_LOGI(this->NAME, "Created ColorDetector's task successfully");

  ESP_LOGI(this->NAME, "Creating LineFollower's task...");
  this->lineFollower->createTask();
  delay(1000);
  ESP_LOGI(this->NAME, "Created LineFollower's task successfully");

  ESP_LOGI(this->NAME, "All component tasks have been created successfully!");

  this->setState(RobotState::READY);

  return true;
}

bool Controller::ready()
{
  ESP_LOGI(this->NAME, "Run important components...");

  // this->runComponent(this->webSocketClient);
  // ESP_LOGI(this->NAME, "Run webSocketClient's task successfully");

  this->runComponent(this->monitor);
  ESP_LOGI(this->NAME, "Run Monitor's task successfully");

  this->runComponent(this->mpuReader);
  ESP_LOGI(this->NAME, "Run Mpu Reader's task successfully");

  ESP_LOGI(this->NAME, "All important components have run!");
  this->setState(RobotState::PING);

  return true;
}

bool Controller::ping()
{
  this->monitor->setRobotState("PING");

  ESP_LOGI(this->NAME, "Try to connect Classifier");
  std::vector<String> pingMsg;
  pingMsg.push_back("Request to connect...");
  this->communicate->send("PING", pingMsg);

  CommunicateResponse response = this->communicate->getResponse();
  if (response.header.compareTo("RESPONSE") == 0)
  {
    if (response.content[0].compareTo("OK") == 0)
    {
      ESP_LOGI(this->NAME, "Connection is OK! Received message: %s", response.content[0].c_str());
      this->setState(RobotState::START);
      delay(1000);
      return true;
    }
  }

  ESP_LOGE(
      this->NAME,
      "Connection failed! Received message: header: %s",
      response.header.c_str());

  delay(1000);
  return false;
}

bool Controller::start()
{
  this->monitor->setRobotState("START");
  ESP_LOGI(this->NAME, "Running component's tasks...");

  this->runComponent(this->colorDetector);
  ESP_LOGI(this->NAME, "Run colorDetector's task successfully");

  this->runComponent(this->lineFollower);
  this->lineFollower->setEnable(true);
  ESP_LOGI(this->NAME, "Run lineFollower's task successfully");

  ESP_LOGI(this->NAME, "All components have run!");

  this->setNextArea(YELLOW);
  ESP_LOGI(this->NAME, "Next Area is YELLOW");
  this->setState(RobotState::PICKUP_TRANSIT);
  this->monitor->setRobotState("PICKUP_TS");

  // this->setState(IDLE);
  return true;
}

bool Controller::pickup()
{
  ESP_LOGI(this->NAME, "Pickup");
  delay(2000);
  this->setState(RobotState::CLASSIFY);
  this->monitor->setRobotState("CLASSIFY");
  // this->setState(RobotState::DROPOFF_TRANSIT);

  return true;
}

bool Controller::dropoff()
{
  ESP_LOGI(this->NAME, "Dropoff");
  this->setNextArea(YELLOW);
  ESP_LOGI(this->NAME, "Move to to YELLOW");
  delay(5000);
  this->setState(RobotState::PICKUP_TRANSIT);
  return true;
}

bool Controller::classify()
{
  delay(2000);
  ESP_LOGI(this->NAME, "Classifing...");

  if (!this->isClassifying)
  {
    std::vector<String> msg;
    msg.push_back("CLASSIFY");
    this->communicate->send("CLASSIFY", msg);
    delay(1000);
    if (this->communicate->isSentSuccess())
    {
      this->isClassifying = true;
    }
    return false;
  }

  CommunicateResponse response = this->communicate->getResponse();

  if (response.header.compareTo("OBJECT") != 0)
  {
    ESP_LOGE(this->NAME, "Wrong Header! Header: %s", response.header.c_str());
    return false;
  }

  if (response.content.size() != 1)
  {
    ESP_LOGE(this->NAME, "Content size is wrong! Content size: %s", response.content.size());
    return false;
  }

  std::vector<String> objInfo = StringUtils::splitString(response.content[0], ',');

  if (objInfo.size() != 6)
  {
    ESP_LOGE(this->NAME, "Wrong Object information size! bject information size: %s", objInfo.size());
    return false;
  }

  // ESP_LOGI(this->NAME, "Send object to web");
  // ProductTypeData data;
  // data.label = objInfo[ProductClassProps::LABEL];
  // data.accuration = objInfo[ProductClassProps::ACCURATION].toDouble();
  // data.x = objInfo[ProductClassProps::X].toInt();
  // data.y = objInfo[ProductClassProps::Y].toInt();
  // data.width = objInfo[ProductClassProps::WIDTH].toInt();
  // data.height = objInfo[ProductClassProps::HEIGHT].toInt();
  // this->webSocketClient->setProductTypeData(data);

  if (objInfo[ProductClassProps::LABEL] == "jerry")
  {
    this->setNextArea(BLUE);
  }
  else if (objInfo[ProductClassProps::LABEL] == "vit_vang")
  {
    this->setNextArea(RED);
  }
  else if (objInfo[ProductClassProps::LABEL] == "hiep_si")
  {
    this->setNextArea(GREEN);
  }
  else
  {
    ESP_LOGE(this->NAME, "Unknow the label! The label: %s", objInfo[0].c_str());
    return false;
  }

  this->setState(RobotState::DROPOFF_TRANSIT);
  this->monitor->setRobotState("DROPOFF_TS");
  this->isClassifying = false;
  std::vector<String> msg;
  msg.push_back("STOP_CLASSIFY");
  this->communicate->send("STOP_CLASSIFY", msg);

  ESP_LOGI(this->NAME, "Stop classify");
  ESP_LOGI(this->NAME, "This object is %s", objInfo[ProductClassProps::LABEL].c_str());
  return true;
}

bool Controller::idle()
{
  ESP_LOGI(this->NAME, "Idle");

  ColorSet currentColor = this->colorDetector->getColor().color;
  ESP_LOGI(this->NAME, "Current color: %s", this->colorDetector->colorToString(currentColor));
  return true;
}

bool Controller::pickupTransit()
{
  ESP_LOGI(this->NAME, "Pickup Transit");

  if (!this->isMoving)
  {
    this->lineFollower->setEnable(true);
    this->isMoving = true;
  }
  ColorSet currentColor = this->colorDetector->getColor().color;
  ESP_LOGI(this->NAME, "Current color: %s", this->colorDetector->colorToString(currentColor));
  if (currentColor == this->nextArea)
  {
    ESP_LOGI(this->NAME, "Arrived Pick up arera");
    if (this->isMoving)
    {
      this->lineFollower->setEnable(false);
      this->isMoving = false;
    }
    this->setState(RobotState::PICKUP);
    this->monitor->setRobotState("PICKUP");
    return true;
  }

  ESP_LOGI(this->NAME, "In Pick up transit....");

  return false;
}

bool Controller::dropoffTransit()
{
  ESP_LOGI(this->NAME, "Dropoff Transit");
  if (!this->isMoving)
  {
    this->lineFollower->setEnable(true);
    this->isMoving = true;
  }

  ColorSet currentColor = this->colorDetector->getColor().color;
  ESP_LOGI(this->NAME, "Current color: %s", this->colorDetector->colorToString(currentColor));
  if (currentColor == this->nextArea)
  {
    ESP_LOGI(this->NAME, "Arrived Drop off arera");
    if (this->isMoving)
    {
      this->lineFollower->setEnable(false);
      this->isMoving = false;
    }
    this->setState(RobotState::DROPOFF);
    this->monitor->setRobotState("DROPOFF");
    return true;
  }
  ESP_LOGI(this->NAME, "In Drop off transit....");
  return false;

  // this->setState(RobotState::DROPOFF);
  // return true;
}

bool Controller::setNextArea(ColorSet area)
{
  this->nextArea = area;
  return true;
}