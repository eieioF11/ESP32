#ifndef TOW_WHEELS_H_
#define TOW_WHEELS_H_
#include "MoveBase.h"
#include "MD.h"

#if defined __cplusplus
extern "C" {
#endif

class Two_wheels: public MoveBase
{
    public:
        Two_wheels(MD **md_);
        void Move(float V_x,float V_y,float V_angle);
        void Stop();
};

#if defined __cplusplus
}
#endif

#endif