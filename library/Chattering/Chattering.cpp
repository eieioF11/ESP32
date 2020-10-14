#include "Chattering.h"

Chattering::Chattering(int min,int max,int min2,int max2)
{
    count=0;
    this->min=min;
    this->max=max;
    this->min2=min2;
    this->max2=max2;
}

Chattering& Chattering::operator <= (int val)
{
    this->val=val;
    return *this;
}

Chattering::operator int ()
{
    if(val)
    {
        count++;
        if(max2!=0&&max2!=0&&min2<count&&count<max2)
        {
            count=0;
            return 3;
        }
    }
    else
    {
        if(count<min)
        {
            count=0;
            return 0;
        }
        else if(min<count&&count<max)
        {
            count=0;
            return 1;
        }
        else
        {
            count=0;
            return 2;
        }
    }
    return 0;
}