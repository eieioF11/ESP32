#ifndef TRAPEZOID_H_
#define TRAPEZOID_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "odometry.h"
#include "MD.h"
#include "MoveBase.h"
#include "angle_corr.h"
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
        Angle_correction *acorrZ;

        float speed;
        float sX,sY,sAngle;//初期位置
        float dis,now_dis;//現在位置
        float dir;//方向

        bool start,tstart;//スタートフラグ
        bool end,tend;//終了フラグ
        uint8_t task;//実行中のタスク

        float dt;
        float a,v,d3;
        uint8_t step;
        unsigned long  nowtime,oldtime;
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
        bool turn(float angle,float v,float a=0);//angle[deg] a[m/s^2]
        bool movepoint(float x,float y,float v,float a=0);//x,y[m] a[m/s^2]
};

#if defined __cplusplus
}
#endif
#endif