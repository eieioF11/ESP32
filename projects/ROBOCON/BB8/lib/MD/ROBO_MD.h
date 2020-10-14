#ifndef __ROBO_MD_H__
#define __ROBO_MD_H__

#include "MD.h"
#include <Arduino.h>
#include <Wire.h>

#define PIC_ADDRESS 0x04
#define PIC_PWM_RESOLUTION 1023.0

class roboMD : public MD{
    public:
    //num : MD's number
    roboMD(int num);
    //val : -1.0 ~ +1.0
    void Move(double val);

    private:
    int mdNum;
};

extern MD *m[4];

void ROBO_MD_Init();

/*
1:
    |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
    |                        pwmDuty(MSbs)                          |
2:
    |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0   |
    | pwmDuty(LSbs) |  dir  |     mdNum     |   -   |   -   |   -   |

*/
#endif