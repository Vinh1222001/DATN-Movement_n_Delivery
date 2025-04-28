#include "main.hpp"

#define WIFI_SSID "Vinhtage"
#define WIFI_PASSWORD "haochoancuc"
const char *TAG = "SETUP";

IPAddress localIP(192, 168, 2, 210);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

void setup()
{
  ESP_LOGI(TAG, "Set up Serial...");
  Serial.begin(CONFIG_MONITOR_BAUD);
  while (!Serial)
  {
    ESP_LOGE(TAG, "Serial is not found!\n");
    delay(10);
  }

  Serial.setDebugOutput(true);

  esp_log_level_set("*", ESP_LOG_VERBOSE);

  log_v("Verbose");
  log_d("Debug");
  log_i("Info");
  log_w("Warning");
  log_e("Error");

  ESP_LOGI(TAG, "Initializing...\n");

  while (
      !WifiUtil::initWifi(
          WIFI_SSID,
          WIFI_PASSWORD,
          true,
          localIP,
          gateway,
          subnet,
          primaryDNS,
          secondaryDNS))
  {
    ESP_LOGI(TAG, "Retry to connect wifi! SSID: %s, PASSWORD: %s", WIFI_SSID, WIFI_PASSWORD);
    delay(1000);
  }

  Controller *controller = new Controller();

  if (controller == nullptr)
  {
    ESP_LOGE(TAG, "Failed to setup robot");
    return;
  }

  controller->createTask();
  delay(2000);
  controller->run();
}

void loop()
{
  delay(1);
}
