#ifndef TB6612FNG_H_
#define TB6612FNG_H_
#include <Arduino.h>
#include "Mbed.h"
#include "Setup.h"


#if defined __cplusplus
extern "C" {
#endif

class TB6612FNG:private PwmOut
{
  private:
    int pin1,pin2;
  public:
    TB6612FNG(int pin1_,int pin2_,gpio_num_t sp);
    TB6612FNG& operator = (float speed);
    void move(float speed);
    void Stop();
    void Standby(int sw);
};

#if defined __cplusplus
}
#endif

#endif