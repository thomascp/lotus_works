
#ifndef COMM_H
#define COMM_H

#include "typedef.h"
#include "arch.h"

#define LTAI_CMD_PROMPT "lotus->"

void sprintf(char *print_buf, const char *fmt, ...);
void ltprintf(const char *fmt, ...);

void Uart_SendByte(int whichUart, int data);
void Uart_SendByteInt(int whichUart, int data);

//void reg_irq_func(UINT32 irq_id, IRQ_FUNC irq_func);

#endif

