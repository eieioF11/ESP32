#include "Two_wheels.h"

Two_wheels::Two_wheels(MD **md_):MoveBase(md_){}

void Two_wheels::Move(float V_x,float V_y,float V_angle)
{
  md[0]->Move(V_x-V_angle);
  md[1]->Move(V_x+V_angle);
}

void Two_wheels::Stop()
{
  md[0]->Brake();
  md[1]->Brake();
}