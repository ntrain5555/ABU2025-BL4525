#include <Arduino.h>
#include "wheel.h"

char strbuf[1024];

Wheel *w_main = nullptr;

void setup()
{
	w_main = new Wheel(16, 31, 18);
	Serial.begin(115200);
	w_main->setSpeed(12);

	sleep(1);
	w_main->printConfiguration();
}

void loop()
{
	w_main->printSpeed();
}
