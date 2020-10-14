#ifndef __MOVEBASE_H__
#define __MOVEBASE_H__
#include "MD.h"
#include <math.h>

class MoveBase{
    public:    
    MD **md;
    MoveBase();
    MoveBase(MD **md_);
    ~MoveBase();
     
    virtual void Move(float V_x, float V_y, float V_angular);
    virtual void Stop();

    void MoveXY(float V_x, float V_y);
    void MoveR(float V_r, float angle);
    void MoveR(float V_r, float angle, float V_angular);
    void Rotate(float V_anglular);

    protected:
   
};

#endif