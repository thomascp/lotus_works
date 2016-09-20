/*
interrupt.c:
Copyright (C) 2009  david leels <davidontech@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see http://www.gnu.org/licenses/.
*/

#include "typedef.h"
#include "arch.h"

typedef void (*IRQ_FUNC)(void);

IRQ_FUNC irq_func[INT_MAX_NUM];

void irq_reg_func(UINT32 irq_id, IRQ_FUNC irq_func_tmp)
{
	if(irq_id < INT_MAX_NUM)
	{
		irq_func[irq_id] = irq_func_tmp;
	}
}

#if (CPUARCH == ARM)

void enable_irq(void){
	asm volatile (
		"mrs r4,cpsr\n\t"
		"bic r4,r4,#0x80\n\t"
		"msr cpsr,r4\n\t"
		:::"r4"
	);
}

void disable_irq(void){
	asm volatile (
		"mrs r4,cpsr\n\t"
		"orr r4,r4,#0x80\n\t"
		"msr cpsr,r4\n\t"
		:::"r4"
	);
}

void umask_int(unsigned int offset){
	*(volatile unsigned int *)INTMSK&=~(1<<offset);
}

void usubmask_int(unsigned int offset){
	*(volatile unsigned int *)SUBINTMASK&=~(1<<offset);
}

void int_clear_int(void){
	unsigned int tmp=(1<<(*(volatile unsigned int *)INTOFFSET));
	*(volatile unsigned int *)SRCPND|=tmp;
	*(volatile unsigned int *)INTPND|=tmp;
}

#endif

#if (CPUARCH == MIPS)

void enable_irq(void){
	asm (
		"mfc0 $8, $12\n\t"
		"li $9, 0x1\n\t"
		"or $9, $9, $8\n\t"
		"mtc0 $9, $12\n\t"
	);
}

void disable_irq(void){
	asm (
		"mfc0 $8, $12\n\t"
		"li $9, 0xfffffffe\n\t"
		"and $9, $9, $8\n\t"
		"mtc0 $9, $12\n\t"
	);
}

void int_clear_clock_int(void){
	unsigned int tmp;
	tmp = *((volatile unsigned int *)(CLOCK_ADDR + CLOCKDEV_CONTROL));
}

#endif

void common_irq_handler(void){
#if (CPUARCH == ARM)
	UINT32 irq_id = (*(volatile unsigned int *)INTOFFSET);
	unsigned int tmp = (1 << irq_id);
	//printk_int("%d\r\n",*(volatile unsigned int *)INTOFFSET);
	//*(volatile unsigned int *)SRCPND|=tmp;
	//*(volatile unsigned int *)INTPND|=tmp;
#endif

#if (CPUARCH == MIPS)
	unsigned int tmp = 0;
	UINT32 i = 0;
	UINT32 irq_id = 0;

	asm(
		"mfc0 %0, $13\n\t"
		:"=r"(tmp)
		:
	);

	for(i = 0; i < INT_MAX_NUM; i++)
	{
		if(tmp & (0x1 << (i + 8)))
		{
			break;
		}
	}

	irq_id = i;
	
#endif

	if(irq_func[irq_id] != NULL)
	{
		irq_func[irq_id]();
	}
}



void setreg(int reg, int value)
{
	*(volatile unsigned int *)reg=value;
}


int showregint = 0;
void showreg(int reg)
{
	showregint = *(volatile unsigned int *)reg;
	printk("%x\r\n", showregint);
}

