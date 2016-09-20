
#include "typedef.h"
#include "error.h"
#include "task.h"




/* sched status */
UINT32 task_sched_sign = TASK_SCHED_UNLOCK;

/* task resource */
task_struct task_init[TASK_INIT_NUM] = {0};
char task_sp[TASK_INIT_NUM][TASK_INIT_SPLENGTH] = {0};

/* sched obj */
task_struct *task_cur = NULL;
task_struct *task_head = NULL;
task_struct *task_tail = NULL;
UINT32 ticknum = 0;

extern void task_switch(void *old_task_ctx, void *new_task_ctx);

void task_start(void)
{
	strncpy(task_init[0].task_name, "idle", TASK_NAME_LEN);
	task_cur = task_tail = task_head = &task_init[0];
	task_init[0].next = task_head;
	task_init[0].task_state = TASK_READY;
	task_init[0].task_priority = 100;
	task_init[0].task_runtime = TASK_INIT_RUNTIME;
	task_init[0].task_sleeptime = 0;
}

UINT32 task_get_resource(task_struct **task_avail)
{
	UINT32 task_idx = 0;
	UINT32 task_sp_idx = 0;

	/* step1 : get task_struct */
	for (task_idx = 0; task_idx < TASK_INIT_NUM; task_idx++)
	{
		if (0 == task_init[task_idx].task_name[0])
		{
			break;
		}
	}

	if (TASK_INIT_NUM == task_idx)
	{
		*task_avail = NULL;
		return TASK_NORESOURCE;
	}
	
	*task_avail = &task_init[task_idx];

	/* step2 : get stack */
	task_sp_idx = task_idx+1;
	/*memset(task_sp[task_sp_idx], TASK_INIT_SPCT, TASK_INIT_SPLENGTH);*/
	(*task_avail)->task_ctx.sp = task_sp[task_sp_idx];

#if (CPUARCH == MIPS)
	(*task_avail)->task_ctx.sp -= 160;
#endif

	return LOS_OK;
}



UINT32 task_create(CHAR *name, TASK_FUNC task_func, UINT32 argc, VOID **argv, UINT32 priority)
{
	UINT32 task_sp_idx = 0;
	UINT32 ret = 0;
	task_struct *task_avail = NULL;

	/* step1:get task resource */
	ret = task_get_resource(&task_avail);
	if(LOS_OK != ret)
	{
		return ret;
	}

	/* step2: init task */
	int_disable();

	strncpy(task_avail->task_name, name, TASK_NAME_LEN);
	task_avail->task_sleeptime = 0;
	task_avail->task_runtime = TASK_INIT_RUNTIME;
	task_avail->task_priority = priority;
	task_avail->task_state = TASK_READY;

#if (CPUARCH == ARM)	
	task_avail->task_ctx.r0 = argc;
	task_avail->task_ctx.r1 = argv;
	
	task_avail->task_ctx.pc = task_func;
	task_avail->task_ctx.cpsr = 0x13; /*svc*/
#endif

#if (CPUARCH == MIPS)
	asm volatile
	(
		".set mips1\n\t"
		"move %0, $28\n\t"
		:"=r" (ret)
	);
	task_avail->task_ctx.a0 = argc;
	task_avail->task_ctx.a1 = argv;
	task_avail->task_ctx.gp = ret;

	task_avail->task_ctx.EPC = task_func;
	task_avail->task_ctx.ra = task_func;
	task_avail->task_ctx.pc = task_func;
	task_avail->task_ctx.Status = 0x408005;
	
#endif

	/* step3: insert sched */
	task_tail->next = task_avail;
	task_tail = task_avail;
	task_avail->next = task_head;
	
	int_enable();

	return LOS_OK;
}

/* 暂且未使用，BUGGY */
UINT32 task_delete(CHAR * name)
{
	UINT32 task_idx = 0;
	UINT32 task_sp_idx = 0;
	task_struct *task_tmp = task_head;

	/* step1 : find the task */
	do
	{
		if(0 == strcmp(task_tmp->task_name, name))
		{
			break;
		}

		task_tmp = task_tmp->next;
	}
	while(task_tmp !=  task_tail);

	/* step2 : release resource */
	int_disable();
	task_tmp->task_name[0] = 0;
	int_enable();

	return LOS_OK;	
}


