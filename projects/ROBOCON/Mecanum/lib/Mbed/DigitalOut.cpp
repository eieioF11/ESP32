#include "DigitalOut.h"

DigitalOut::DigitalOut()
{
}

DigitalOut::DigitalOut(gpio_num_t gpio_num){
    init(gpio_num);
}

void DigitalOut::init(gpio_num_t gpio_num){
    pinMode(gpio_num , OUTPUT);
    gpio = gpio_num;
}

DigitalOut& DigitalOut::operator = (uint32_t level){
    if(level == 0)
    {
        gpio_level = 0;
        digitalWrite(gpio, LOW);
    }
    else
    {
        gpio_level = 1;
        digitalWrite(gpio , HIGH);
    }
    return *this;
}

DigitalOut::operator int (){
    return gpio_level;
}