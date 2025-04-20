#include <Arduino.h>
#include "wheel.h"

#define MIN_PULSE_LENGTH 1400UL
#define MAX_PULSE_LENGTH 23000UL

Wheel::Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir, uint8_t pwm_min, uint8_t pwm_max) : pin{pin_pwm, pin_fg, pin_dir},
                                                                                                   speed{0, FORWARD},
                                                                                                   pwm{pwm_min, pwm_max}
{
    pinMode(pin.pwm, OUTPUT);
    pinMode(pin.fg, INPUT_PULLUP);
    pinMode(pin.direction, OUTPUT);
    xTaskCreatePinnedToCore([](void *param)
                {
        Wheel *w = (Wheel *) param;
        uint8_t pwm = 0;
        struct {
            const uint16_t min = 1E+6 / MAX_PULSE_LENGTH;
            const uint16_t max = 1E+6 / MIN_PULSE_LENGTH;
            const double_t step = (max - min) / 255.0;
            uint16_t expected;
            uint16_t curr;
        } frequency;
        uint32_t pulse_length;
        direction direction = w->speed.direction;
        digitalWrite(w->pin.direction, (direction == FORWARD) ? HIGH : LOW);
        while (true) {
            pulse_length = pulseIn(w->pin.fg, LOW, MAX_PULSE_LENGTH * 2);
            frequency.curr = 1E+6 / pulse_length;
            frequency.expected = frequency.min + (frequency.step * w->speed.curr);
            // On direction changed
            if (direction != w->speed.direction) {
                direction = w->speed.direction;
                // Slow down
                while (pulseIn(w->pin.fg, LOW, 50000) != 0) {
                    analogWrite(w->pin.pwm, 0);
                }
                digitalWrite(w->pin.direction, (direction == FORWARD) ? HIGH : LOW);
                pwm = 0;
            }
            // Retaining speed and brake
            if (w->speed.curr == 0) { // Brake
                pwm = 0;
            } else if (pulse_length == 0 || frequency.curr < frequency.expected) { // Too slow
                pwm = (pwm >= w->pwm.max) ? w->pwm.max : ++pwm;
            } else if (frequency.curr > frequency.expected) { // Too fast
                pwm = (pwm <= w->pwm.min) ? w->pwm.min : --pwm;
            }
            //Serial.printf("OBJSPD: %hd, PWM: %hd, PL: %d, FREQ: %huHz, E.FREQ: %hu, FREQ: %hu %hu %lf\n", w->speed.curr, pwm, pulse_length, frequency.curr, frequency.expected, frequency.min, frequency.max, frequency.step);
            analogWrite(w->pin.pwm, pwm);
        } }, "Speed Retaining", 4000, this, 1, NULL, 1);
}

Wheel::Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir) : Wheel(pin_pwm, pin_fg, pin_dir, 12, 255) {}

void Wheel::setSpeed(uint8_t speed_target)
{
    speed.curr = speed_target;
}

void Wheel::setDirection(enum direction direction)
{
    speed.direction = direction;
}

void Wheel::printConfiguration(void) {
	Serial.printf("Current pin configuration\nPWM: %hd\nFG: %hd\nDIR: %hd\n", pin.pwm, pin.fg, pin.direction);
}