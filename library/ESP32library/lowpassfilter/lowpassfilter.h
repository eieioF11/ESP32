#ifndef LOWPASSFILTER_H_
#define LOWPASSFILTER_H_
#include <Arduino.h>

#if defined __cplusplus
extern "C" {
#endif


class LowpassFilter
{
        private:
            float LPFval,K,KL,KH;
        public:
        LowpassFilter();
        LowpassFilter(float KH,float KL=0.f);//Kは１以下
        void begin(float KH,float KL=0.f);
        LowpassFilter& operator <= (float inival);
        LowpassFilter& operator = (float nowval);
        operator float ();
};

#if defined __cplusplus
}
#endif

#endif