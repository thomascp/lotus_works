/*
sem.c:

*/

#include "typedef.h"
#include "sem.h"
#include "task.h"

#define SEM_TYPE 0x01010101

#define SEM_SRC_MAX 10
#define SEM_VALID 0x5a5a5a5a
#define SEM_INVALID 0


sem sem_src[SEM_SRC_MAX] = {0};

sem *sem_create(UINT32 init, UINT32 max)
{
	UINT32 i = 0;

	int_disable();
	
	for(i = 0; i < SEM_SRC_MAX; i++)
	{
		if(sem_src[i].flag != SEM_VALID)
		{
			break;
		}
	}

	if(i == SEM_SRC_MAX)
		return NULL;
	
	sem_src[i].type = SEM_TYPE;
	sem_src[i].flag = SEM_VALID;
	sem_src[i].max = max;
	sem_src[i].cnt = init;
	sem_src[i].tasklist.next = sem_src[i].tasklist.prev = NULL;
		
	int_enable();

	return &sem_src[i];
}


void sem_take(sem *sem_id, UINT32 delay)
{
	list *list = NULL;
	task_struct *task_tmp;
	
	if((sem_id->type != SEM_TYPE) || (sem_id->flag != SEM_VALID))
		return;

	int_disable();
	if(sem_id->cnt > 0)
	{
		sem_id->cnt--;
		int_enable();
		
		return;
	}

	task_cur->task_state = TASK_PEND;
	task_cur->task_sleeptime = delay;
	task_cur->sem_id = sem_id;
	
	list = &sem_id->tasklist;
	while(list->next != NULL)
	{
		if(task_cur == (task_tmp = (task_struct *)((UINT32)(list->next) - (UINT32)&(((task_struct *)0)->sem_chain))))
		{
			task_sched();
			
			int_enable();
			
			return;
		}
		list = list->next;
	}
	list->next = &task_cur->sem_chain;
	task_cur->sem_chain.prev = list;
	task_cur->sem_chain.next = NULL;
	
	task_sched();
	
	int_enable();
}


void sem_give(sem *sem_id)
{
	list *list;
	task_struct *task_tmp;

	if((sem_id->type != SEM_TYPE) || (sem_id->flag != SEM_VALID))
		return;

	int_disable();
	
	if(sem_id->cnt < sem_id->max)
	{
		sem_id->cnt++;
	}

	list = sem_id->tasklist.next;
	if(list != NULL)
	{
		task_tmp = (task_struct *)((UINT32)list - (UINT32)&(((task_struct *)0)->sem_chain));
		
		task_tmp->task_state = TASK_READY;
		task_tmp->task_sleeptime = 0;
		task_tmp->sem_id = NULL;

		if(list->next != NULL)
		{
			sem_id->tasklist.next = list->next;
			list->next->prev = list->prev;
		}
		else
		{
			sem_id->tasklist.next = NULL;
		}

		sem_id->cnt--;
	}

	int_enable();
}

void sem_delete(sem *sem_id)
{
	task_struct *task_tmp;
	list *list = sem_id->tasklist.next;

	int_disable();

	sem_id->flag = SEM_INVALID;
	while(list != NULL)
	{
		task_tmp = (task_struct *)((UINT32)list - (UINT32)&(((task_struct *)0)->sem_chain));
		task_tmp->sem_id = NULL;
	
		list = list->next;
	}

	int_enable();
}

