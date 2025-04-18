#include <Arduino.h>
#include "wheel.h"

Wheel::Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir, uint8_t speed_min, uint8_t speed_max) : pin{pin_pwm, pin_fg, pin_dir},
                                                                                                       speed{0, speed_min, speed_max, FORWARD}
{
    pinMode(pin.pwm, OUTPUT);
    pinMode(pin.fg, INPUT_PULLUP);
    pinMode(pin.direction, OUTPUT);
    xTaskCreate([](void *param)
                {
        Wheel *w = (Wheel *) param;
        uint8_t speed = 0;
        uint32_t pulse_length;
        direction direction = w->speed.direction;
        while (true) {
            pulse_length = pulseIn(w->pin.fg, HIGH);
            // On direction changed
            if (direction != w->speed.direction) {
                direction = w->speed.direction;
                while (pulseIn(w->pin.fg, HIGH) != 0) {
                    digitalWrite(w->pin.pwm, LOW);
                }
                digitalWrite(w->pin.direction, (direction == FORWARD) ? HIGH : LOW);
            }
            // On speed changed
            if (pulse_length > w->expected_pulse_length) {
                analogWrite(w->pin.pwm, +speed);
            } else if (pulse_length < w->expected_pulse_length) {
                analogWrite(w->pin.pwm, --speed);
            }
            vTaskDelay(1 / portTICK_PERIOD_MS);
        } }, "Speed Retaining", 4000, this, 1, NULL);
}

Wheel::Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir) : Wheel(pin_pwm, pin_fg, pin_dir, 12, 230) {}

void Wheel::setSpeed(uint8_t speed_target)
{
    speed_target = (speed_target > speed.max) ? speed.max : speed_target;
    speed_target = (speed_target < speed.min) ? speed.min : speed_target;
    speed.curr = speed_target;
}

void Wheel::setDirection(enum direction direction)
{
    speed.direction = direction;
}

void Wheel::printConfiguration(void) {
	Serial.printf("Current pin configuration\nPWM: %hd\nFG: %hd\nDIR: %hd\n", pin.pwm, pin.fg, pin.direction);
}

void Wheel::printSpeed(void)
{
    Serial.printf("Speed: %d -> %u\n", speed.curr, pulseIn(pin.fg, HIGH));
}