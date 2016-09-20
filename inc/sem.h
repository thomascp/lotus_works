

#ifndef SEM_H
#define SEM_H

#include "typedef.h"



typedef struct sem
{
	UINT32 type;
	UINT32 flag;
	UINT32 cnt;
	UINT32 max;
	list tasklist;
} sem;


sem *sem_create(UINT32 init, UINT32 max);
void sem_take(sem *sem_id, UINT32 delay);
void sem_give(sem *sem_id);
void sem_delete(sem *sem_id);


#endif

