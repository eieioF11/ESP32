#include "taskrunner.h"
taskrun::taskrun(){Setup();tauto=true;}
void taskrun::Setup()
{
    taskstatus=0;
    Number_of_tasks=1;
    tauto=false;
}
void taskrun::setfunction(const char*name,void (*tfp)(Flag_t *),int tasknum)
{
    taskname[tasknum-1]=name;
    taskp[tasknum-1]=tfp;
}
void taskrun::setfunction(const char*name,void (*tfp)(Flag_t *))
{
    setfunction(name,tfp,Number_of_tasks);
    Number_of_tasks++;
}
void taskrun::select(int *statusnum)
{
    if(*statusnum>=Number_of_tasks-1)*statusnum=0;
    taskstatus=*statusnum;
    if(Number_of_tasks-1>*statusnum)
    {
        (*taskp[taskstatus])(&flag);
    }
}
const char* taskrun::status()
{
    return taskname[taskstatus];
}