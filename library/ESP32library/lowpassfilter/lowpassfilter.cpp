#include "lowpassfilter.h"

#define JUDGVAL 0.33f

LowpassFilter::LowpassFilter()
{
    LPFval=0.f;
}

LowpassFilter::LowpassFilter(float KH,float KL)
{
    begin(KH,KL);
    LPFval=0.f;
}

void LowpassFilter::begin(float KH,float KL)
{
    this->K=this->KH=KH;
    this->KL=KL;
    LPFval=0.f;
}

LowpassFilter& LowpassFilter::operator <= (float inival)
{
    LPFval=inival;
    return *this;
}

LowpassFilter& LowpassFilter::operator = (float nowval)
{
    if(KL!=0.f)
    {
        if(abs(nowval-LPFval)>JUDGVAL)
            K=KH;
        else
            K=KL;
    }
    LPFval+=K*(nowval-LPFval);
    return *this;
}

LowpassFilter::operator float ()
{
    return LPFval;
}