#include "esphome.h"

class CustomUART : public Component, public UARTDevice {
public:
	CustomUART(UARTComponent *parent) : UARTDevice(parent) { }

	void setup() override {

	}

	void loop() override {

	}
};