#include "main.hpp"

void setup()
{
  ESP_LOGI("SET UP", "Set up Serial...");
  Serial.begin(CONFIG_MONITOR_BAUD);
  while (!Serial)
  {
    ESP_LOGE("SET UP", "Serial is not found!\n");
    delay(10);
  }

  Serial.setDebugOutput(true);

  esp_log_level_set("*", ESP_LOG_VERBOSE);
  ESP_LOGD("EXAMPLE", "This doesn't show");

  log_v("Verbose");
  log_d("Debug");
  log_i("Info");
  log_w("Warning");
  log_e("Error");

  ESP_LOGI("SET UP", "Initializing...\n");

  // Controller *controller = new Controller();

  // controller->createTask();
  // controller->run();

  ClassifyingCommunicate *communicate = new ClassifyingCommunicate();
  const uint8_t peerMac[6] = {0x08, 0xd1, 0xf9, 0x38, 0xa8, 0xac};
  if (communicate->begin(peerMac))
  {
    ESP_LOGI("ESP32 B", "ESP-NOW Initialized Successfully");
  }
  else
  {
    ESP_LOGE("ESP32 B", "Failed to initialize ESP-NOW");
  }
}

void loop()
{
}
