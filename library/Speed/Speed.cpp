#include "Speed.h"

Speed::Speed(uint8_t mode,float R,float lim)
{
    this->mode=mode;
    this->rps=0.f;
    this->R=R;
    this->lim=lim;
}

/*
w[rad/s]=2*Pi*n[rps]
n[rps]=w[rad/s]/2*Pi
V[m/s]=R[m]*w[rad/s]
w[rad/s]=V[m/s]/R[m]
*/
Speed& Speed::operator = (float val)
{
    switch(mode)
    {
        case Speed_V:this->rps=val / (2 * PI * R );break;
        case Speed_W:this->rps=val / 2 * PI;break;
    }
    return *this;
}

Speed::operator float ()
{
    if(fabs(this->rps)<lim)
        return this->rps;
    else
        return lim * (this->rps/fabs(this->rps));
}