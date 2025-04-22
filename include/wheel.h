#ifndef WHEEL_H
#define WHEEL_H
#include <unistd.h>

class Wheel
{
public:
    enum direction
    {
        FORWARD,
        BACKWARD
    };

private:
    struct
    {
        uint8_t pwm;
        uint8_t fg;
        uint8_t direction;
    } pin;
    struct
    {
        uint8_t curr;
        enum direction direction;
    } speed;
    struct
    {
        uint8_t min;
        uint8_t max;
    } pwm;
    struct
    {
        unsigned long min;
        unsigned long max;
    } pl;

public:
    Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir, uint8_t pwm_min, uint8_t pwm_max, unsigned long pl_min, unsigned long pl_max);
    Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir, uint8_t pwm_min, uint8_t pwm_max);
    Wheel(uint8_t pin_pwm, uint8_t pin_fg, uint8_t pin_dir);

    void setSpeed(uint8_t speed_target);
    void setDirection(enum direction direction);
    void printConfiguration(void);
};

#endif