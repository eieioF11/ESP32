#include <Omuni3.h>

Omuni3::Omuni3(MD **md_):MoveBase(md_){}

void Omuni3::Move(float V_x, float V_y, float V_angle){
    md[0]->Move(V_x + V_angle);
    md[1]->Move(-0.5*V_x - 0.866*V_y + V_angle);
    md[2]->Move(-0.5*V_x + 0.866*V_y + V_angle);
}
