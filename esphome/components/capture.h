#include "esphome.h"

class Touch {
public:
	int pin;
	int state;
	int index;
	unsigned long last_press;
	unsigned long last_release;
};

class Capture: public Component, public CustomAPIDevice {
public:
	Capture(int size, int * pins) {
		_size = size;
		for (int i=0; i < _size; i++) {
			_touches[i] = { pins[i], HIGH, i };
		}
	}
	void setup() {

	}
	void loop() {
		read_state();
		if (_lastPress && _lastRelease && is_touch_end() && millis() - _lastRelease->last_release > 50) {
			if (!swiping && _lastPress->index == _lastRelease->index) {
				ESP_LOGD("touch", "tap detected button %i", _lastRelease->index);
				if (on_tap) on_tap();
				switch (_lastRelease->index) {
					case 0: {
//						id(state_1) = !id(state_1);
						id(sensor_1).publish_state(!id(sensor_1).state);
						break;
					}
					case 1: {
//						id(state_2) = !id(state_2);
						id(sensor_2).publish_state(!id(sensor_2).state);
						break;
					}
					case 2: {
//						id(state_3) = !id(state_3);
						id(sensor_3).publish_state(!id(sensor_3).state);
						break;
					}
				}
			}
			_lastPress = NULL;
			_lastRelease = NULL;
			swiping = false;
			ESP_LOGD("touch", "touching has ended");
		}
	}
	void read_state() {
		int state;
		for (int i=0; i < _size; i++) {
			state = digitalRead(_touches[i].pin);
			if (state != _touches[i].state) {
				if (state == LOW) {
					_touches[i].last_press = millis();
					if (_lastPress == NULL) {
						ESP_LOGD("touch", "Touch begins %i.", i);
					} else {
						swiping = true;
						int duration = _touches[i].last_press - _lastPress->last_press;
						int steps = _touches[i].index - _lastPress->index;
						if (steps != 0) {
							ESP_LOGD("touch", "swipe detected %i in %i ms.", steps, duration);
							if (steps > 0 && on_swipe_up) on_swipe_up();
							else if (on_swipe_down) on_swipe_down();
						}
					}
					_lastPress = &_touches[i];
				} else {
					_touches[i].last_release = millis();
					_lastRelease = &_touches[i];
				}
			}
			_touches[i].state = state;
		}
	}
	bool is_multiple_touches() {
		int count = 0;
		for (int i=0; i < _size; i++) {
			if (_touches[i].state == LOW) count++;
		}
		return count > 1;
	}
	bool is_touch_end() {
		int count = 0;
		for (int i=0; i < _size; i++) {
			if (_touches[i].state == LOW) count++;
		}
		return count == 0;
	}
	void (*on_tap)();
	void (*on_swipe_up)();
	void (*on_swipe_down)();

protected:
	int _size;
	bool swiping;
	Touch _touches[9];
	Touch *_lastPress = NULL;
	Touch *_lastRelease = NULL;
};