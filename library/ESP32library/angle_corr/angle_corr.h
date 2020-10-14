#ifndef ANGLE_CORR_H_
#define ANGLE_CORR_H_
#include <Arduino.h>
#include "PID.h"
#if defined __cplusplus
extern "C" {
#endif
class Angle_correction:public PID
{
  private:
    float anglemem=0.f;
  public:
    Angle_correction(float Kp_,float Ki_,float Kd_,float Min_,float Max_,float dt_);
    void setup(float angle);
    double Output(bool reset,float angle);
};
#if defined __cplusplus
}
#endif
#endif