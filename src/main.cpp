#include <Arduino.h>

void setup()
{
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    delay(10); // Pause the program until serial port opens.
}

void loop()
{
  Serial.println("Testing new git");
  delay(3000);
}
