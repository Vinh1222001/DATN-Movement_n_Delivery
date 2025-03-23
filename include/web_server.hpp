#pragma once
#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP

#include <Arduino.h>
#include <WiFi.h>
#include <FS.h>
#include <SPIFFS.h>
#include <WebServer.h>

#include "base_module.hpp"
#include "line_follower.hpp"

class RWebServer : public BaseModule
{
private:
  const char *ssid = RWEB_SERVER_SSID;
  const char *password = RWEB_SERVER_PASSWORD;
  WebServer *server = nullptr;

  LineFollower *lineFollower;

  void taskFn() override;

  void getIndex();
  void getLineFollower();

public:
  RWebServer(LineFollower *lineFollower);
  ~RWebServer();
};

#endif
