#include <Omuni4.h>

Omuni4::Omuni4(MD **md_):MoveBase(md_){}

void Omuni4::Move(double V_x, double V_y, double V_angle){
    md[0]->Move(double(V_y/2.0+V_angle));
    md[1]->Move(double(-V_x/2.0+V_angle));
    md[2]->Move(double(-V_y/2.0+V_angle));
    md[3]->Move(double(V_x/2.0+V_angle));
    Serial.printf("/%f,%f,%f,%f/",(V_y/2+V_angle),(-V_x/2+V_angle),(-V_y/2+V_angle),(V_x/2+V_angle));
}
