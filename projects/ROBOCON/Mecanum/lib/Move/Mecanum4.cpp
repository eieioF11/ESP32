#include <Mecanum4.h>

Mecanum4::Mecanum4(MD **md_):MoveBase(md_){}

void Mecanum4::Move(double V_x, double V_y, double V_angle){
    md[0]->Move(-V_x+V_y+V_angle);
    md[1]->Move(-V_x-V_y+V_angle);
    md[2]->Move(V_x-V_y+V_angle);
    md[3]->Move(-V_x-V_y-V_angle);
}
/*  md[0]->Move(-V_x-V_y+V_angle);
    md[1]->Move(V_x-V_y+V_angle);
    md[2]->Move(-V_x+V_y+V_angle);
    md[3]->Move(V_x+V_y+V_angle);
*/