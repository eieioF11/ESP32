#include "Autoprocessing.h"

Autoprocessing::Autoprocessing(){Setup();tauto=true;}
Autoprocessing::Autoprocessing(int Times){Setup();times=Times;}
void Autoprocessing::Setup()
{
    taskstatus=0;
    Number_of_tasks=1;
    told=0;
    times=0;
    tauto=false;
    stop=false;
    Execution=false;
    debug_t.reset();
    ernum=0;
}
void Autoprocessing::reset()
{
    taskstatus=0;
    tauto=false;
}
void Autoprocessing::setfunction(const char*name,bool (*tfp)(void),int tasknum)
{
    taskname[tasknum-1]=name;
    told=tasknum;
    taskp[tasknum-1]=tfp;
    if(tauto)times = (tasknum>told)?tasknum:told;
} 
void Autoprocessing::setfunction(const char*name,bool (*tfp)(void))
{
    setfunction(name,tfp,Number_of_tasks);
    Number_of_tasks++;
}
void Autoprocessing::setfunction(void (*tfp)(void),float t)
{
    debugtask=tfp;
    this->t=t;
}
bool Autoprocessing::taskrun()
{
    bool endcheck=false;
    if(endcheck==false)
    {
        if(stop==false)
        {
            if((*taskp[taskstatus])()!=NULL)
                endcheck=(*taskp[taskstatus])();
            else
                enum=function_error;
        }
    }
    return endcheck;
}
bool Autoprocessing::run()
{
    bool end=false,endtask=false;
    if(Debug_function)Execution=debug_t.stand_by(t);
    if(!Execution||Debug_function==OFF)
    {
        if(taskstatus<times)endtask=taskrun();
        if(endtask)
        {
            if(taskstatus<times)taskstatus++;
            if(taskstatus==times)end=true;
        }
    }
    else 
    {
        if((*debugtask)()!=NULL)
            (*debugtask)();
        else
            enum=debugtask_error;
    }
    return end;
}
void Autoprocessing::selecttask(int statusnum){taskstatus=statusnum-1;}
int Autoprocessing::statusread()
{
    if(times==taskstatus)return 0;
    else return taskstatus+1;
}
int Autoprocessing::errornum()
{
    return ernum;
}
const char* Autoprocessing::error()
{
    const char* name=" ";
    switch(ernum)
    {
        case normal:name="NORMAL";
        case debugtask_error:"Debug Error";
        case function_error:"Function Error";
    }
    return name;
}
const char* Autoprocessing::status()
{
    const char* name=" ";
    if(times==taskstatus)name="all task end";
    else name=taskname[taskstatus];
    return name;
}