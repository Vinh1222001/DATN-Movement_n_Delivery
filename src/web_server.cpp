#include "web_server.hpp"

RWebServer::RWebServer(LineFollower *lineFollower)
    : BaseModule(
          "WEB_SERVER",
          RWEB_SERVER_TASK_PRIORITY,
          RWEB_SERVER_TASK_DELAY,
          RWEB_SERVER_TASK_STACK_DEPTH_LEVEL,
          RWEB_SERVER_TASK_PINNED_CORE_ID)
{
  this->lineFollower = lineFollower;

  this->server = new WebServer(80);

  if (!SPIFFS.begin(true))
  {
    ESP_LOGE(this->NAME, "SPIFFS Mount Failed");
    return;
  }

  WiFi.begin(this->ssid, this->password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    ESP_LOGE(this->NAME, ".");
  }
  ESP_LOGI(this->NAME, "WiFi connected. IP: %s", WiFi.localIP().toString());

  this->server->on("/", std::bind(&RWebServer::getIndex, this));
  this->server->on("/line-follower", std::bind(&RWebServer::getLineFollower, this));
  this->server->begin();
}

RWebServer::~RWebServer() {}

void RWebServer::getIndex()
{
  File file = SPIFFS.open("/index.html", "r");
  if (!file)
  {
    this->server->send(500, "text/plain", "Failed to open file");
    return;
  }

  this->server->streamFile(file, "text/html");
  file.close();
}

void RWebServer::getLineFollower()
{
  SensorValues value = this->lineFollower->getLineFollowerValues();
  String data = "[\"" + String(value.out1) + "\",\"" + String(value.out2) + "\",\"" + String(value.out3) + "\",\"" + String(value.out4) + "\",\"" + String(value.out5) + "\"]";
  ESP_LOGI(this->NAME, "Sending data: %s", data.c_str());
  this->server->sendHeader("Access-Control-Allow-Origin", "*");
  this->server->send(200, "application/json", data);
}

void RWebServer::taskFn()
{
  this->server->handleClient();
}
