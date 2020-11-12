#include "Trapezoid.h"

#define END  0
#define TURN 1
#define MOVE 2

Trapezoid::Trapezoid(Odometry *odm,float dt_)
{
    this->odm=odm;
    pid=new PID(10,0.00001,0,-1.0f,1.0f,dt_);
    ST="STOP";
    pid->reset();
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
        now_dis=sqrtf(squar(sY-odm->y(ODOM_m))+squar(sX-odm->x(ODOM_m)));
        switch(step)
        {
            case 0://加速
                VY += a / (2 * M_2_PI *ODOM_R);
                if (abs(VY) >= v || abs(now_dis) >= d3)
                {
                    VY=v;
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
                VY = DIF * v;
                break;
            default:
                step = 0;
                break;
        }

        ST="dis="+String(dis)+",now_dir="+String(now_dis)+",d3="+String(d3)+",v="+String(v)+",a="+String(a)+",step"+String(step)+",pid"+String(DIF);//デバッグ用
    }
    if(abs(dis-now_dis)<=0.0001)
    {
        end=true;
        VX=VY=this->Angler=0;
    }
    *Vx=VX;
    *Vy=VY;
    *Angler=this->Angler;
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
bool Trapezoid::turn(float angle,float a)
{
    if(!tstart)
    {
        tend=false;
        tstart=true;
        dir=(angle*M_2_PI*ODOM_L*ODOM_m)/180.0f;
        d3=dir/3;
        T1=sqrtf(2*abs(d3)/a);
        //Vm=a*T1;
        //T2=abs(d3)/Vm;
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
            //float angle=atan2f(y-odm->y(ODOM_m),x-odm->x(ODOM_m))*RAD_TO_DEG;
            //printf("%f\n\r",angle);
            //if(turn(angle,a))
            //{
                sX=odm->x(ODOM_m);
                sY=odm->y(ODOM_m);
                dis=sqrtf(squar(y-odm->y(ODOM_m))+squar(x-odm->x(ODOM_m)));
                d3=dis/3;
                this->v=vtorps(v);
                this->a=vtorps(a);
                VX=VY=0.0f;
                step=0;
                start=true;
                task=MOVE;
            //}
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