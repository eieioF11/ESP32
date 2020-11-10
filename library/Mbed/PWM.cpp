#include "PWM.h"

int using_ch = 0;//Robocon 4
PwmOut::PwmOut(gpio_num_t gpio)
{
    begin(gpio);
}
void PwmOut::begin(gpio_num_t gpio)
{
    // １：ledcSetup(チャンネル, 周波数, PWMの範囲);
    ledcSetup(pwm_ch,5000, 10);
    using_ch +=1;
    // ２：ledcAttachPin(ピン番号, チャンネル);
    ledcAttachPin(gpio, pwm_ch);
}
void PwmOut::output(float duty)
{
    Duty = duty;
    uint32_t width = (uint32_t)(1023*duty);
    // ３：ledcWrite(チャンネル, PWM値);
    ledcWrite(pwm_ch, width);
}

PwmOut& PwmOut::operator = (float duty)
{
    output(duty);
    return *this;
}

PwmOut::operator float (){
    return Duty;
}
