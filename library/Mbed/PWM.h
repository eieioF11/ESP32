#ifndef PWM_H
#define PWM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Arduino.h>

#if defined __cplusplus
extern "C" {
#endif

extern int using_ch;
class PwmOut{
    public:
        PwmOut(gpio_num_t gpio);
        void begin(gpio_num_t gpio);
        void output(float duty);
        PwmOut& operator = (float duty);
        operator float ();
    private:
        const int pwm_ch = using_ch;
        float Duty;
};


#if defined __cplusplus
}
#endif
#endif