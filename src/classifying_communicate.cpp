#include "classifying_communicate.hpp"

// ESP32 CAM MAC: 08:d1:f9:38:a8:ac

ClassifyingCommunicate *ClassifyingCommunicate::instance = nullptr;

ClassifyingCommunicate::ClassifyingCommunicate()
    : BaseModule("CLASSIFYING_COMMUNICATE")
{
  instance = this;
}

ClassifyingCommunicate::~ClassifyingCommunicate() {}

bool ClassifyingCommunicate::begin()
{
  if (esp_now_init() != ESP_OK)
  {
    ESP_LOGE(this->NAME, "ESP-NOW init failed");
    return false;
  }

  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, peerMac, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  if (!esp_now_is_peer_exist(peerMac))
  {
    if (esp_now_add_peer(&peerInfo) != ESP_OK)
    {
      ESP_LOGE(this->NAME, "Failed to add peer");
      return false;
    }
  }

  // Register both send and receive callbacks
  esp_now_register_send_cb(onDataSentStatic);
  esp_now_register_recv_cb(onDataRecvStatic);

  ESP_LOGI(this->NAME, "ESP-NOW init successful");
  return true;
}

bool ClassifyingCommunicate::send(const bool &data)
{
  const Types::EspNowMessage _msg = SetUtils::createEspNowMessage<bool>(data);
  esp_err_t result = esp_now_send(peerMac, reinterpret_cast<const uint8_t *>(&_msg), sizeof(_msg));
  if (result == ESP_OK)
  {
    ESP_LOGI(this->NAME, "Sent data successfully");
    return true;
  }
  else
  {
    ESP_LOGE(this->NAME, "Failed to send data: %d", result);
    return false;
  }
}

void ClassifyingCommunicate::onDataSentStatic(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  if (instance)
    instance->onDataSent(mac_addr, status);
}

void ClassifyingCommunicate::onDataRecvStatic(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  if (instance)
    instance->onDataRecv(mac, incomingData, len);
}

void ClassifyingCommunicate::onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status)
{
  ESP_LOGI(this->NAME, "Data sent to %02X:%02X:%02X:%02X:%02X:%02X - Status: %s",
           mac_addr[0], mac_addr[1], mac_addr[2],
           mac_addr[3], mac_addr[4], mac_addr[5],
           status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

void ClassifyingCommunicate::onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len)
{
  ESP_LOGI(this->NAME, "Received data from %02X:%02X:%02X:%02X:%02X:%02X, length: %d",
           mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], len);

  // Chuyển đổi dữ liệu thành Message
  const Types::EspNowMessage *packet = reinterpret_cast<const Types::EspNowMessage *>(incomingData);
  ESP_LOGI(this->NAME, "Data Received: Id: %s, Value: %s", packet->id, packet->content);
  // TODO: Bạn có thể thêm logic xử lý dữ liệu ở đây
  send(true);
}

void ClassifyingCommunicate::taskFn()
{
}