#include "esphome.h"

class Touch {
public:
	int pin;
	int state;
	int index;
	unsigned long last_press;
	unsigned long last_release;
};

class TouchDimmer : public Component {
public:
	Sensor *value = new Sensor();

	TouchDimmer(int size, int * pins) {
		_size = size;
		for (int i=0; i < _size; i++) {
			_touches[i] = { pins[i], HIGH, i };
		}
	}
	void setup() {
		value->publish_state(id(last_value));
	}
	void loop() {
		read_state();
		if (_lastPress && _lastRelease && is_touch_end() && millis() - _lastRelease->last_release > 50) {
			if (!swiping && _lastPress->index == _lastRelease->index) {
				ESP_LOGD("touch", "tap detected button %i", _lastRelease->index);
				if (on_tap) on_tap();
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
							if (steps > 0) on_swipe_up();
							else on_swipe_down();
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
	void on_swipe_up() {
		if (value->state < 10){
			value->publish_state(value->state + 1);
			id(last_value) = value->state;
		}
	}
	void on_swipe_down() {
		if (value->state > 0){
			value->publish_state(value->state - 1);
			id(last_value) = value->state;
		}
	}

protected:
	int _size;
	bool swiping;
	Touch _touches[9];
	Touch *_lastPress = NULL;
	Touch *_lastRelease = NULL;
};