#include <Arduino.h>
#include "ROBO_MD.h"

roboMD::roboMD(int num){
    mdNum = num;
}

void roboMD::Move(double val){   
    if(val>(double)1)val=1.0;
    if(val<(double)-1)val=-1.0;
    union{
        //for transmit
        uint8_t bit8[2];
        //instruction
        struct { 
            uint8_t padding : 3;
            uint8_t mdNum : 2;
            uint8_t ph : 1;
            uint16_t pwmDuty :10;
        }__packed ir;
    }data;
    data.ir.padding  = 0x00;
    data.ir.pwmDuty  = (uint16_t)(abs(val)*PIC_PWM_RESOLUTION);
    
    if(val >= 0.0)
        data.ir.ph = 1;
    else
        data.ir.ph = 0;

    data.ir.mdNum = this->mdNum;

    //printf("duty:%d %x %x", data.ir.pwmDuty, data.bit8[0], data.bit8[1]);
    
    //transmit
    Wire.beginTransmission(PIC_ADDRESS);
    Wire.write(data.bit8[0]);
    Wire.write(data.bit8[1]);
    Wire.endTransmission();
    
}
