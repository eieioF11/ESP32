#ifndef TIMER_H_
#define TIMER_H_
#include <stdio.h>
#include "Arduino.h"

#if defined __cplusplus
extern "C" {
#endif

class Timer
{
    private:
        unsigned long nowtime,oldtime;
        bool countstart;
    public:
        Timer();
        void reset();
        bool stand_by(double t);//t[s] t秒以上になったらtrueを返す
};

#if defined __cplusplus
}
#endif
#endif