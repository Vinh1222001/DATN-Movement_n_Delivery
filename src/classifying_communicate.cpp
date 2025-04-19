#include "classifying_communicate.hpp"

// ESP32 CAM MAC: 08:d1:f9:38:a8:ac

ClassifyingCommunicate *ClassifyingCommunicate::instance = nullptr;

ClassifyingCommunicate::ClassifyingCommunicate()
    : BaseModule("CLASSIFYING_COMMUNICATE")
{
  instance = this;
  this->receiveMsg.xMutex = xSemaphoreCreateMutex();
  this->begin();
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

bool ClassifyingCommunicate::send(const std::vector<String> &data)
{
  String combined;
  for (size_t i = 0; i < data.size(); ++i)
  {
    combined += data[i];
    if (i < data.size() - 1)
      combined += ","; // dùng dấu phẩy để phân tách
  }
  char hold[200];

  const Types::EspNowMessage msg = SetUtils::createEspNowMessage<String>(combined);

  ESP_LOGI(this->NAME, "Data send: Id:%s and content: %s, size: %d", msg.id, msg.content, sizeof(msg));
  esp_err_t result = esp_now_send(peerMac, reinterpret_cast<const uint8_t *>(&msg), sizeof(msg));
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
  if (xSemaphoreTake(this->receiveMsg.xMutex, portMAX_DELAY) == pdTRUE)
  {
    this->receiveMsg.value = String(packet->content);
    xSemaphoreGive(this->receiveMsg.xMutex);
  }
}

void ClassifyingCommunicate::taskFn()
{
}

String ClassifyingCommunicate::getReceiveMsg()
{
  String msg;
  if (xSemaphoreTake(this->receiveMsg.xMutex, portMAX_DELAY) == pdTRUE)
  {
    if (this->receiveMsg.value.length() > 0)
    {
      msg = this->receiveMsg.value;
    }

    xSemaphoreGive(this->receiveMsg.xMutex);
  }

  return msg;
}