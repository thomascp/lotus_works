

#ifndef TASK_H
#define TASK_H

#include "typedef.h"
#include "sem.h"
#include "msg.h"

#define TASK_NAME_LEN 100
#define TASK_INIT_NUM 10
#define TASK_INIT_SPLENGTH 1024
#define TASK_INIT_SPCT 0x4e

#define TASK_RUN		0x8000
#define TASK_READY 		0x0800
#define TASK_PEND		0x0080
#define TASK_DEAD 		0x0008

#define TASK_SCHED_UNLOCK		0
#define TASK_SCHED_LOCK		1

#define TASK_INIT_RUNTIME 15


typedef UINT32 (*TASK_FUNC)(UINT32 argc, VOID **argv);

#if (CPUARCH == ARM)
typedef struct task_struct_ctx
{
	UINT32 r4;
	UINT32 r5;
	UINT32 r6;
	UINT32 r7;
	UINT32 r8;
	UINT32 r9;
	UINT32 r10;
	UINT32 r11;
	UINT32 r12;
	UINT32 r0;
	UINT32 r1;
	UINT32 r2;
	UINT32 r3;
	UINT32 sp;
	UINT32 lr;
	UINT32 cpsr;
	UINT32 pc;
	UINT32 spsr;
}task_struct_ctx;
#elif (CPUARCH == MIPS)

typedef struct task_struct_ctx
{
	UINT32 at;
	UINT32 v0;
	UINT32 v1;
	UINT32 a0;
	UINT32 a1;
	UINT32 a2;
	UINT32 a3;
	UINT32 t0;
	UINT32 t1;
	UINT32 t2;
	UINT32 t3;
	UINT32 t4;
	UINT32 t5;
	UINT32 t6;
	UINT32 t7;
	UINT32 t8;
	UINT32 t9;
	UINT32 s0;
	UINT32 s1;
	UINT32 s2;
	UINT32 s3;
	UINT32 s4;
	UINT32 s5;
	UINT32 s6;
	UINT32 s7;
	UINT32 gp;
	UINT32 sp;
	UINT32 fp;
	UINT32 ra;
	UINT32 pc;

	UINT32 Status;
	UINT32 Cause;
	UINT32 EPC;
}task_struct_ctx;

#endif

typedef struct task_struct
{
	CHAR task_name[TASK_NAME_LEN];
	
	UINT32 task_state;
	UINT32 task_priority;
	
	UINT32 task_runtime;
	UINT32 task_sleeptime;

	task_struct_ctx task_ctx;
	
	struct task_struct *next;

	sem *sem_id;
	list sem_chain;

	msg *msg_id;
	list msg_chain;
}  task_struct;


extern task_struct *task_cur;

void task_delay(UINT32 tick);
void task_sched(void);
void task_show(void);

#endif

