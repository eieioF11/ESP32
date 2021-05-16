#include <Arduino.h>
#include "odometry.h"

#define nineaxis 0
#define sixaxis  1
#define other    3

Odometry::Odometry()
{
    reset();
    ARoll.begin(0.25);
    APitch.begin(0.25);
    //AYaw.begin(0.25);
}
void Odometry::setup(odmmode mode)
{
    reset();
    this->mode=mode;
    kalmanX.setAngle(0); // Set starting angle
    kalmanY.setAngle(0); // Set starting angle
    kalmanZ.setAngle(0); // Set starting angle
    kalmanYaw.setAngle(0);// Set starting angle
}
bool Odometry::zeroset(int n)
{
    static int i=0;
    //Update time calculation
    dt=update_time();
    //Posture calculation
    update_posture(dt);
    //Zero point setting
	ini_Pitch+=Pitch;
	ini_Roll+=Roll;
	ini_Yaw+=Yaw;
    if(i>n)
    {
        ini_Pitch/=i;
        ini_Roll/=i;
        ini_Yaw/=i;
        printf("ini(%f,%f,%f)\n\r",ini_Pitch,ini_Roll,ini_Yaw);
        i=0;
        return false;
    }
    i++;
    return true;
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
    w[0]=2.0f*PI*v1;//w[ras/s]=2*PI*n  PI=3.14... n[rps]
    w[1]=2.0f*PI*v2;
    w[2]=2.0f*PI*v3;
    w[3]=2.0f*PI*v4;
}
void Odometry::setspeed_rpm(float v1,float v2,float v3,float v4)
{
    w[0]=(2.0f*PI*v1)/60.0f;//w[ras/s]=(2*PI*N)/60  PI=3.14... N[rpm]
    w[1]=(2.0f*PI*v2)/60.0f;
    w[2]=(2.0f*PI*v3)/60.0f;
    w[3]=(2.0f*PI*v4)/60.0f;
}
void Odometry::setangular(float w1,float w2,float w3,float w4)
{
    w[0]=w1;//[rad/s]
    w[1]=w2;
    w[2]=w3;
    w[3]=w4;
}
void Odometry::Accelposture()
{
    //加速度から傾斜（roll,pitch）を算出
    Aroll=atan2f(ay,az);
    Apitch=-atan2f(ax,sqrtf(ay*ay+az*az));
}
void Odometry::Magposture()
{
    //地磁気から回転角（yaw）を算出
    float Mx=mx*cosf(Apitch)+my*sinf(Aroll)*sinf(Apitch)+mz*cosf(Aroll)*sinf(Apitch);
    float My=my*cosf(Aroll)-mz*sinf(Aroll);
    Ayaw=atan2f(My,Mx);
}
void Odometry::update_posture(float dt)
{
    if(sensor==nineaxis)//9-axis sensor
    {
        Accelposture();
        Magposture();
        //加速度と地磁気から求めた角度をローパスフィルターに通す
        ARoll  = Aroll*RAD_TO_DEG;
        APitch = Apitch*RAD_TO_DEG;
        AYaw   = Ayaw*RAD_TO_DEG;
        //カルマンフィルタでジャイロセンサーのドリフト誤差補正
        Roll  = kalmanX.getAngle((float)ARoll,gx, dt);
        Pitch = kalmanY.getAngle((float)APitch,gy, dt);
        Yaw   = kalmanZ.getAngle((float)AYaw,gz, dt);
    }
    else if(sensor==sixaxis)//6-axis sensor
    {
        static float preval = 0.0;
        Accelposture();
        //カルマンフィルタでジャイロセンサーのドリフト誤差補正
        Roll  = kalmanX.getAngle((float)ARoll,gx, dt);
        Pitch = kalmanY.getAngle((float)APitch,gy, dt);
        //Trapezoidal integral
        if(fabs(gz)<fabs(GyroZrange))
            gz=0.0;
        Yaw += (preval + gz) * dt / 2.f;
        preval = gz;
    }
}
float Odometry::update_time()
{
    nowtime=micros();
    float dt=(nowtime-oldtime)*1.0E-6;
    oldtime=nowtime;
    return dt;
}
void Odometry::update()
{
    //Update time calculation
    dt=update_time();
    //Coordinate calculation
    //Mem
    //v=r*w  v[m/s] r[m] w[rad/s]
    //|<-L->|
    //[-----] }R
    //l=L/2 l[m] angle=(r*w)/l [rad]
    switch(mode)
    {
        case NONE:
            rx=0.0f,ry=0.0f,rc=0.0f;
            break;
        case TWOWHEEL:
            rx=ODOM_R*((w[0]+w[1])/2.0f);
            ry=ODOM_R*((w[0]+w[1])/2.0f);
            W=(ODOM_R/(2.0f*ODOM_L))*(w[1]-w[0]);
            break;
        case OMUNI2:
            rx=ODOM_R*w[0];
            ry=ODOM_R*w[1];
            rc=Yaw*DEG_TO_RAD;
            break;
        case OMUNI3:
            rx=ODOM_R*(w[0]*cosf(5.0f*PI/12)+w[1]*cosf(PI/4.0f)-w[2]*cosf(PI/12.0f));
            ry=ODOM_R*(-w[0]*sinf(5.0f*PI/12)+w[1]*sinf(PI/4.0f)+w[2]*sinf(PI/12.0f));
            angular=(-ODOM_R/(3.0f*ODOM_L))*(w[0]+w[1]+w[2]);
            rc+=angular;
            break;
        case OMUNI4:
            rx=ODOM_R*(w[3]-w[1]);
            ry=ODOM_R*(w[0]-w[2]);
            angular=(ODOM_R / (4.0f * ODOM_L)) * (w[0] + w[1] + w[2] + w[3]);
            rc+=angular;
            break;
        case MECANUM4:
            float a=ODOM_A/2.0f;
            float b=ODOM_B/2.0f;

            rx=(ODOM_R/4.0f)*(w[0]+w[1]+w[2]+w[3]);
            ry=(ODOM_R/4.0f)*(w[0]-w[1]-w[2]+w[3]);
            angular=(ODOM_R/(4.0f*(b+a)))*(w[1]-w[0]-w[2]+w[3]);
            rc+=angular;
            break;
    }
    //Posture calculation
    update_posture(dt);
    //Field coordinate calculation
    if(mode!=TWOWHEEL&&mode!=NONE)
    {
        //Convert to field coordinates
        float sinc=sinf(rc*DEG_TO_RAD);
        float cosc=cosf(rc*DEG_TO_RAD);
        X+=rx*cosc-ry*sinc;
        Y+=ry*cosc+rx*sinc;
    }
    else if(mode==TWOWHEEL)
    {
        angular=W*dt*RAD_TO_DEG;
        rc+=angular;
        //rc=kalmanYaw.getAngle((float)yaw(),W*RAD_TO_DEG,dt);
        //rc=0;
        X+=rx*cosf(rc*DEG_TO_RAD)*dt;
        Y+=ry*sinf(rc*DEG_TO_RAD)*dt;
    }
    wYaw=rc;
}
void Odometry::reset()
{
    //posture
    Pitch=0.0f;
    Roll=0.0f;
    Yaw=0.0f;
    Aroll=0.f;
    Apitch=0.f;
    Ayaw=0.f;
    //coordinates
    wYaw=0.0f;
    X=0.0f;
    Y=0.0f;
    //time
    nowtime=0ul;
    oldtime=0ul;
    dt=0.0f;
    //speed
    w[0]=0.0f;
    w[1]=0.0f;
    w[2]=0.0f;
    w[3]=0.0f;
    rx=0.0f;
    ry=0.0f;
    rc=0.0f;
}