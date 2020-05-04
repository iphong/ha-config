#include "esphome.h"

class MyCustom : public Component {
public:
	char buffer[1024];
	int size = 0;
	int blank = 0;
	bool found;
	void setup() override {
		Serial.begin(9600);
	}
	void loop() override {
		while (Serial.available()) {
			char b = Serial.read();
			if (b != 0x00) {
				buffer[size++] = b;
			} else if (size && b == 0x00) {
				if (size > 10) {
					for (int i=0; i<size; i++) {
						Serial.print(buffer[i], HEX);
						Serial.print(' ');
					}
					Serial.println();
					found = true;
				}
				size = 0;

				if (found && blank++ > 8) {
					found = false;
					blank = 0;
					Serial.print("\n\n-----------------------------------------\n\n");
				}
			}
		}
	}
};
