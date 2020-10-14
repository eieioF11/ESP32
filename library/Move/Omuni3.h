#ifndef __OMUNI3_H__
#define __OMUNI3_H__

#include "MoveBase.h"
#include "MD.h"

class Omuni3: public MoveBase{
    public:
    Omuni3(MD **md_);
    void Move(float V_x, float V_y, float V_angle);
};
/*---------------------
       --md[0]-->
        /   \
  ^    /     \
   \  /       \    /
   md[1]_ _ _ _md[2]
     \           /
                v
 y
 |  
_|____x
o|   
----------------------*/
#endif