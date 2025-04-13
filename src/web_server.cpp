#include "web_server.hpp"

RWebSocketClient::RWebSocketClient()
    : BaseModule(
          "WEB_SOCKET_CLIENT",
          RWEB_SERVER_TASK_PRIORITY,
          RWEB_SERVER_TASK_DELAY,
          RWEB_SERVER_TASK_STACK_DEPTH_LEVEL,
          RWEB_SERVER_TASK_PINNED_CORE_ID)
{
  this->lineFollowerMessage.xMutex = xSemaphoreCreateMutex();
  this->colorDetectorMessage.xMutex = xSemaphoreCreateMutex();
  this->productTypeMessage.xMutex = xSemaphoreCreateMutex();

  this->client = new websockets::WebsocketsClient();

  this->client->onMessage(
      [this](websockets::WebsocketsMessage msg)
      {
        this->onMessage(msg);
      });

  this->client->onEvent(
      [this](websockets::WebsocketsEvent event, websockets::WSInterfaceString message)
      {
        this->onEvent(event, message);
      });

  this->client->setInsecure();

  if (this->client->connect(this->url))
  {
    ESP_LOGE(this->NAME, "Can't Connect to server");
  }
  else
  {
    this->client->send("ESP32 hello server");
  }
}

RWebSocketClient::~RWebSocketClient() {}

void RWebSocketClient::taskFn()
{
  if (this->client->available())
  {
    this->client->poll();
  }
  JsonDocument message;
  JsonDocument lineFollower;
  JsonDocument colorDetector;
  JsonDocument productType;

  if (xSemaphoreTake(this->lineFollowerMessage.xMutex, portMAX_DELAY) == pdTRUE)
  {
    lineFollower["leftMost"] = this->lineFollowerMessage.value.leftMost;
    lineFollower["left"] = this->lineFollowerMessage.value.left;
    lineFollower["center"] = this->lineFollowerMessage.value.center;
    lineFollower["right"] = this->lineFollowerMessage.value.right;
    lineFollower["rightMost"] = this->lineFollowerMessage.value.rightMost;
    lineFollower["decision"] = this->lineFollowerMessage.value.decision;
    xSemaphoreGive(this->lineFollowerMessage.xMutex);
  }

  if (xSemaphoreTake(this->colorDetectorMessage.xMutex, portMAX_DELAY) == pdTRUE)
  {
    colorDetector["red"] = this->colorDetectorMessage.value.red;
    colorDetector["green"] = this->colorDetectorMessage.value.green;
    colorDetector["blue"] = this->colorDetectorMessage.value.blue;
    colorDetector["color"] = this->colorDetectorMessage.value.color;
    xSemaphoreGive(this->colorDetectorMessage.xMutex);
  }

  if (xSemaphoreTake(this->productTypeMessage.xMutex, portMAX_DELAY) == pdTRUE)
  {
    productType["label"] = this->productTypeMessage.value.label;
    productType["accuration"] = this->productTypeMessage.value.accuration;
    productType["x"] = this->productTypeMessage.value.x;
    productType["y"] = this->productTypeMessage.value.y;
    productType["width"] = this->productTypeMessage.value.width;
    productType["height"] = this->productTypeMessage.value.height;
    xSemaphoreGive(this->productTypeMessage.xMutex);
  }

  message["lineFollower"] = lineFollower;
  message["colorDetector"] = colorDetector;
  message["productType"] = productType;

  String JsonString;
  serializeJson(message, JsonString);
  this->client->send(JsonString.c_str());
}

void RWebSocketClient::onMessage(websockets::WebsocketsMessage message)
{
  ESP_LOGI(this->NAME, "Got message: %s", message.data().c_str());
}

void RWebSocketClient::onEvent(websockets::WebsocketsEvent event, websockets::WSInterfaceString message)
{
  ESP_LOGI(this->NAME, "Event: %d, Message: %s", event, message.c_str());

  // You can use `this` here too for class logic
}

void RWebSocketClient::setLineFollowerData(LineFollowerData data)
{
  if (xSemaphoreTake(this->lineFollowerMessage.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->lineFollowerMessage.value.leftMost = data.leftMost;
    this->lineFollowerMessage.value.left = data.left;
    this->lineFollowerMessage.value.center = data.center;
    this->lineFollowerMessage.value.right = data.right;
    this->lineFollowerMessage.value.rightMost = data.rightMost;
    this->lineFollowerMessage.value.decision = data.decision;

    xSemaphoreGive(this->lineFollowerMessage.xMutex);
  }
}

void RWebSocketClient::setColorDetectorData(ColorDetectorData data)
{
  if (xSemaphoreTake(this->colorDetectorMessage.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->colorDetectorMessage.value.red = data.red;
    this->colorDetectorMessage.value.green = data.green;
    this->colorDetectorMessage.value.blue = data.blue;
    this->colorDetectorMessage.value.color = data.color;

    xSemaphoreGive(this->colorDetectorMessage.xMutex);
  }
}

void RWebSocketClient::setProductTypeData(ProductTypeData data)
{
  if (xSemaphoreTake(this->productTypeMessage.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->productTypeMessage.value.label = data.label;
    this->productTypeMessage.value.accuration = data.accuration;
    this->productTypeMessage.value.x = data.x;
    this->productTypeMessage.value.y = data.y;
    this->productTypeMessage.value.width = data.width;
    this->productTypeMessage.value.height = data.height;

    xSemaphoreGive(this->productTypeMessage.xMutex);
  }
}
