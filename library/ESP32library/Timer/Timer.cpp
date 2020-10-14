#include "Timer.h"
Timer::Timer()
{
    reset();
}
void Timer::reset()
{
    countstart=false;
    nowtime=oldtime=0;
}
bool Timer::stand_by(double t)
{
    if(countstart==false){oldtime=micros();countstart=true;}
    nowtime=micros();
    //printf("%ld\n",(nowtime-oldtime));
    if((nowtime-oldtime)>(unsigned long)(t*10E5))
    {
        countstart=false;
        return true;
    }
    return false;
}