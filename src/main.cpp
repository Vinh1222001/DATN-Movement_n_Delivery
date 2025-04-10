#include "main.hpp"

websockets::WebsocketsClient wsClient;

void onMessage(websockets::WebsocketsMessage message)
{
  ESP_LOGI("WEB_SOCKET", "Got message: %s", message.data().c_str());
}

void onEvent(websockets::WebsocketsEvent event, websockets::WSInterfaceString message)
{
  ESP_LOGI("WEB_SOCKET", "Event: %d, Message: %s", event, message.c_str());
}

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

  if (!WifiUtil::initWifi("KAI Coffee", "camonban", true))
  {
    while (1)
    {
    }
  }
  String ip("ws://192.168.1.81:8081/");
  wsClient.onMessage(onMessage);
  wsClient.onEvent(onEvent);

  wsClient.setInsecure();

  if (!wsClient.connect(ip.c_str()))
  {
    ESP_LOGE("WEB_SOCKET", "Can't Connect to server");
  }
  else
  {
    wsClient.send("ESP32 hello server");
  }
}

void loop()
{
  if (wsClient.available())
  {
    wsClient.poll();
  }

  wsClient.send("Xin chafo");
  delay(1000);
}
