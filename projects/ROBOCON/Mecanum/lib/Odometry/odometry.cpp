#include "odometry.h"
#define nineaxis 0
#define sixaxis  1
#define other    3

Odometry odm;

Odometry::Odometry()
{
    reset();
}
void Odometry::setup(odmmode mode)
{
    this->mode=mode;
}
void Odometry::setposture(float Pitch,float Roll,float Yaw)
{
    this->Pitch=Pitch;
    this->Roll=Roll;
    this->Yaw=Yaw;
    sensor=other;
}
void Odometry::setnineaxis(float ax,float ay,float az,float gx,float gy,float gz,float mx,float my,float mz)
{
    this->ax=ax;
    this->ay=ay;
    this->az=az;
    this->gx=gx;
    this->gy=gy;
    this->gz=gz;
    this->mx=mx;
    this->my=my;
    this->mz=mz;
    sensor=nineaxis;
}
void Odometry::setsixaxis(float ax,float ay,float az,float gx,float gy,float gz)
{
    this->ax=ax;
    this->ay=ay;
    this->az=az;
    this->gx=gx;
    this->gy=gy;
    this->gz=gz;
    sensor=sixaxis;
}
void Odometry::setspeed(float v1,float v2,float v3,float v4)
{
    w[0]=2*PI*v1;
    w[1]=2*PI*v2;
    w[2]=2*PI*v3;
    w[3]=2*PI*v4;
}
void Odometry::setangular(float w1,float w2,float w3,float w4)
{
    w[0]=w1;
    w[1]=w2;
    w[2]=w3;
    w[3]=w4;    
}
void Odometry::update()
{
    //Posture calculation
    nowtime=micros();
    //Update time calculation
    float dt=(nowtime-oldtime)*1.0E-6;
    //9-axis sensor
    if(sensor==nineaxis)
    {
        MahonyQuaternionUpdate(ax,ay,az,gx*DEG_TO_RAD,gy*DEG_TO_RAD,gz*DEG_TO_RAD,mx,my,mz,dt);
        //MadgwickQuaternionUpdate(ax,ay,az,gx*DEG_TO_RAD,gy*DEG_TO_RAD,gz*DEG_TO_RAD,mx,my,mz,dt);
        //Convert quaternions to Euler angles
        Yaw   = atan2f(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() * *(getQ() + 3)), *getQ() * *getQ() + * (getQ() + 1) * *(getQ() + 1) - * (getQ() + 2) * *(getQ() + 2) - * (getQ() + 3) * *(getQ() + 3));
        Pitch = -asinf(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() * *(getQ() + 2)));
        Roll  = atan2f(2.0f * (*getQ() * *(getQ() + 1) + * (getQ() + 2) * *(getQ() + 3)), *getQ() * *getQ() - * (getQ() + 1) * *(getQ() + 1)  - * (getQ() + 2) * *(getQ() + 2) + * (getQ() + 3) * *(getQ() + 3));
        //Convert from radians to degrees
        Pitch *= RAD_TO_DEG;
        Yaw   *= RAD_TO_DEG;
        // Declination of SparkFun Electronics (40°05'26.6"N 105°11'05.9"W) is
        //  8° 30' E  ± 0° 21' (or 8.5°) on 2016-07-19
        // - http://www.ngdc.noaa.gov/geomag-web/#declination
        //Yaw   -= 8.5f;
        Yaw   -= 7.f;
        Roll  *= RAD_TO_DEG;
    }
    else if(sensor==sixaxis)
    {
        MadgwickQuaternionUpdate(ax,ay,az,gx*DEG_TO_RAD,gy*DEG_TO_RAD,gz*DEG_TO_RAD,dt);
        //Convert quaternions to Euler angles
        Yaw   = atan2f(2.0f * (*(getQ() + 1) * *(getQ() + 2) + *getQ() * *(getQ() + 3)), *getQ() * *getQ() + * (getQ() + 1) * *(getQ() + 1) - * (getQ() + 2) * *(getQ() + 2) - * (getQ() + 3) * *(getQ() + 3));
        Pitch = -asinf(2.0f * (*(getQ() + 1) * *(getQ() + 3) - *getQ() * *(getQ() + 2)));
        Roll  = atan2f(2.0f * (*getQ() * *(getQ() + 1) + * (getQ() + 2) * *(getQ() + 3)), *getQ() * *getQ() - * (getQ() + 1) * *(getQ() + 1)  - * (getQ() + 2) * *(getQ() + 2) + * (getQ() + 3) * *(getQ() + 3));
        //Convert from radians to degrees
        Pitch *= RAD_TO_DEG;
        Yaw   *= RAD_TO_DEG;
        Roll  *= RAD_TO_DEG;
    }
    else;
    oldtime=nowtime;

    //Coordinate calculation
    float rx=0.f,ry=0.f,rc=0.f;//Robot coordinates rx[mm],ry[mm],rc[rad]
    switch(mode)
    {
        case NONE:
            rx=0.f,ry=0.f,rc=0.f;
            break;
        case TWOWHEEL:
            X+=R*cosf(Yaw*DEG_TO_RAD)*(w[0]+w[1])/2.f;
            Y+=R*sinf(Yaw*DEG_TO_RAD)*(w[0]+w[1])/2.f;
            wYaw=Yaw;
            break;
        case OMUNI2:
            rx=R*w[0];
            ry=R*w[1];
            wYaw=Yaw;
            rc=Yaw*DEG_TO_RAD;
            break;
        case OMUNI3:
            rx=R*(w[0]*cosf(5*PI/12)+w[1]*cosf(PI/4)-w[2]*cosf(PI/12));
            ry=R*(-w[0]*sinf(5*PI/12)+w[1]*sinf(PI/4)+w[2]*sinf(PI/12));;
            rc+=(-R/(3.f*L))*(w[0]+w[1]+w[2]);
            break;   
        case OMUNI4:
            rx=R*(w[3]-w[1]);
            ry=R*(w[0]-w[2]);
            rc+=(R/(4.f*L))*(w[0]+w[1]+w[2]+w[3]);
            break;  
        case MECANUM4:
            float a=A/2.f;
            float b=B/2.f;
            rx=(R/4.f)*(w[0]+w[1]+w[2]+w[3]);
            ry=(R/4.f)*(w[0]-w[1]-w[2]+w[3]);
            rc+=(R/(4.f*(b+a)))*(w[1]-w[0]-w[2]+w[3]);
            break; 
    }
    if(mode!=TWOWHEEL&&mode!=NONE)
    {
        //Convert to field coordinates
        float sinc=sinf(rc);
        float cosc=cosf(rc);
        X+=rx*cosc-ry*sinc;
        Y+=ry*cosc+rx*sinc;
        wYaw=rc*RAD_TO_DEG;
    }
}
float Odometry::pitch(float unit)
{
    return Pitch*unit;
}
float Odometry::roll(float unit)
{
    return Roll*unit;
}
float Odometry::yaw(float unit)
{
    return Yaw*unit;
}
float Odometry::x(float unit)
{
    return X*unit;
}
float Odometry::y(float unit)
{
    return Y*unit;
}
float Odometry::wyaw(float unit)
{
    return wYaw*unit;
}
void Odometry::reset()
{
    Pitch=0.f;
    Roll=0.f;
    Yaw=0.f;
    nowtime=0ul;
    oldtime=0ul;
    yawini=0.f;
    w[0]=0.f;
    w[1]=0.f;
    w[2]=0.f;
    w[3]=0.f; 
}