#include "main.hpp"

// Test
#include "shift_register.hpp"
#include "motor_driver.hpp"

ShiftRegister sr;
MotorDriver motorDriver;

bool value[4][8] = {
    {0, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0},
};

int idx = 0;

void setup()
{
  // put your setup code here, to run once:
  ESP_LOGI("SET UP", "Set up Serial...");
  Serial.begin(CONFIG_MONITOR_BAUD);
  while (!Serial)
  {
    ESP_LOGI("SET UP", "Serial is not found!\n");
    delay(10); // Pause the program until serial port opens.
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

  // lineColorTracker = new LineColorTracker();
  lineFollower = new LineFollower();
  lineFollower->run();
  // mpuReader = new MPUReader();
  // ultraSonicReader = new UltraSonicReader();
  // sr.run();

  // pinMode(33, OUTPUT);
  // pinMode(25, OUTPUT);
  // pinMode(26, OUTPUT);
  // pinMode(27, OUTPUT);

  // digitalWrite(33, HIGH);
  // digitalWrite(25, LOW);

  // digitalWrite(26, HIGH);
  // digitalWrite(27, LOW);
}

void loop()
{
  // ESP_LOGI("MOTOR DRIVER", "Move forward...\n");
  // motorDriver.moveFoward();
  // delay(3000);

  // ESP_LOGI("MOTOR DRIVER", "Move backward...\n");
  // motorDriver.moveBackward();
  // delay(3000);

  // ESP_LOGI("MOTOR DRIVER", "Move left...\n");
  // motorDriver.moveLeft();
  // delay(3000);

  // ESP_LOGI("MOTOR DRIVER", "Move right...\n");
  // motorDriver.moveRight();
  // delay(3000);

  // ESP_LOGI("MOTOR DRIVER", "Stopped...\n");
  // motorDriver.stop();
  // delay(3000);
}
