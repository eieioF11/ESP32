#include "Trapezoid.h"

#define END  0
#define TURN 1
#define MOVE 2

Trapezoid::Trapezoid(Odometry *odm,float dt_)
{
    this->odm=odm;
    pid=new PID(77,3,1,-1.0f,1.0f,dt_);
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
    float DIF=0;
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
        ST="dis="+String(dis)+",now_dir="+String(now_dis)+",d3="+String(d3)+",v="+String(v)+",a="+String(a)+",v[rps]="+String(v_rps)+",a[rps]="+String(a_rps)+"speed[rps]"+String(speed)+",step"+String(step)+",pid"+String(DIF);//デバッグ用
    }
    if(abs(dis-now_dis)<=0.0001)
    {
        if(task==TURN)
            tend=true;
        else
            end=true;
        speed=0;
    }
    if(task!=TURN)
    {
        *Vx=0;
        *Vy=speed;
        *Angler=0;
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
        setgain(77,3,1);
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
                sX=odm->x(ODOM_m);
                sY=odm->y(ODOM_m);
                dis=sqrtf(squar(y-odm->y(ODOM_m))+squar(x-odm->x(ODOM_m)));
                d3=dis/3;
                this->v=v;
                this->a=(a!=0)?a:squar(v)/(2*d3);
                setgain(15,0.1,1);
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

/*
#define END  0
#define TURN 1
#define MOVE 2

Trapezoid::Trapezoid(Odometry *odm,float dt_)
{
    this->odm=odm;
    pid=new PID(0.5,0.12,0.00001,-10.0f,10.0f,dt_);
    ST="STOP";
    pid->reset();
}
void Trapezoid::setgain(float Kp_,float Ki_,float Kd_)
{
    pid->setgain(Kp_,Ki_,Kd_);
}
void Trapezoid::update(float *Vx,float *Vy,float *Angler_)
{
    if(task)
    {
        //Posture calculation
        nowtime=micros();
        //Get current status
        Tnow=(nowtime-oldtime)*1.0E-6;//各stepの開始からの時間[sec]
        dt=(nowtime-oldtime2)*1.0E-6;//微小時間（実行間隔）[sec]
        if(task==TURN)
            now_dir=((odm->wyaw()-sAngle)*PI*ODOM_L*ODOM_m)/180.0f;
        else
            now_dir=sqrtf(squar(odm->y(ODOM_m)-sY)+squar(odm->x(ODOM_m)-sX));
        switch(step)
        {
            case 0://加速
                v+=a*dt;
                if(v>Vm)v=Vm;
                if(task==TURN)
                    Angler=vtorps(v)*(dir/abs(dir));
                else
                {
                    VX=VY=vtorps(v);
                }
                if(Tnow>T1)
                {
                    step=1;
                    oldtime=nowtime;
                }
                break;
            case 1://等速移動
                if(Tnow>T2)
                {
                    step=2;
                    oldtime=nowtime;
                }
                break;
            case 2://減速
                pid->setoutput(0,Vm);
                v=pid->output(abs(dir),abs(now_dir));
                //v-=a*dt;
                if(task==TURN)
                    Angler=vtorps(v)*(dir/abs(dir));
                else
                {
                    VX=VY=vtorps(v);
                }
                if(Tnow>T1||Angler<0)
                {
                    if(task==TURN)
                        tend=true;
                    else
                        end=true;
                    task=END;
                }
                break;
        }
        oldtime2=nowtime;
        ST="T1="+String(T1)+",T2="+String(T2)+",nowT="+String(Tnow)+",dir="+String(dir)+",now_dir="+String(now_dir)+",d3="+String(d3)+",v="+String(v)+",step"+String(step)+",flip"+String(dir/abs(dir));//デバッグ用
    }
    *Vx=VX;
    *Vy=VY;
    *Angler_=Angler;
}
void Trapezoid::reset()
{
    VX=0.0f;
    VY=0.0f;
    Angler=0.0;
    dir=0.0f;
    a=0.0f;
    start=false;
    end=false;
    tstart=false;
    tend=false;
    task=END;
    step=0;
    nowtime=oldtime=oldtime2=0;
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
bool Trapezoid::turn(float angle,float a)
{
    if(!tstart)
    {
        tend=false;
        tstart=true;
        dir=(angle*PI*ODOM_L*ODOM_m)/180.0f;
        d3=dir/3;
        T1=sqrtf(2*abs(d3)/a);
        Vm=a*T1;
        T2=abs(d3)/Vm;
        this->a=a;
        sAngle=odm->wyaw();
        VX=VY=0.0f;
        step=0;
        v=0.0f;
        oldtime=micros();
        task=TURN;
    }
    if(tend)
    {
        step=0;
        oldtime2=oldtime=nowtime=0;
        T1=T2=d3=dir=Angler=0.0f;
        tstart=false;  
        pid->reset();
        ST="STOP";     
        task=END; 
    }
    return tend;
}
bool Trapezoid::movepoint(float x,float y,float a)
{
    if(!start)
    {
        if(odm->Mode()!=TWOWHEEL)
        {
            dir=sqrtf(squar(y-odm->y(ODOM_m))+squar(x-odm->x(ODOM_m)));
            sX=odm->x(ODOM_m);
            sY=odm->y(ODOM_m);
            start=true;
            task=MOVE;
        }
        else
        {
            float angle=atan2f(y-odm->y(ODOM_m),x-odm->x(ODOM_m))*RAD_TO_DEG;
            //printf("%f\n\r",angle);
            if(turn(angle,a))
            {
                sX=odm->x(ODOM_m);
                sY=odm->y(ODOM_m);
                dir=sqrtf(squar(y-odm->y(ODOM_m))+squar(x-odm->x(ODOM_m)));
                d3=dir/3;
                T1=sqrtf(2*abs(d3)/a);
                Vm=a*T1;
                T2=abs(d3)/Vm;
                this->a=a;
                VX=VY=0.0f;
                step=0;
                oldtime=micros();
                start=true;
                task=MOVE;
            }
        }
        end=false;
    }
    if(end)
    {
        x=0.0f;
        y=0.0f;
        dir=0.0f;
        start=false;   
        pid->reset();
        ST="STOP"; 
        task=END;    
    }
    return end;
}
*/