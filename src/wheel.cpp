#include <Arduino.h>
#include "wheel.h"

Wheel::Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir, uint8_t pwm_min, uint8_t pwm_max, unsigned long pl_min, unsigned long pl_max) :
                                                                                                   pin{pin_pwm, pin_fg, pin_dir},
                                                                                                   speed{0, FORWARD},
                                                                                                   pwm{pwm_min, pwm_max},
                                                                                                   pl{pl_min, pl_max}
{
    pinMode(pin.pwm, OUTPUT);
    pinMode(pin.fg, INPUT_PULLUP);
    pinMode(pin.direction, OUTPUT);
    xTaskCreatePinnedToCore([](void *param)
                            {
        Wheel *w = (Wheel *) param;
        uint8_t pwm = 0;
        struct frequency {
            const double min;
            const double max;
            const double step;
            double expected;
            double curr;
        };
        struct frequency frequency = {
            .min = 1E+6 / w->pl.max,
            .max = 1E+6 / w->pl.min,
            .step = ((1E+6 / w->pl.min) - (1E+6 / w->pl.max)) / 255.0
        };
        uint32_t pulse_length;
        direction direction = w->speed.direction;
        digitalWrite(w->pin.direction, (direction == FORWARD) ? HIGH : LOW);
        while (true) {
            pulse_length = pulseIn(w->pin.fg, LOW, w->pl.max * 2);
            frequency.curr = 1E+6 / pulse_length;
            frequency.expected = frequency.min + (frequency.step * w->speed.curr);
            // On direction changed
            if (direction != w->speed.direction) {
                direction = w->speed.direction;
                // Slow down
                while (pulseIn(w->pin.fg, LOW, w->pl.max * 2)) {
                    analogWrite(w->pin.pwm, 0);
                }
                digitalWrite(w->pin.direction, (direction == FORWARD) ? HIGH : LOW);
                pwm = w->pwm.min;
            }
            // Retaining speed and brake
            if (w->speed.curr == 0) { // Brake
                pwm = 0;
            } else if (pulse_length == 0 || frequency.curr < frequency.expected) { // Too slow
                ++pwm;
            } else if (frequency.curr > frequency.expected) { // Too fast
                --pwm;
            }
            pwm = (pwm >= w->pwm.max) ? w->pwm.max : ++pwm;
            pwm = (pwm <= w->pwm.min) ? w->pwm.min : --pwm;
#ifdef DEBUG
        Serial.printf("DIR: %s, OBJSPD: %hd, PWM: %hd, PL: %d, FREQ: %.0lf, E.FREQ: %.0lf, FREQ: %.0lf %.0lf %lf\n",
                direction == FORWARD ? "FORWARD" : "BACKWARD",
                w->speed.curr,
                pwm,
                pulse_length,
                frequency.curr,
                frequency.expected,
                frequency.min,
                frequency.max,
                frequency.step
            );
#endif
            analogWrite(w->pin.pwm, pwm);
        } }, "Speed Retaining", 4000, this, 1, NULL, 1);
}

Wheel::Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir, uint8_t pwm_min, uint8_t pwm_max) : Wheel(pin_pwm, pin_fg, pin_dir, pwm_min, pwm_max, 1400UL, 100000UL) {}
Wheel::Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir) : Wheel(pin_pwm, pin_fg, pin_dir, 12, 255) {}

void Wheel::setSpeed(uint8_t speed_target)
{
    speed.curr = speed_target;
}

void Wheel::setDirection(enum direction direction)
{
    speed.direction = direction;
}

void Wheel::printConfiguration(void)
{
    Serial.printf("Current pin configuration\nPWM: %hd\nFG: %hd\nDIR: %hd\n", pin.pwm, pin.fg, pin.direction);
}