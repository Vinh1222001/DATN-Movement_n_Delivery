#pragma once
#ifndef WEB_SERVER_HPP
#define WEB_SERVER_HPP

#include <Arduino.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <ArduinoWebsockets.h>

#include "base_module.hpp"
#include "types.hpp"
#include "line_follower.hpp"
#include "color_detector.hpp"

using SocketMessage = Types::SemaphoreMutexData<JsonDocument>;

class RWebServer : public BaseModule
{
private:
  const char *url = RWEB_SERVER_BASE_URL;
  websockets::WebsocketsClient *client = nullptr;

  LineFollower *lineFollower;
  ColorDetector *colorDetector;

  SocketMessage messageSend;

  void taskFn() override;

  void onMessage(websockets::WebsocketsMessage message);
  void onEvent(websockets::WebsocketsEvent event, websockets::WSInterfaceString message);

public:
  RWebServer(LineFollower *lineFollower = nullptr, ColorDetector *colorDetector = nullptr);
  ~RWebServer();
};

#endif
