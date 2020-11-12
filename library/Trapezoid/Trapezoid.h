#ifndef TRAPEZOID_H_
#define TRAPEZOID_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "odometry.h"
#include "MD.h"
#include "MoveBase.h"
#include "PID.h"
#include "Setup.h"

#if defined __cplusplus
extern "C" {
#endif

class Trapezoid
{
    private:
        Odometry *odm;
        PID *pid;

        float VX,VY,Angler;
        float sX,sY,sAngle;//初期位置
        float dis,now_dis;
        float dir;

        bool start,tstart;
        bool end,tend;
        uint8_t task;

        float dt;
        float T1,T2,Tnow;//加減速時間、等速移動時間、現在の時間
        float a,v,d3;
        uint8_t step;
        unsigned long  nowtime,oldtime,oldtime2;
        inline float squar(float A)//a^2
        {
            return A*A;
        }
        inline float vtorps(float V)//速度[m/s]を[rps]に変換
        {
            return V/(2*PI*ODOM_R*ODOM_m);
        }
        String ST;
    public:
        Trapezoid(Odometry *odm,float dt_=Delta_T);
        String status();
        void update(float *Vx,float *Vy,float *Angler);
        void reset();
        void stop();
        void setgain(float Kp_,float Ki_,float Kd_);
        bool turn(float angle,float a);//angle[deg] a[m/s^2]
        bool movepoint(float x,float y,float v,float a);//x,y[m] a[m/s^2]
};

#if defined __cplusplus
}
#endif
#endif