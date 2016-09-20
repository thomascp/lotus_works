/*
msg.c:

*/

#include "typedef.h"
#include "error.h"
#include "task.h"

#define MSG_TYPE 0x02020202

#define MSG_SRC_MAX 10
#define MSG_SRC_LEN 1024
#define MSG_VALID 0x5a5a5a5a
#define MSG_INVALID 0
#define MSG_BUF_VALID 0x12344321

char msg_src_buf[MSG_SRC_MAX][MSG_SRC_LEN] = {0};
msg msg_src[MSG_SRC_MAX] = {0};

typedef struct msg_buf_head
{
	UINT32 msg_buf_sign;
	UINT32 msg_buf_len;
} msg_buf_head;


msg *msg_create(UINT32 init)
{
	UINT32 i = 0;

	int_disable();
	
	for(i = 0; i < MSG_SRC_MAX; i++)
	{
		if(msg_src[i].flag != MSG_VALID)
		{
			break;
		}
	}

	if(i == MSG_SRC_MAX)
		return NULL;
	
	msg_src[i].type = MSG_TYPE;
	msg_src[i].flag = MSG_VALID;
	msg_src[i].phead = msg_src[i].ptail = msg_src[i].msg_buf = (void *)msg_src_buf[i];
	msg_src[i].buf_len = MSG_SRC_LEN;
	msg_src[i].tasklist.next = msg_src[i].tasklist.prev = NULL;
		
	int_enable();

	return &msg_src[i];
}




