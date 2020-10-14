#ifndef ODOMETRY_H_
#define ODOMETRY_H_
#include <Arduino.h>
#include <stdio.h>
#include "math.h"
#include "quaternionFilters.h"

#if defined __cplusplus
extern "C" {
#endif
//---------------------User setup-------------------
#define R 10.f//Wheel radius [mm]
#define L 30.f//Distance from the center of the body to the wheel [mm]
//Mecanum Wheel only
#define A 5.f //Body length [mm]
#define B 5.f //Body length [mm]
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
#define DEG 1
#define RAD DEG_TO_RAD
/*Distance Unit*/
#define mm 1.f
#define cm 0.1f
#define m  0.001f

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
    unsigned long  nowtime,oldtime;
    float ax,ay,az;
    float gx,gy,gz;
    float mx,my,mz;
    uint8_t sensor;

    float Pitch;
    float Roll;
    float Yaw;

    float X;
    float Y;
    float wYaw;//wheel yaw

    float w[4];//[rad/s]
    float yawini;
    odmmode mode;
  public:
    Odometry();
    //Setup
    void setup(odmmode);
    void setposture(float Pitch,float Roll,float Yaw);//[deg] Execute with update () function
    void setnineaxis(float ax,float ay,float az,float gx,float gy,float gz,float mx,float my,float mz);//ax,ay,az[G] gx,gy,gz[deg/s] mx,my,mz[mGauss] Execute with update () function
    void setsixaxis(float ax,float ay,float az,float gx,float gy,float gz);//ax,ay,az[G] gx,gy,gz[deg/s] Execute with update () function
    void setspeed(float v1=0.f,float v2=0.f,float v3=0.f,float v4=0.f);//v1~4 [rps] Execute with update () function
    void setangular(float w1=0.f,float w2=0.f,float w3=0.f,float w4=0.f);//w1~4 [rad/s] Execute with update () function
    //Update
    void update();//Run regularly
    //Posture
    float pitch(float unit=DEG);
    float roll(float unit=DEG);
    float yaw(float unit=DEG);
    //Coordinate
    float x(float unit=mm);
    float y(float unit=mm);
    float wyaw(float unit=DEG);

    void reset();
};

extern Odometry odm;

#if defined __cplusplus
}
#endif
#endif