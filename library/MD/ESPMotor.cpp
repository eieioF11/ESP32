#include "ESPMotor.h"

ESPMotor::ESPMotor(int pin1_,int pin2_,gpio_num_t sp,int A,int B,int resolution_,pcnt_unit_t pcnt_,float dt)
{
  t=new TB6612FNG(pin1_,pin2_,sp);
  e=new Encoder(A,B,resolution_,pcnt_);
  speed=0;
  Kp=0.04;//0.04
  Ki=0.06;//0.0002
  Kd=0.01;//0.0001
  pidmd=new PID(Kp,Ki,Kd,-0.01,0.01,dt);
  t->Standby(ON);
  e->Reverse(true);
  mode=PID_M;
};
ESPMotor::ESPMotor(int pin1_,int pin2_,gpio_num_t sp)
{
  t=new TB6612FNG(pin1_,pin2_,sp);
  mode=DUTY_M;
};
void ESPMotor::set()
{
  if(mode==PID_M)
    pidmd->setgain(Kp,Ki,Kd);
  t->Stop();
  speed=0;
}
void ESPMotor::Move(float val)
{
  if(mode==DUTY_M)
  {
    if(reverse)val*=-1;
    t->move(val);
    return;
  }
  t->Standby(ON);
  if(reverse)val*=-1;
  //if((val>0&&speed<0)||(val<0&&speed>0)){speed=0;pidmd->reset();}
  if(fabs(val)<0&&fabs(speed)<0.00001){speed=0;pidmd->reset();}
  speed+=pidmd->output(val,e->get_rps());
  if(speed>1.0)speed=1.0;if(speed<-1.0)speed=-1.0;
  t->move(speed);
}
void ESPMotor::Update()
{
  if(mode==DUTY_M)
    return;
  e->Update();
  now_val=e->get_rps();
}
void ESPMotor::Reset()
{
  if(mode==DUTY_M)
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