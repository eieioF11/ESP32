#ifndef __STEER4_H__
#define __STEER4_H__

#include "MoveBase.h"
#include "MD.h"

#define pow2(x) ((x)*(x))
const int sign[5]={1,-1,-1,1,1};

class Steer4 : public MoveBase{
    public:
    Steer4(MD **md_);
    void Move(double V_x, double V_y, double V_angular);
};

#endif