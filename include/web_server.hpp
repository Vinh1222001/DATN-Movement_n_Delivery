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

struct LineFollowerData
{
  int leftMost;
  int left;
  int center;
  int right;
  int rightMost;
  String decision;
};

struct ColorDetectorData
{
  int red;
  int green;
  int blue;
  String color;
};

struct ProductTypeData
{
  String label;
  double accuration;
  int x;
  int y;
  int width;
  int height;
};

using LineFollowerMessage = Types::SemaphoreMutexData<LineFollowerData>;
using ColorDetectorMessage = Types::SemaphoreMutexData<ColorDetectorData>;
using ProductTypeMessage = Types::SemaphoreMutexData<ProductTypeData>;

class RWebSocketClient : public BaseModule
{
private:
  const char *url = RWEB_SERVER_BASE_URL;
  websockets::WebsocketsClient *client = nullptr;

  LineFollowerMessage lineFollowerMessage;
  ColorDetectorMessage colorDetectorMessage;
  ProductTypeMessage productTypeMessage;

  void taskFn() override;

  void onMessage(websockets::WebsocketsMessage message);
  void onEvent(websockets::WebsocketsEvent event, websockets::WSInterfaceString message);

public:
  RWebSocketClient();
  ~RWebSocketClient();

  void setLineFollowerData(LineFollowerData data);
  void setColorDetectorData(ColorDetectorData data);
  void setProductTypeData(ProductTypeData data);
};

#endif
