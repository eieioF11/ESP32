#include <Omuni4.h>

Omuni4::Omuni4(MD **md_):MoveBase(md_){}

void Omuni4::Move(double V_x, double V_y, double V_angle){
    md[0]->Move(V_y/2+V_angle);
    md[1]->Move(-V_x/2+V_angle);
    md[2]->Move(-V_y/2+V_angle);
    md[3]->Move(V_x/2+V_angle);
}
