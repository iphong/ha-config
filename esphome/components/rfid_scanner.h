#include "esphome.h"

class RFID_Scanner : public Component, public CustomMQTTDevice {
public:
  char buffer[10];
  void setup() override {
    Serial.begin(9600);
  }
  void loop() override {
    if (Serial.available() >= 12) {
      if (Serial.read() == 0x02) {
        Serial.readBytes(buffer, 10);
        Serial.read();
        if (Serial.read() == 0x03) {
          Serial.printf("RFID: %s \n", buffer);
          publish("home/card/id", buffer);
        }
      }
    }
  }
};
