#include "ServoSpeed.h"

ServoSpeed::ServoSpeed(){}
void ServoSpeed::begin(int pin,int min,int max)
{
	speed = 100;
	this->min=min;
	this->max=max;
	angle = min;
	target = min;
	step = 1;
	flag = false;
	start = true;
	s.setPeriodHertz(50);
	s.attach(pin);
}
bool ServoSpeed::set(int angle,int speed)
{
	if(this->start)
	{
		this->target=map(angle,0,MAX_ANGLE,this->min,this->max);
		int S=abs(this->angle - this->target);
		if(S!=0)
			this->step=(this->target - this->angle) / S;
		this->speed=speed;
		this->flag=false;
		this->start=false;
		this->stop=false;
	}
	if(this->flag)
	{
		this->start=true;
		this->flag=false;
		return true;
	}
	return false;
}
void ServoSpeed::setMs(int val)
{
	this->stop=true;
	s.writeMicroseconds(val);
}
float ServoSpeed::nowangle()
{
	return this->nowdeg;
}
int ServoSpeed::nowMs()
{
	return this->angle;
}
void ServoSpeed::update()
{
	nowdeg = mapfloat(angle,min,max,0,MAX_ANGLE);
	if (st.stand_by(1.f / (float)speed) && !flag)
		angle += step;
	if (abs(angle-target)<=1||angle>=max||angle<=min)
		flag = true;
	if(!stop)
		s.writeMicroseconds(angle);
}
