#include "Steer4.h"

Steer4::Steer4(MD **md_):MoveBase(md_){}

void Steer4::Move(float V_x, float V_y, float V_angular){
    V_angular *= M_SQRT1_2;
    float VY,VX;
    for(int i = 0; i < 4; i++){
        VY = V_y + V_angular*sign[i];
        VX = V_x + V_angular*sign[i+1];
        md[i]->Move(atan(VY / VX));
        md[i + 4]->Move(sqrt(pow2(VY) + pow2(VX))*((VX>0.0) ? 1.0:-1.0));
    }

    /*
    //angle
    md[0]->Move(atan((V_y + V_angular) / (V_x - V_angular)));
    md[1]->Move(atan((V_y - V_angular) / (V_x - V_angular)));
    md[2]->Move(atan((V_y - V_angular) / (V_x + V_angular)));
    md[3]->Move(atan((V_y + V_angular) / (V_x + V_angular)));
    //velocity
    md[4]->Move(sqrt(pow(V_y + V_angular, 2) + pow(V_x - V_angular, 2)));
    md[5]->Move(sqrt(pow(V_y - V_angular, 2) + pow(V_x - V_angular, 2)));
    md[6]->Move(sqrt(pow(V_y - V_angular, 2) + pow(V_x + V_angular, 2)));
    md[7]->Move(sqrt(pow(V_y + V_angular, 2) + pow(V_x + V_angular, 2)));
    */
}