UINT32 task_locksched()
{
	int_disable();
	task_sched_sign = TASK_SCHED_LOCK;
	int_enable();

	return LOS_OK;
}

UINT32 task_unlocksched()
{
	int_disable();
	task_sched_sign = TASK_SCHED_UNLOCK;
	int_enable();

	return LOS_OK;
}

BOOL task_islocksched()
{
	if (TASK_SCHED_LOCK == task_sched_sign)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

task_struct *task_getnextrun(void)
{
	task_struct *task_tmp = task_cur->next;

	if(task_tmp == task_cur)
	{
		return task_cur;
	}

	while(task_tmp != task_cur)
	{
		if(task_tmp->task_state == TASK_READY)
			break;

		task_tmp = task_tmp->next;
	}

	return task_tmp;
}

task_struct_ctx *task_nextrun(void)
{
	task_struct *task_tmp = task_getnextrun();
	task_struct_ctx *taskctx = &(task_tmp->task_ctx);
	task_cur->task_state = TASK_READY;
	task_tmp->task_state = TASK_RUN;
	task_cur = task_tmp;
	
	return taskctx;
}

BOOL task_needsched()
{
	task_struct *task_tmp = NULL;

	if(task_islocksched())
	{
		return FALSE;
	}

	task_tmp = task_getnextrun();

	if(task_tmp != task_cur)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void task_showtick()
{
	printk("tick num is %d\r\n", ticknum);
}

UINT32 task_gettick()
{
	return ticknum;
}

void task_tickdo()
{
	sem *sem_id = NULL;
	list *list = NULL;
	task_struct *task_tmp = task_cur->next;

	ticknum++;
	if(ticknum == 0xffffffff)
	{
		printk("ticknum revert\r\n");
		ticknum = 0;
	}

	while(task_tmp != task_cur)
	{
		if((task_tmp->task_state != TASK_READY)
			&&(task_tmp->task_state != TASK_RUN)
			&&(task_tmp->task_sleeptime != 0))
		{			
			task_tmp->task_sleeptime--;

			if(task_tmp->task_sleeptime == 0)
			{
				task_tmp->task_state = TASK_READY;

				if(task_tmp->sem_id != NULL)
				{
					list = &task_tmp->sem_chain;
					if(list->next != NULL)
					{
						list->prev->next = list->next;
						list->next->prev = list->prev;
					}
					else
					{
						list->prev->next = NULL;
					}
					
					task_tmp->sem_id = NULL; 
				}

				if(task_tmp->msg_id != NULL)
				{
					list = &task_tmp->msg_chain;
					if(list->next != NULL)
					{
						list->prev->next = list->next;
						list->next->prev = list->prev;
					}
					else
					{
						list->prev->next = NULL;
					}
					
					task_tmp->msg_id = NULL; 
				}
			}
		}
			
		task_tmp = task_tmp->next;
	}
}

void *task_getcursp()
{
	return &(task_cur->task_ctx);
}


/* 同步任务切换 */
void task_sched(void)
{
	task_struct *new_taskstruct = NULL;
	task_struct *task_curtmp = task_cur;
	
	if (FALSE == task_needsched())
	{
		return;
	}	

	new_taskstruct = task_getnextrun();
	task_cur = new_taskstruct;
	new_taskstruct->task_state = TASK_RUN;
	task_switch(&(task_curtmp->task_ctx), &(new_taskstruct->task_ctx));
	
	return;
}


void task_show(void)
{
	task_struct *task_tmp = task_head;

	ltprintf("========= task show =========\r\n");
	
	do	
	{
		ltprintf("%s   %x  %d\r\n", task_tmp->task_name, task_tmp->task_state, task_tmp->task_sleeptime);
		task_tmp = task_tmp->next;
	}
	while(task_tmp != task_head);

	ltprintf("=============================\r\n");
}


