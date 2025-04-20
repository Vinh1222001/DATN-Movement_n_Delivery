#include "controller.hpp"
#include "utils/stringUtils.hpp"

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
  uint8_t mac[6] = {0x08, 0xd1, 0xf9, 0x38, 0xa8, 0xac};
  this->communicate = new Communicate(mac);
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
  // this->setState(PING);
  this->setState(START);

  return true;
}

bool Controller::ping()
{
  std::vector<String> pingMsg;
  pingMsg.push_back("Request to connect...");
  this->communicate->send("PING", pingMsg);

  CommunicateResponse response = this->communicate->getResponse();
  if (response.header.compareTo("RESPONSE") == 0)
  {
    if (response.content[0].compareTo("OK"))
    {
      ESP_LOGI(this->NAME, "Connection is OK! Received message: %s", response.content[0].c_str());
      this->setState(START);
      delay(1000);
      return true;
    }
  }

  ESP_LOGE(
      this->NAME,
      "Connection failed! Received message: %s",
      response.content[0].c_str());
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

  this->setNextArea(YELLOW);
  this->setState(PICKUP_TRANSIT);
  // this->setState(IDLE);
  return true;
}

bool Controller::pickup()
{
  ESP_LOGI(this->NAME, "Pickup");

  ESP_LOGI(this->NAME, "Delivery to BLUE");
  delay(2000);
  this->setState(CLASSIFY);

  return true;
}

bool Controller::dropoff()
{
  ESP_LOGI(this->NAME, "Dropoff");
  this->setNextArea(YELLOW);
  ESP_LOGI(this->NAME, "Move to to YELLOW");
  delay(2000);
  this->setState(PICKUP_TRANSIT);
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
    this->communicate->send("CLASSIFY", msg);
    this->isClassifying = true;
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

  ProductTypeData data;
  data.label = objInfo[0];
  data.accuration = objInfo[1].toDouble();
  data.x = objInfo[2].toInt();
  data.y = objInfo[3].toInt();
  data.width = objInfo[4].toInt();
  data.height = objInfo[5].toInt();
  this->webSocketClient->setProductTypeData(data);

  std::vector<String> msg;
  msg.push_back("STOP_CLASSIFY");
  this->communicate->send("STOP_CLASSIFY", msg);

  if (objInfo[0] == "jerry")
  {
    this->setNextArea(BLUE);
  }
  else if (objInfo[0] == "vit_lego_vang")
  {
    this->setNextArea(RED);
  }
  else if (objInfo[0] == "hiep_si")
  {
    this->setNextArea(GREEN);
  }
  else
  {
    ESP_LOGE(this->NAME, "Unknow the label! The label: %s", objInfo[0].c_str());
    return false;
  }

  std::vector<String> msg;
  msg.push_back("STOP_CLASSIFY");
  this->communicate->send("STOP_CLASSIFY", msg);
  this->setState(DROPOFF_TRANSIT);

  ESP_LOGI(this->NAME, "Classify is %s", objInfo[0].c_str());

  delay(1000);
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
  if (this->lineFollower == nullptr || this->colorDetector == nullptr)
  {
    ESP_LOGE(this->NAME, "Some component is still null");
    return false;
  }

  this->lineFollower->setEnable(true);
  ColorSet currentColor = this->colorDetector->getColor().color;
  ESP_LOGI(this->NAME, "Current color: %s", this->colorDetector->colorToString(currentColor));
  if (currentColor == this->nextArea)
  {
    ESP_LOGI(this->NAME, "Arrived Pick up arera");
    this->lineFollower->setEnable(false);
    this->setState(PICKUP);
    delay(3000);
    return true;
  }

  ESP_LOGI(this->NAME, "In Pick up transit....");
  return false;
}

bool Controller::dropoffTransit()
{
  ESP_LOGI(this->NAME, "Dropoff Transit");
  this->lineFollower->setEnable(true);

  if (this->lineFollower == nullptr || this->colorDetector == nullptr)
  {
    ESP_LOGE(this->NAME, "Some component is still null");
    return false;
  }

  ColorSet currentColor = this->colorDetector->getColor().color;
  ESP_LOGI(this->NAME, "Current color: %s", this->colorDetector->colorToString(currentColor));
  if (currentColor == this->nextArea)
  {
    ESP_LOGI(this->NAME, "Arrived Drop off arera");
    this->lineFollower->setEnable(false);
    this->setState(DROPOFF);
    delay(3000);
    return true;
  }
  ESP_LOGI(this->NAME, "In Drop off transit....");
  return false;
}

bool Controller::setNextArea(ColorSet area)
{
  this->nextArea = area;
  return true;
}