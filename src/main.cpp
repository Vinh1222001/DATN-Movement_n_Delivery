#include <Arduino.h>
#include "line_follower_reader.hpp"

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.

  line_follower_reader_init();

  xTaskCreatePinnedToCore(line_follower_read, "LINE_FOLOWER_READER", 5120, nullptr, 10, nullptr, 1);
}

void loop() {}
