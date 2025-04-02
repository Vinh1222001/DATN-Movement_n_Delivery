#include "classifying_communicate.hpp"

ClassifyingCommunicate::ClassifyingCommunicate()
    : BaseModule(
          "CLASSIFYING_COMMUNICATE",
          CLASSIFYING_COMMUNICATE_TASK_PRIORITY,
          CLASSIFYING_COMMUNICATE_TASK_DELAY,
          CLASSIFYING_COMMUNICATE_TASK_STACK_DEPTH_LEVEL,
          CLASSIFYING_COMMUNICATE_TASK_PINNED_CORE_ID)
{
  this->communicate = new HardwareSerial(2);

  this->communicate->begin(
      9600,
      SERIAL_8N1,
      CLASSIFYING_COMMUNICATE_PIN_RXD,
      CLASSIFYING_COMMUNICATE_PIN_TXD);
}

ClassifyingCommunicate::~ClassifyingCommunicate() {}

void ClassifyingCommunicate::taskFn()
{
  this->communicate->println("Hello from ESP32!");

  if (this->communicate->available())
  {
    String message = this->communicate->readString();
    ESP_LOGI(this->NAME, "Recieved: %s", message);
  }
}