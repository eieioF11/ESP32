#include "TB6612FNG.h"
TB6612FNG::TB6612FNG(int pin1_,int pin2_,gpio_num_t sp):PwmOut(sp)
{
    pin1=pin1_;
    pin2=pin2_;
    pinMode(pin1, OUTPUT);
    pinMode(pin2, OUTPUT);
};
TB6612FNG& TB6612FNG::operator = (float speed){
    move(speed);
    return *this;
}
void TB6612FNG::move(float speed)
{
    digitalWrite(STBY, HIGH);
    speed = (abs(speed)>1.00)?1.00*(speed/abs(speed)):speed;
    if(speed>0)
    {
        digitalWrite(pin1, LOW);
        digitalWrite(pin2, HIGH);
        output(speed);
    }
    else
    {
        digitalWrite(pin1, HIGH);
        digitalWrite(pin2, LOW);
        output(speed*-1);
    }
}
void TB6612FNG::Stop()
{
  digitalWrite(pin1, HIGH);
  digitalWrite(pin2, HIGH);
  output(0);
}
void TB6612FNG::Standby(int sw)
{
    if(sw==ON)digitalWrite(STBY,HIGH);
    else digitalWrite(STBY,LOW);
}