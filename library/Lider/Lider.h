#ifndef LIDER_H_
#define LIDER_H_
#include <Arduino.h>
#include <Wire.h>
#include "VL53L0X.h"

#if defined __cplusplus
extern "C" {
#endif

#define AS5600_AS5601_DEV_ADDRESS      0x36
#define AS5600_AS5601_REG_RAW_ANGLE    0x0C
#define AS5600_max 4096
#define AS5600_ini 3420

enum LiderReadmode
{
    Angle,
    Distance
};

class Lider
{
    private:
        unsigned long  nowtime,oldtime,starttime,stoptime;
        uint16_t dis;
        float deg,nowdeg,predeg;
        uint16_t readangledata();
        float readdeg();
        VL53L0X gVL530X;
    public:
        Lider();
        void setup();
        void update();
        float read(LiderReadmode mode);
};

#if defined __cplusplus
}
#endif
#endif