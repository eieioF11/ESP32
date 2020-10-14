#ifndef PID_H_
#define PID_H_
#include <stdio.h>
#include "stprint.h"

#if defined __cplusplus
extern "C" {
#endif

class PID
{
  private:
    float Kp,Ki,Kd,Min,Max;
    float integral;
    float past,deviation;
    float dt;
  public:
    PID(float Kp_,float Ki_,float Kd_,float Min_,float Max_,float dt_);
    void setgain(float Kp_,float Ki_,float Kd_);
    float output(float Kp_,float Ki_,float Kd_,float R,float Y);
    float output(float R,float Y);
    void reset();
};

#if defined __cplusplus
}
#endif
#endif