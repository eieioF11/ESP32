#include "Filtered_double.h"
Filtered_double::Filtered_double(){}
Filtered_double::Filtered_double(double Slope){
    now_val = 0.0;
    slope = Slope;
}
Filtered_double& Filtered_double::operator= (double val){
    target_val = val;
    return *this;
}
void Filtered_double::SetSlope(double Slope){
    slope = Slope;
}
Filtered_double& Filtered_double ::operator<<= (double Slope){
    SetSlope(Slope);
    return *this;
}
double Filtered_double::GetNowVal(){
    if((now_val - target_val)*(now_val - target_val) < slope*slope )
        now_val = target_val;
    else{
        if(now_val < target_val)
        now_val += slope;
        else
        now_val -= slope;
    }
    return now_val;
}
Filtered_double::operator double(){
    return GetNowVal();
}


