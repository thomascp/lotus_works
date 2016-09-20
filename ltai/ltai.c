
#include "typedef.h"
#include "msg.h"
#include "comm.h"


typedef struct ltai_state_stru
{
	UINT32 state;
	char *in_msg;
	char *suc_out_msg;
	UINT32 suc_next_state;
	char *fail_out_msg;
	UINT32 fail_next_state;
} ltai_state_stru;

#define LTAI_GREETING_STATE 0x01
#define LTAI_IDENTIFY_STATE 0x02
#define LTAI_TALK_STATE 	0x03


UINT32 ltai_cur_state = LTAI_GREETING_STATE;

ltai_state_stru ltai_state[] = 
{
	{LTAI_GREETING_STATE, 
	 "hello", 
	 "hello, who are you?", LTAI_IDENTIFY_STATE, 
	 "you have to be polite to say hello", LTAI_GREETING_STATE},
	{LTAI_IDENTIFY_STATE, 
	 "lotus", 
	 "hi, merry christmas, how are you?", LTAI_TALK_STATE, 
	 "i dont know u, bye", LTAI_GREETING_STATE}, 
	{LTAI_TALK_STATE, 
	 "fine", 
	 "lol, thomas said best wishes to you, love you forever!", LTAI_GREETING_STATE, 
	 "may be i will talk to you later", LTAI_GREETING_STATE},
};

msg *g_uart0_in_msgid = NULL;


void ltai_get_msg(char *cmd, UINT32 msg_len)
{
	if(NULL != g_uart0_in_msgid)
	{
		msg_send(g_uart0_in_msgid, cmd, msg_len);
	}
}

void ltai_machine(char *buf)
{
	UINT32 i = 0;
	UINT32 lati_num = sizeof(ltai_state)/sizeof(ltai_state[0]);

	for(i = 0; i < lati_num; i++)
	{
		if(ltai_cur_state == ltai_state[i].state)
			break;
	}

	if(i == lati_num)
	{
		ltprintf("oops!\r\n");
		ltprintf(LTAI_CMD_PROMPT);
	}

	if(0 == strcmp(buf, ltai_state[i].in_msg))
	{
		ltprintf("%s\r\n", ltai_state[i].suc_out_msg);
		ltprintf(LTAI_CMD_PROMPT);
		ltai_cur_state = ltai_state[i].suc_next_state;
	}
	else
	{
		ltprintf("%s\r\n", ltai_state[i].fail_out_msg);
		ltprintf(LTAI_CMD_PROMPT);
		ltai_cur_state = ltai_state[i].fail_next_state;
	}
}

void ltai_main()
{
	#define LTAI_RCV_BUF_LEN 100
	char rcv_buf[LTAI_RCV_BUF_LEN] = {0};
	
	g_uart0_in_msgid = msg_create(1500);
	if(NULL == g_uart0_in_msgid)
	{
		ltprintf("ltai msg_create fail\r\n");
		return;
	}

	ltprintf("=====================\r\n");
	ltprintf("==== enter ltai =====\r\n");
	ltprintf("=====================\r\n");
	ltprintf(LTAI_CMD_PROMPT);

	//turn_on_keyboard_interrupts();

	while(1)
	{
		msg_rcv(g_uart0_in_msgid, 0, rcv_buf, LTAI_RCV_BUF_LEN);

		ltai_machine(rcv_buf);
	}
}

void ltai_start()
{
	task_create("ltai", ltai_main, 0, 0, 150);
}

