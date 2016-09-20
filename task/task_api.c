

#include "typedef.h"
#include "error.h"
#include "task.h"


void task_delay(UINT32 tick)
{
	
	if(0 == strcmp(task_cur->task_name, "idle"))
	{
		return;
	}

	int_disable();
	
	task_cur->task_sleeptime = tick;
	task_cur->task_state = TASK_PEND;
	task_sched();
	
	int_enable();

	return;
}




