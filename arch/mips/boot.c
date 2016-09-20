/*
boot.c:
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
#include "arch.h"

extern void testloop(void);
extern void testloop1(void);
extern void testloop2(void);
extern void testloop3(void);
extern void * memcpy(void * dest,const void *src,unsigned int count);

typedef void (*init_func)(void);

#define UFCON0	((volatile unsigned int *)(0x50000020))

void helloworld(void){
	const char *p="helloworld\n";
	while(*p){
		*UFCON0=*p++;
	};
}

void test_mmu(void){
	const char *p="test_mmu\n";
	while(*p){
		*(volatile unsigned int *)0xd0000020=*p++;
	};
}

static init_func init[]={
	0,
};

#if (CPUARCH == ARM)
void timer_init(void){
#define TIMER_BASE  (0xd1000000)
#define TCFG0   ((volatile unsigned int *)(TIMER_BASE+0x0))
#define TCFG1   ((volatile unsigned int *)(TIMER_BASE+0x4))
#define TCON    ((volatile unsigned int *)(TIMER_BASE+0x8))
#define TCONB4  ((volatile unsigned int *)(TIMER_BASE+0x3c))
	*TCFG0|=0x800;
	*TCON&=(~(7<<20));
	*TCON|=(1<<22);
	*TCON|=(1<<21);

	*TCONB4=10000;

	*TCON|=(1<<20);
	*TCON&=~(1<<21);

	umask_int(14);
	enable_irq();
}
#elif (CPUARCH == MIPS)

void turn_on_clock_interrupts (void)
{
	asm volatile
	(
		".set mips1\n\t"
		"mfc0 $8, $12\n\t"
		"li $9, 0x08001\n\t"
		"or $8, $8, $9\n\t"
		"mtc0 $8, $12\n\t"
	);

	#if 0
	int t = get_status ();
	t |= (Status_IEc_MASK | 0x08000 /* clock interrupt line */);
	set_status (t);
	#endif
}


void timer_init(void){
	/* external clk device, configured by hardware ini file */
	unsigned int tmp = *((volatile unsigned int *)(CLOCK_ADDR + CLOCKDEV_CONTROL));
	printk("clock config is 0x%x\r\n", tmp);

	printk("clock is ready\r\n");
	*((volatile unsigned int *)(CLOCK_ADDR + CLOCKDEV_CONTROL)) = 0x03000000;
	turn_on_clock_interrupts();
	enable_irq();

	#if 0
	if(tmp == 0x01000000)
	{
		printk("clock is ready\r\n");
		*((volatile unsigned int *)(CLOCK_ADDR + CLOCKDEV_CONTROL)) = 0x03000000;
		turn_on_clock_interrupts();
		enable_irq();
	}
	#endif
}

#endif

void banner(void)
{
	 printk("system ver 1.0\r\n");
	 printk("with vmips simulator\r\n");
	 printk(" **       ********       *******  \r\n");
	 printk(" **       ********      ********  \r\n");
	 printk(" **          **        *          \r\n");
	 printk(" **          **        *          \r\n");
	 printk(" **     **   **   *  *  *******   \r\n");
	 printk(" **    *  *  **   *  *         *  \r\n");
	 printk(" **    *  *  **   *  *         *  \r\n");
	 printk(" *******  *  **   *  *  *******   \r\n");
	 printk(" ****** **   **    **   ******    \r\n");
	 printk("\r\n");
	 printk("and for the baby\r\n");
}

void plat_boot(void){
	int i;
	for(i=0;init[i];i++){
		init[i]();
	}

	#if (CPUARCH == ARM)
	memcpy((void * )0x0, (void * )0x30000000, 512);
	init_sys_mmu();
	start_mmu();
	test_mmu();
	#endif

	clk_init();
	uart_init();
	
	banner();
	
	/* start idle task */
	task_start();

	/* tick irq */
	timer_init();

	start_print_task();

	ltai_start();
	task_create("test_loop", testloop, 0, 0, 150);
	task_create("test_loop1", testloop1, 0, 0, 150);
	/*task_create("test_loop3", testloop3, 0, 0, 150);*/
	
	while(1)
	{
		extern int gcnt;
		Delay(2000);
		gcnt++;
	}
}
