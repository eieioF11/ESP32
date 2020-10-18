#include "ESPMotor.h"

ESPMotor::ESPMotor(int pin1_,int pin2_,gpio_num_t sp,int A,int B,int resolution_,pcnt_unit_t pcnt_,float dt)
{
  t=new TB6612FNG(pin1_,pin2_,sp);
  e=new Encoder(A,B,resolution_,pcnt_);
  speed=0;
  Kp=0.04;
  Ki=0.0002;
  Kd=0.0001;
  pidmd=new PID(Kp,Ki,Kd,-1.0,1.0,dt);
  t->Standby(ON);
  e->Reverse(true);
  mode=PID_;
};
ESPMotor::ESPMotor(int pin1_,int pin2_,gpio_num_t sp)
{
  t=new TB6612FNG(pin1_,pin2_,sp);
  mode=DUTY;
};
void ESPMotor::set()
{
  if(mode==PID_)
    pidmd->setgain(Kp,Ki,Kd);
  t->Stop();
  speed=0;
}
void ESPMotor::Move(float val)
{
  if(mode==DUTY)
  {
    if(reverse)val*=-1;
    t->move(val);
    return;
  }
  t->Standby(ON);
  if(reverse)val*=-1;
  if((val>0&&speed<0)||(val<0&&speed>0)){speed=0;pidmd->reset();}
  speed+=pidmd->output(val,e->get_rps());
  if(speed>1.0)speed=1.0;if(speed<-1.0)speed=-1.0;
  t->move(speed);
}
void ESPMotor::Update()
{
  if(mode==DUTY)
    return;
  e->Update();
  now_val=e->get_rps();
}
void ESPMotor::Reset()
{
  if(mode==DUTY)
    return;
  speed=0.0f;
  pidmd->reset();
  e->reset();
  now_val=0.0f;
}
void ESPMotor::Brake()
{
  t->Standby(ON);
  t->Stop();
  Reset();
}