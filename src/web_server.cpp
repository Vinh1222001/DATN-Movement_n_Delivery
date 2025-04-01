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
  this->server->on("/color", std::bind(&RWebServer::getColor, this));

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
  LineFollowerSensorValues sensor = this->lineFollower->getLineFollowerValues();

  StaticJsonDocument<200> doc;
  doc["out1"] = sensor.out1;
  doc["out2"] = sensor.out2;
  doc["out3"] = sensor.out3;
  doc["out4"] = sensor.out4;
  doc["out5"] = sensor.out5;

  String jsonString;
  serializeJson(doc, jsonString);
  this->onResponse(jsonString);
}

void RWebServer::getColor()
{
  ColorRGB color = this->colorDetector->getColor();

  StaticJsonDocument<200> doc;
  doc["red"] = color.red;
  doc["green"] = color.green;
  doc["blue"] = color.blue;

  String jsonString;
  serializeJson(doc, jsonString);

  this->onResponse(jsonString);
}

void RWebServer::onResponse(String data)
{
  this->server->sendHeader("Access-Control-Allow-Origin", "*");
  this->server->send(200, "application/json", data);
}

void RWebServer::taskFn()
{
  this->server->handleClient();
}