UINT32 msg_rcv(msg *msg_id, UINT32 delay, void *pbuf, UINT32 buf_len)
{
	UINT32 msg_len = 0;
	list *list = NULL;
	task_struct *task_tmp;
	msg_buf_head *pmsg_buf;
	char *pmsg_end = (char *)msg_id->msg_buf + msg_id->buf_len - 1;
	
	if((msg_id->type != MSG_TYPE) || (msg_id->flag != MSG_VALID))
		return;

	int_disable();

	if(((char *)msg_id->ptail + sizeof(msg_buf_head) - 1) <= pmsg_end)
	{
		pmsg_buf = (msg_buf_head *)msg_id->ptail;
	}
	else
	{
		pmsg_buf = (msg_buf_head *)msg_id->msg_buf;
	}

	if(pmsg_buf->msg_buf_sign == MSG_BUF_VALID)
	{
		if((msg_len = pmsg_buf->msg_buf_len) <= buf_len)
		{
			/* COPY */
			if(((char *)pmsg_buf + sizeof(msg_buf_head) + msg_len - 1) > pmsg_end)
			{
				UINT32 copy_len_first = pmsg_end - ((char *)pmsg_buf + sizeof(msg_buf_head)) + 1;
				
				memcpy(pbuf, (void *)((char *)pmsg_buf + sizeof(msg_buf_head)), copy_len_first);
				memset((void *)((char *)pmsg_buf + sizeof(msg_buf_head)), 0, copy_len_first + sizeof(msg_buf_head));
				memcpy((void *)((char *)pbuf + copy_len_first), msg_id->msg_buf, msg_len - copy_len_first);
				memset(msg_id->msg_buf, 0, msg_len - copy_len_first);
				msg_id->ptail = (void *)((char *)msg_id->msg_buf + msg_len - copy_len_first);
			}
			else
			{
				memcpy(pbuf, (void *)((char *)pmsg_buf + sizeof(msg_buf_head)), msg_len);
				memset((void *)pmsg_buf, 0, msg_len + sizeof(msg_buf_head));
				msg_id->ptail = (void *)((char *)pmsg_buf + msg_len + sizeof(msg_buf_head));
			}

			int_enable();
			
			return msg_len;
		}
		else
		{
			int_enable();
			
			return MSG_GIVEBUF_TOOSMALL;
		}
	}

	task_cur->task_state = TASK_PEND;
	task_cur->task_sleeptime = delay;
	task_cur->msg_id = msg_id;
	
	list = &msg_id->tasklist;
	while(list->next != NULL)
	{
		if(task_cur == (task_tmp = (task_struct *)((UINT32)(list->next) - (UINT32)&(((task_struct *)0)->msg_chain))))
		{
			task_sched();

			if(((char *)msg_id->ptail + sizeof(msg_buf_head) - 1) <= pmsg_end)
			{
				pmsg_buf = (msg_buf_head *)msg_id->ptail;
			}
			else
			{
				pmsg_buf = (msg_buf_head *)msg_id->msg_buf;
			}

			if(pmsg_buf->msg_buf_sign == MSG_BUF_VALID)
			{
				if((msg_len = pmsg_buf->msg_buf_len) <= buf_len)
				{
					/* COPY */
					if(((char *)pmsg_buf + sizeof(msg_buf_head) + msg_len - 1) > pmsg_end)
					{
						UINT32 copy_len_first = pmsg_end - ((char *)pmsg_buf + sizeof(msg_buf_head)) + 1;
						
						memcpy(pbuf, (void *)((char *)pmsg_buf + sizeof(msg_buf_head)), copy_len_first);
						memset((void *)((char *)pmsg_buf + sizeof(msg_buf_head)), 0, copy_len_first + sizeof(msg_buf_head));
						memcpy((void *)((char *)pbuf + copy_len_first), msg_id->msg_buf, msg_len - copy_len_first);
						memset(msg_id->msg_buf, 0, msg_len - copy_len_first);
						msg_id->ptail = (void *)((char *)msg_id->msg_buf + msg_len - copy_len_first);
					}
					else
					{
						memcpy(pbuf, (void *)((char *)pmsg_buf + sizeof(msg_buf_head)), msg_len);
						memset((void *)pmsg_buf, 0, msg_len + sizeof(msg_buf_head));
						msg_id->ptail = (void *)((char *)pmsg_buf + msg_len + sizeof(msg_buf_head));
					}

					int_enable();
					
					return msg_len;
				}
				else
				{
					int_enable();
					
					return MSG_GIVEBUF_TOOSMALL;
				}
			}

			/* should not go here */
			int_enable();
			
			return msg_len;
		}
		list = list->next;
	}
	list->next = &task_cur->msg_chain;
	task_cur->msg_chain.prev = list;
	task_cur->msg_chain.next = NULL;
	
	task_sched();

	if(((char *)msg_id->ptail + sizeof(msg_buf_head) - 1) <= pmsg_end)
	{
		pmsg_buf = (msg_buf_head *)msg_id->ptail;
	}
	else
	{
		pmsg_buf = (msg_buf_head *)msg_id->msg_buf;
	}

	if(pmsg_buf->msg_buf_sign == MSG_BUF_VALID)
	{
		if((msg_len = pmsg_buf->msg_buf_len) <= buf_len)
		{
			/* COPY */
			if(((char *)pmsg_buf + sizeof(msg_buf_head) + msg_len - 1) > pmsg_end)
			{
				UINT32 copy_len_first = pmsg_end - ((char *)pmsg_buf + sizeof(msg_buf_head)) + 1;
				
				memcpy(pbuf, (void *)((char *)pmsg_buf + sizeof(msg_buf_head)), copy_len_first);
				memset((void *)((char *)pmsg_buf + sizeof(msg_buf_head)), 0, copy_len_first + sizeof(msg_buf_head));
				memcpy((void *)((char *)pbuf + copy_len_first), msg_id->msg_buf, msg_len - copy_len_first);
				memset(msg_id->msg_buf, 0, msg_len - copy_len_first);
				msg_id->ptail = (void *)((char *)msg_id->msg_buf + msg_len - copy_len_first);
			}
			else
			{
				memcpy(pbuf, (void *)((char *)pmsg_buf + sizeof(msg_buf_head)), msg_len);
				memset((void *)pmsg_buf, 0, msg_len + sizeof(msg_buf_head));
				msg_id->ptail = (void *)((char *)pmsg_buf + msg_len + sizeof(msg_buf_head));
			}

			int_enable();
			
			return msg_len;
		}
		else
		{
			int_enable();
			
			return MSG_GIVEBUF_TOOSMALL;
		}
	}

	/* should not go here */
	int_enable();

	return msg_len;
	
}

