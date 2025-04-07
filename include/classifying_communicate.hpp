#pragma once
#ifndef CLASSIFYING_COMMUNICATE_HPP
#define CLASSIFYING_COMMUNICATE_HPP

#include "base_module.hpp"
#include <esp_now.h>
#include "esp_log.h"
#include "types.hpp"
#include "utils/set.hpp"
class ClassifyingCommunicate : public BaseModule
{
private:
  static ClassifyingCommunicate *instance; // Static pointer to the current instance
  const uint8_t peerMac[6] = {0x08, 0xd1, 0xf9, 0x38, 0xa8, 0xac};

  // Static callbacks
  static void onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status);
  static void onDataRecvStatic(const uint8_t *mac, const uint8_t *incomingData, int len);

  // Instance methods to call from static
  void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
  void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);
  void taskFn() override;

public:
  ClassifyingCommunicate();
  ~ClassifyingCommunicate();

  bool begin();
  bool send(const bool &data);
};

#endif
