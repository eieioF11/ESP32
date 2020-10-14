#ifndef __OMUNI4_H__
#define __OMUNI4_H__

#include "MoveBase.h"
#include "MD.h"

class Omuni4: public MoveBase{
    public:
    Omuni4(MD **md_);
    void Move(float V_x, float V_y, float V_angle);
};
/*---------------------
        <-
        md[1]
         |        ^
|md[2]-------md[0]|
v        |       
       md[3]
         ->

 y
 |  
_|____x
o|   
----------------------*/
#endif