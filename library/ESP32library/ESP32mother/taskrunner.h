#ifndef TASKRUNNER_H_
#define TASKRUNNER_H_
#include "Setup.h"
#include <Arduino.h>


#if defined __cplusplus
extern "C" {
#endif

class taskrun
{
	private:
                int taskstatus,Number_of_tasks;
                bool tauto;
                const char *taskname[20];
                void (*taskp[20])(void);
                void Setup();
	public:
                taskrun();      
                void setfunction(const char*name,void (*tfp)(void),int tasknum);//tasknum 1~
                void setfunction(const char*name,void (*tfp)(void));
                void select(int *statusnum);
                const char *status();
};

#if defined __cplusplus
}
#endif
#endif