UINT32 msg_buf_avail(msg *msg_id)
{
	if(msg_id->phead >= msg_id->ptail)
	{
		return (msg_id->buf_len - (msg_id->phead - msg_id->ptail));
	}
	else
	{
		return (msg_id->ptail - msg_id->phead);
	}
}

UINT32 msg_send(msg *msg_id, void *pbuf, UINT32 msg_len)
{
	list *list;
	task_struct *task_tmp;
	char *pmsg_end = (char *)msg_id->msg_buf + msg_id->buf_len - 1;

	if((msg_id->type != MSG_TYPE) || (msg_id->flag != MSG_VALID))
		return MSG_PARAM_INVALID;

	int_disable();

	msg_len = (msg_len + 4 )/ 4 * 4;

	/* 判断剩余空间，*2是为了保证末尾不足msg_buf_head的情况 */
	if(msg_buf_avail(msg_id) < (msg_len + 2 * sizeof(msg_buf_head)))
	{
		int_enable();

		return MSG_BUF_FULL;
	}

	/* 拷贝情况比较多 */
	/* 1.末尾无法放置，msg_buf_head，则从头开始 */
	if(((char *)msg_id->phead + sizeof(msg_buf_head) - 1) > pmsg_end)
	{
		char *ptmp = (char *)msg_id->msg_buf + sizeof(msg_buf_head);
		msg_buf_head *pmsg_buf_head = (msg_buf_head *)msg_id->msg_buf;

		pmsg_buf_head->msg_buf_sign = MSG_BUF_VALID;
		pmsg_buf_head->msg_buf_len = msg_len;

		memcpy(ptmp, pbuf, msg_len);

		msg_id->phead = (void *)(ptmp + msg_len);
	}
	/* 2.内容分段 */
	else if(((char *)msg_id->phead + sizeof(msg_buf_head) + msg_len - 1) > pmsg_end)
	{
		char *ptmp = (char *)msg_id->phead + sizeof(msg_buf_head);
		msg_buf_head *pmsg_buf_head =  (msg_buf_head *)msg_id->phead;
		int copy_len_first = pmsg_end - ((char *)msg_id->phead + sizeof(msg_buf_head)) + 1;

		pmsg_buf_head->msg_buf_sign = MSG_BUF_VALID;
		pmsg_buf_head->msg_buf_len = msg_len;

		memcpy(ptmp, pbuf, copy_len_first);
		memcpy(msg_id->msg_buf, pbuf + copy_len_first, msg_len - copy_len_first);

		msg_id->phead = (void *)((char *)msg_id->msg_buf + msg_len - copy_len_first);
	}
	/* 3.直接拷贝 */
	else
	{
		char *ptmp = (char *)msg_id->phead + sizeof(msg_buf_head);
		msg_buf_head *pmsg_buf_head =  (msg_buf_head *)msg_id->phead;

		pmsg_buf_head->msg_buf_sign = MSG_BUF_VALID;
		pmsg_buf_head->msg_buf_len = msg_len;

		memcpy(ptmp, pbuf, msg_len);

		msg_id->phead = (void *)(ptmp + msg_len);
	}

	list = msg_id->tasklist.next;
	if(list != NULL)
	{
		task_tmp = (task_struct *)((UINT32)list - (UINT32)&(((task_struct *)0)->msg_chain));
		
		task_tmp->task_state = TASK_READY;
		task_tmp->task_sleeptime = 0;
		task_tmp->msg_id = NULL;

		if(list->next != NULL)
		{
			msg_id->tasklist.next = list->next;
			list->next->prev = list->prev;
		}
		else
		{
			msg_id->tasklist.next = NULL;
		}
	}

	int_enable();

	return LOS_OK;
}



void msg_delete(msg *msg_id)
{
	task_struct *task_tmp;
	list *list = msg_id->tasklist.next;

	int_disable();

	msg_id->flag = MSG_INVALID;
	while(list != NULL)
	{
		task_tmp = (task_struct *)((UINT32)list - (UINT32)&(((task_struct *)0)->msg_chain));
		task_tmp->msg_id = NULL;
	
		list = list->next;
	}

	int_enable();
}


