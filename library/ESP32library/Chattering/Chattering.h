#ifndef CHATTERING_H_
#define CHATTERING_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"

#if defined __cplusplus
extern "C" {
#endif

class Chattering
{
    private:
        int val;
        int count;
        int max,min,min2,max2;
    public:
        Chattering(int min,int max,int min2=0,int max2=0);
        Chattering& operator <= (int val);
        operator int ();
};

#if defined __cplusplus
}
#endif
#endif