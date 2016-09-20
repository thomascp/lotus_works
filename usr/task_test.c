#include "typedef.h"
#include "error.h"
#include "task.h"

extern void printk(const char *fmt, ...);
extern void printk_int(const char *fmt, ...);
void testloop1(void);
void testloop2(void);
void testloop3(void);
sem *g_testsemid = NULL;
msg *g_testmsgid = NULL;

int gcnt = 0;

void testloop(void)
{
	int i = 0;
	g_testsemid = sem_create(0, 1);
	g_testmsgid = msg_create(0);

	//task_create("test_loop2", testloop2, 0, 0, 150);
	//task_create("test_loop3", testloop3, 0, 0, 150);
	
	while(1)
	{
		//Delay(1000);
		task_delay(2000);
		//printk("test printf %d\r\n", i++);
		ltprintf("testloop %d\r\n", gcnt++);
	}
}

void testloop1(void)
{
	int i = 0;
	
	while(1)
	{
		task_delay(1000);
		task_show();
	}
}

void testloop2(void)
{
	int i = 0;
	char msg_send_buf[100] = {0};
	
	while(1)
	{
		task_delay(200);

		
		memset(msg_send_buf, 0, 100);
		sprintf(msg_send_buf, "hello world %d", i++);

		ltprintf("send msg len %d %s\r\n", strlen(msg_send_buf)+1, msg_send_buf);
		
		msg_send(g_testmsgid, msg_send_buf, strlen(msg_send_buf)+1);
	}
}


void testloop3(void)
{
	int i = 0;
	int msg_len = 0;
	char msg_rcv_buf[100] = {0};

	while(1)
	{
		//task_delay(120);
		memset(msg_rcv_buf, 0, 100);
		msg_len = msg_rcv(g_testmsgid, 0, (void *)msg_rcv_buf, 100);

		ltprintf("rcv msg %d, %s\r\n", msg_len, msg_rcv_buf);
	}
}


