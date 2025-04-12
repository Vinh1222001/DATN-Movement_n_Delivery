#include "web_server.hpp"

RWebServer::RWebServer(LineFollower *lineFollower, ColorDetector *colorDetector)
    : BaseModule(
          "WEB_SERVER",
          RWEB_SERVER_TASK_PRIORITY,
          RWEB_SERVER_TASK_DELAY,
          RWEB_SERVER_TASK_STACK_DEPTH_LEVEL,
          RWEB_SERVER_TASK_PINNED_CORE_ID)
{
  this->lineFollower = lineFollower;
  this->colorDetector = colorDetector;

  this->messageSend.xMutex = xSemaphoreCreateMutex();

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

RWebServer::~RWebServer() {}

void RWebServer::taskFn()
{
  if (this->client->available())
  {
    this->client->poll();
  }

  if (xSemaphoreTake(this->messageSend.xMutex, portMAX_DELAY) == pdTRUE)
  {
    JsonDocument lineFollower;
    lineFollower["leftMost"] = millis();
    lineFollower["left"] = "hih";
    lineFollower["center"] = "hih";
    lineFollower["right"] = "hih";
    lineFollower["rightMost"] = "hih";
    lineFollower["decision"] = "hih";

    JsonDocument colorDetector;
    colorDetector["red"] = "hahahahah";
    colorDetector["green"] = "hahahahah";
    colorDetector["blue"] = "hahahahah";
    colorDetector["color"] = "hahahahah";

    String productType = "I don't know";

    this->messageSend.value["lineFollower"] = lineFollower;
    this->messageSend.value["colorDetector"] = colorDetector;
    this->messageSend.value["productType"] = productType;

    String JsonString;
    serializeJson(this->messageSend.value, JsonString);
    this->client->send(JsonString.c_str());

    xSemaphoreGive(this->messageSend.xMutex);
  }
}

void RWebServer::onMessage(websockets::WebsocketsMessage message)
{
  ESP_LOGI(this->NAME, "Got message: %s", message.data().c_str());
}

void RWebServer::onEvent(websockets::WebsocketsEvent event, websockets::WSInterfaceString message)
{
  ESP_LOGI(this->NAME, "Event: %d, Message: %s", event, message.c_str());

  // You can use `this` here too for class logic
}