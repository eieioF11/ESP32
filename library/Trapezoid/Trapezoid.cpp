#include "Trapezoid.h"

#define END  0
#define TURN 1
#define MOVE 2

Trapezoid::Trapezoid(Odometry *odm,float dt)
{
    this->odm=odm;
    pid=new PID(77,3,1,-1.0f,1.0f,dt);
    acorrZ=new Angle_correction(-0.0035, -0.0001, -0.001, -1.0, 1.0,dt);
    ST="STOP";
    pid->reset();
    dir=1;
}
void Trapezoid::setgain(float Kp_,float Ki_,float Kd_)
{
    pid->setgain(Kp_,Ki_,Kd_);
}
void Trapezoid::update(float *Vx,float *Vy,float *Angler)
{
    float DIF=0,acorrz_angler=0;
    if(task)
    {
        //Get current status
        nowtime=micros();
        dt=(nowtime-oldtime)*1.0E-6;//微小時間（実行間隔）[sec]
        oldtime=nowtime;
        if(task==TURN)
            now_dis=((odm->wyaw()-sAngle)*M_2_PI*ODOM_L*ODOM_m)/180.0f;
        else
            now_dis=sqrtf(squar(sY-odm->y(ODOM_m))+squar(sX-odm->x(ODOM_m)));
        float v_rps=vtorps(v);
        float a_rps=a / (2 * M_2_PI *ODOM_R);
        switch(step)
        {
            case 0://加速
                speed += a_rps;
                if (abs(speed) >= v_rps || abs(now_dis) >= d3)
                {
                    speed=v_rps;
                    step=1;
                }
                break;
            case 1://等速移動
                if (abs(now_dis) >= (2 * d3))
                {
                    step = 2;
                }
                break;
            case 2://減速
                DIF=pid->output(abs(dis),abs(now_dis),dt);
                speed = DIF * v_rps;
                break;
            default:
                step = 0;
                break;
        }
        acorrz_angler=acorrZ->Output(false,odm->yaw(),dt);
        ST="dis="+String(dis)+",now_dir="+String(now_dis)+",d3="+String(d3)+",v="+String(v)+",a="+String(a)+",v[rps]="+String(v_rps)+",a[rps]="+String(a_rps)+"speed[rps]"+String(speed)+",step"+String(step)+",pid"+String(DIF);//デバッグ用
    }
    if(abs(dis-now_dis)<=0.0001)
    {
        if(task==TURN)
            tend=true;
        else
            end=true;
        speed=0;
        acorrz_angler=0;
    }
    if(task!=TURN)
    {
        *Vx=0;
        *Vy=speed;
        *Angler=acorrz_angler;
    }
    else
    {
        *Vx=0;
        *Vy=0;
        *Angler=speed;
    }
}
void Trapezoid::reset()
{
    dir=1;
    a=0.0f;
    start=false;
    end=false;
    tstart=false;
    tend=false;
    task=END;
    step=0;
    nowtime=oldtime=0;
    ST="STOP";
}
String Trapezoid::status()
{
    return ST;
}
void Trapezoid::stop()
{
    reset();
    end=true;
}

bool Trapezoid::turn(float angle,float v,float a)
{
    if(!tstart)
    {
        tend=false;
        tstart=true;
        dis=(angle*M_2_PI*ODOM_L*ODOM_m)/180.0f;
        d3=abs(dis)/3;
        dir=dis/abs(dis);
        this->v=v*dir;
        this->a=((a!=0)?a:squar(v)/(2*d3))*dir;
        sAngle=odm->wyaw();
        setgain(77,3,0.1);
        step=0;
        speed=0;
        task=TURN;
    }
    if(tend)
    {
        dir=1;
        step=0;
        oldtime=nowtime=0;
        tstart=false;
        pid->reset();
        ST="STOP";
        task=END;
    }
    return tend;
}
bool Trapezoid::movepoint(float x,float y,float v,float a)
{
    if(!start)
    {
        if(odm->Mode()!=TWOWHEEL)
        {
            dis=sqrtf(squar(y-odm->y(ODOM_m))+squar(x-odm->x(ODOM_m)));
            sX=odm->x(ODOM_m);
            sY=odm->y(ODOM_m);
            start=true;
            task=MOVE;
        }
        else
        {
            float angle=atan2f(y-odm->y(ODOM_m),x-odm->x(ODOM_m))*RAD_TO_DEG;
            printf("%f\n\r",angle);
            if(turn(angle,v,a))
            {
                acorrZ->setup(odm->yaw());
                sX=odm->x(ODOM_m);
                sY=odm->y(ODOM_m);
                dis=sqrtf(squar(y-odm->y(ODOM_m))+squar(x-odm->x(ODOM_m)));
                d3=dis/3;
                this->v=v;
                this->a=(a!=0)?a:squar(v)/(2*d3);
                setgain(15,0.1,0.1);
                speed=0;
                step=0;
                start=true;
                task=MOVE;
            }
        }
        end=false;
    }
    if(end)
    {
        dir=1;
        x=0.0f;
        y=0.0f;
        start=false;
        pid->reset();
        ST="STOP";
        task=END;
    }
    return end;
}