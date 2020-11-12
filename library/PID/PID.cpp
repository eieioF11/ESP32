#include"PID.h"
PID::PID(float Kp_,float Ki_,float Kd_,float Min_,float Max_,float dt_)
{
  Kp=Kp_;
  Ki=Ki_;
  Kd=Kd_;
  Min=Min_;
  Max=Max_;
  dt=dt_;
  reset();
  //printf("PID initialize!\n\rParameter(Kp=%.3f/Ki=%.3f/Kd=%.3f/Min=%.3f~Max=%.3f/Processing time = %.3f)\n\r",Kp,Ki,Kd,Min,Max,dt);
}
PID::PID(float Min_,float Max_,float dt_)
{
  Kp=Ki=Kd=0.0;
  Min=Min_;
  Max=Max_;
  dt=dt_;
  reset();
  printf("Parameter set (Min=%.3f~Max=%.3f/Processing time = %.3f)\n\r",Min,Max,dt);
}
void PID::setgain(float Kp_,float Ki_,float Kd_)
{
  Kp=Kp_;
  Ki=Ki_;
  Kd=Kd_;
  reset();
  printf("Gain set (Kp=%.5f/Ki=%.5f/Kd=%.5f)\n\r",Kp,Ki,Kd);
}
void PID::setoutput(float Min,float Max)
{
  this->Min=Min;
  this->Max=Max;
}
float PID::output(float r, float y)
{
    float pid=output(r,y,this->dt);
    return pid;
}

float PID::output(float r, float y,float dt)
{
    past = deviation;
    deviation = r - y;                            //偏差
    integral += ((past + deviation) / 2) * dt;    //積分
    float differential = (deviation - past) / dt; //微分
    float pid = Kp * deviation + Ki * integral + Kd * differential;
    if (pid > Max)
        pid = Max;
    if (pid < Min)
        pid = Min;
    return pid;
}
void PID::reset()
{
  integral=0.0;
  past = 0.0;
  deviation = 0.0;
  //printf("PID reset!\n\r");
}