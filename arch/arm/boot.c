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
	helloworld,
	0,
};

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

void banner(void)
{
	 printk("system ver 0.5\r\n");
	 printk(" **       ********       *******  \r\n");
	 printk(" **       ********      ********  \r\n");
	 printk(" **          **        *          \r\n");
	 printk(" **          **        *          \r\n");
	 printk(" **     **   **   *  *  *******   \r\n");
	 printk(" **    *  *  **   *  *         *  \r\n");
	 printk(" **    *  *  **   *  *         *  \r\n");
	 printk(" *******  *  **   *  *  *******   \r\n");
	 printk(" ****** **   **    **   ******    \r\n");
}

void plat_boot(void){
	int i;
	for(i=0;init[i];i++){
		init[i]();
	}

	memcpy((void * )0x0, (void * )0x30000000, 512);

	init_sys_mmu();
	start_mmu();

	clk_init();
	uart_init();
	
	test_mmu();

	banner();
	
	/* start idle task */
	task_start();

	/* tick irq */
	timer_init();

	start_print_task();

	ltai_start();
	//task_create("test_loop", testloop, 0, 0, 150);
	/*task_create("test_loop3", testloop3, 0, 0, 150);*/
	
	while(1)
	{
	}
}
