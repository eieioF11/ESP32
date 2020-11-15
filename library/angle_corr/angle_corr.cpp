#include "angle_corr.h"
Angle_correction::Angle_correction(float Kp_,float Ki_,float Kd_,float Min_,float Max_,float dt_):PID(Kp_,Ki_,Kd_,Min_,Max_,dt_)
{
  reset();
}
void Angle_correction::setup(float angle)
{
  anglemem=angle;
  reset();
}
float Angle_correction::Output(bool reset,float angle)
{
  double out=0;
  if(reset)
  {
    setup(angle);
  }
  else
  {
    out=output(anglemem,angle);
  }
  return out;
}
float Angle_correction::Output(bool reset,float angle,float dt)
{
  double out=0;
  if(reset)
  {
    setup(angle);
  }
  else
  {
    out=output(anglemem,angle,dt);
  }
  return out;
}

