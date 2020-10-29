#ifndef TASKRUNNER_H_
#define TASKRUNNER_H_
#include "Setup.h"
#include <Arduino.h>


#if defined __cplusplus
extern "C" {
#endif

typedef struct Flag
{
    bool Debug;
    bool Start;
    bool SerialMonitor;
    bool Melody;
    int SerialData;
    int UDPData;
}Flag_t;

class taskrun
{
	private:
        int taskstatus,Number_of_tasks;
        bool tauto;
        const char *taskname[20];
        void (*taskp[20])(Flag_t *);
        void Setup();
	public:
        Flag_t flag;
        taskrun();
        void setfunction(const char*name,void (*tfp)(Flag_t *),int tasknum);//tasknum 1~
        void setfunction(const char*name,void (*tfp)(Flag_t *));
        void select(int *statusnum);
        const char *status();
};

#if defined __cplusplus
}
#endif
#endif