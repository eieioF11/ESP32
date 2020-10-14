#ifndef ODOMETRY_H_
#define ODOMETRY_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "Kalman.h"
#include "lowpassfilter.h"

#if defined __cplusplus
extern "C" {
#endif
//---------------------User setup-------------------
#define ODOM_R 33.35f//Wheel radius [mm]
#define ODOM_L 105.0f//Distance from the center of the body to the wheel [mm]
//Mecanum Wheel only
#define ODOM_A 5.f //Body length [mm]
#define ODOM_B 5.f //Body length [mm]
/*
|<----A---->|
[/]-------[\] -
 |         |  |
 |  ROBOT  |  B
 |         |  |
[\]-------[/] -
*/
//--------------------------------------------------

/*Angle Unit*/
#define ODOM_DEG 1
#define ODOM_RAD DEG_TO_RAD
/*Distance Unit*/
#define ODOM_mm 1.f
#define ODOM_cm 0.1f
#define ODOM_m  0.001f

//Gyro sensor Z-axis error
#define GyroZrange      0.15

typedef enum
{
  NONE=0,
  TWOWHEEL,
  OMUNI2,
  OMUNI3,
  OMUNI4,
  MECANUM4
} odmmode;

class Odometry
{
  private:
    Kalman kalmanX;
    Kalman kalmanY;
    Kalman kalmanZ;

    unsigned long  nowtime,oldtime;
    float ax,ay,az;
    float gx,gy,gz;
    float mx,my,mz;
    uint8_t sensor;

    float Pitch;
    float Roll;
    float Yaw;

    LowpassFilter ARoll;
    LowpassFilter APitch;
    LowpassFilter AYaw;

    float Aroll;
    float Apitch;
    float Ayaw;

    float preval=0.0;

    float X;
    float Y;
    float wYaw;//wheel yaw
    float rx,ry,rc;//Robot coordinates rx[mm],ry[mm],rc[rad]

    float w[4];//[rad/s]
    float yawini;
    odmmode mode;
    void Accelposture();
    void Magposture();
    float dt;
  public:
    Odometry();
    //Setup
    void setup(odmmode);
    void setposture(float Pitch,float Roll,float Yaw);//[deg] Execute with update () function
    void setnineaxis(float ax,float ay,float az,float gx,float gy,float gz,float mx,float my,float mz);//ax,ay,az[G] gx,gy,gz[deg/s] mx,my,mz[mGauss] Execute with update () function
    void setsixaxis(float ax,float ay,float az,float gx,float gy,float gz);//ax,ay,az[G] gx,gy,gz[deg/s] Execute with update () function
    void setspeed(float v1=0.f,float v2=0.f,float v3=0.f,float v4=0.f);//v1~4 [rps] Execute with update () function
    void setspeed_rpm(float v1=0.f,float v2=0.f,float v3=0.f,float v4=0.f);//v1~4 [rpm] Execute with update () function
    void setangular(float w1=0.f,float w2=0.f,float w3=0.f,float w4=0.f);//w1~4 [rad/s] Execute with update () function
    //Update
    void update();//Run regularly
    //Posture
    inline float pitch(float unit=ODOM_DEG)
    {
      return Pitch*unit;
    }
    inline float roll(float unit=ODOM_DEG)
    {
      return Roll*unit;
    }
    inline float yaw(float unit=ODOM_DEG)
    {
      return Yaw*unit;
    }
    //Coordinate
    inline float x(float unit=ODOM_mm)
    {
      return X*unit;
    }
    inline float y(float unit=ODOM_mm)
    {
      return Y*unit;
    }
    inline float wyaw(float unit=ODOM_DEG)
    {
      return wYaw*unit;
    }
    //velocity
    inline float Rx(float unit=ODOM_mm)
    {
      return rx*unit;
    }
    inline float Ry(float unit=ODOM_mm)
    {
      return ry*unit;
    }
    inline float V(float unit=ODOM_m)
    {
      return sqrtf(rx*ry)*unit;
    }

    inline odmmode Mode()
    {
      return mode;
    }
    inline float DeltaT()
    {
      return dt;
    }

    void reset();
};

#if defined __cplusplus
}
#endif
#endif