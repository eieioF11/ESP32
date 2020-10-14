#include "PWM.h"

int using_ch = 0;
PwmOut::PwmOut(gpio_num_t gpio)
{
    begin(gpio);
}
void PwmOut::begin(gpio_num_t gpio)
{
    pwm_conf.gpio_num   = gpio;
    if(using_ch > 7)
    pwm_conf.speed_mode = LEDC_HIGH_SPEED_MODE;
    else 
    pwm_conf.speed_mode = LEDC_LOW_SPEED_MODE;
    pwm_conf.channel    = pwm_ch;
    pwm_conf.intr_type  = LEDC_INTR_DISABLE;
    pwm_conf.timer_sel  = LEDC_TIMER_0;
    pwm_conf.duty       = 0;
    using_ch +=1;
    ledc_channel_config(&pwm_conf);

    timer_conf.speed_mode       = LEDC_LOW_SPEED_MODE;
    timer_conf.bit_num           = LEDC_TIMER_10_BIT;
    timer_conf.timer_num        = LEDC_TIMER_0;
    timer_conf.freq_hz          = 5000;
    ledc_timer_config(&timer_conf);
}
void PwmOut::output(float duty)
{
    Duty = duty;
    uint32_t width = 1023*duty;
    ledc_set_duty(LEDC_LOW_SPEED_MODE,pwm_ch,width);
    ledc_update_duty(LEDC_LOW_SPEED_MODE,pwm_ch);
}

PwmOut& PwmOut::operator = (float duty)
{
    output(duty);
    return *this;
}

PwmOut::operator float (){
    return Duty;
}
