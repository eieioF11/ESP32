#include "MoveBase.h"

MoveBase::MoveBase(){}

MoveBase::MoveBase(MD **md_){
    md = md_;
}

MoveBase::~MoveBase(){
    delete[] md;
}

void MoveBase::Move(float V_x, float V_y, float V_angular){}

void MoveBase::Stop(){
    Move(0.0, 0.0, 0.0);
}

void MoveBase::MoveXY(float V_x, float V_y){
    Move(V_x, V_y, 0);
}

void MoveBase::MoveR(float V_r, float angle, float V_angular){
    float V_x, V_y;
    V_x = V_r * cos(angle);
    V_y = V_r * sin(angle);
    Move(V_x, V_y, V_angular);
}

void MoveBase::MoveR(float V_r, float angle){
    MoveR(V_r, angle, 0);
}

void MoveBase::Rotate(float V_angular){
    Move(0, 0, V_angular);
}