#ifndef WHEEL_H
#define WHEEL_H
#include <unistd.h>

class Wheel
{
private:
    enum direction
    {
        FORWARD,
        BACKWARD
    };
    struct
    {
        uint8_t pwm;
        uint8_t fg;
        uint8_t direction;
    } pin;
    struct
    {
        uint8_t curr;
        uint8_t min;
        uint8_t max;
        enum direction direction;
    } speed;
    uint32_t expected_pulse_length;

public:

    Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir, uint8_t speed_min, uint8_t speed_max);
    Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir);

    void setSpeed(uint8_t speed_target);
    void setDirection(enum direction direction);
    void printSpeed(void);
    void printConfiguration(void);
};

#endif