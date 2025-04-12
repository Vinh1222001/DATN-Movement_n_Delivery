#pragma once
#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>

#include "base_module.hpp"
#include "line_follower.hpp"
#include "color_detector.hpp"

class RWebServer : public BaseModule
{
private:
  const char *ssid = RWEB_SERVER_SSID;
  const char *password = RWEB_SERVER_PASSWORD;
  WebServer *server = nullptr;

  LineFollower *lineFollower;
  ColorDetector *colorDetector;

  void taskFn() override;

  void onResponse(String data);

  void getIndex();
  void getLineFollower();
  void getColor();

public:
  RWebServer(LineFollower *lineFollower, ColorDetector *colorDetector);
  ~RWebServer();
};

#endif
