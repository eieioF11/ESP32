#ifndef PID_H_
#define PID_H_
#include <stdio.h>

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

    unsigned long  nowtime,oldtime;
  public:
    PID(float Kp_,float Ki_,float Kd_,float Min_,float Max_,float dt_);
    PID(float Min_,float Max_,float dt_);
    void setgain(float Kp_,float Ki_,float Kd_);
    void setoutput(float Min,float Max);
    float output(float r, float y,float dt); //r,目標値　y,出力値    出力　Min~Maxの範囲で出力
    float output(float r,float y);//r,目標値　y,出力値    出力　Min~Maxの範囲で出力
    void reset();//積分値の初期化
};

#if defined __cplusplus
}
#endif
#endif