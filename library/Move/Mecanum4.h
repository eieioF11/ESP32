#ifndef __MECANUM4_H__
#define __MECANUM4_H__

#include "MoveBase.h"
#include "MD.h"

class Mecanum4: public MoveBase{
    public:
    Mecanum4(MD **md_);
    void Move(float V_x, float V_y, float V_angle);
};
/*---------------------
 md[1] |/|-|\| md[0]
          |
 md[2] |\|-|/| md[3]

 y
 |  
_|____x
o|   
----------------------*/
#endif