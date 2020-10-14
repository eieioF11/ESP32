#include "MoveBase.h"

MoveBase::MoveBase(){}

MoveBase::MoveBase(MD **md_){
    md = md_;
}

MoveBase::~MoveBase(){
    delete[] md;
}

void MoveBase::Move(double V_x, double V_y, double V_angular){}

void MoveBase::Stop(){
    Move(0.0, 0.0, 0.0);
}

void MoveBase::MoveXY(double V_x, double V_y){
    Move(V_x, V_y, 0);
}

void MoveBase::MoveR(double V_r, double angle, double V_angular){
    double V_x, V_y;
    V_x = V_r * cos(angle);
    V_y = V_r * sin(angle);
    Move(V_x, V_y, V_angular);
}

void MoveBase::MoveR(double V_r, double angle){
    MoveR(V_r, angle, 0);
}

void MoveBase::Rotate(double V_angular){
    Move(0, 0, V_angular);
}