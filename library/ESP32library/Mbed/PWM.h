#ifndef PWM_H
#define PWM_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "driver/ledc.h"
#include "driver/spi_master.h"
#include "driver/i2c.h"



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
        ledc_channel_t pwm_ch = static_cast<ledc_channel_t> (using_ch);
        ledc_channel_config_t pwm_conf;
        ledc_timer_config_t timer_conf;
        float Duty;
};


#if defined __cplusplus
}
#endif
#endif