#include "main.hpp"

IPAddress localIP(192, 168, 2, 210);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(8, 8, 8, 8);
IPAddress secondaryDNS(8, 8, 4, 4);

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
  // delay(3000);
  // controller->run();

  WifiUtil::initWifi(
      WIFI_SSID,
      WIFI_PASSWORD,
      true,
      localIP,
      gateway,
      subnet,
      primaryDNS,
      secondaryDNS);

  RWebSocketClient *ws = new RWebSocketClient();
  ws->createTask();
  delay(3000);
  ws->run();
}

void loop()
{
  delay(1000);
}
