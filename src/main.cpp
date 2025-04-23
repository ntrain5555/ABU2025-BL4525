#include <Arduino.h>
#include "wheel.h"

char strbuf[1024];

Wheel *w_main = nullptr;

void setup()
{
	w_main = new Wheel(23, 33, 19);
	Serial.begin(115200);
	w_main->speed.curr = 10;
}

void loop() {
	w_main->speed.direction = Wheel::FORWARD;
}