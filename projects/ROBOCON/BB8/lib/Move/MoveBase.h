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
     
    virtual void Move(double V_x, double V_y, double V_angular);
    virtual void Stop();

    void MoveXY(double V_x, double V_y);
    void MoveR(double V_r, double angle);
    void MoveR(double V_r, double angle, double V_angular);
    void Rotate(double V_anglular);

    protected:
   
};

#endif