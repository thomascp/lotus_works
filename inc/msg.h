

#ifndef MSG_H
#define MSG_H

#include "typedef.h"



typedef struct msg
{
	UINT32 type;
	UINT32 flag;
	
	void *phead;
	void *ptail;
	UINT32 buf_len;
	void *msg_buf;

	list tasklist;
} msg;


#endif

