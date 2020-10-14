#include <Omuni4.h>

Omuni4::Omuni4(MD **md_):MoveBase(md_){}

void Omuni4::Move(float V_x, float V_y, float V_angle){
    md[0]->Move(V_y/2+V_angle);
    md[1]->Move(-V_x/2+V_angle);
    md[2]->Move(-V_y/2+V_angle);
    md[3]->Move(V_x/2+V_angle);
}
