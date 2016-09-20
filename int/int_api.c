
#include "arch.h"
#include "typedef.h"

#if (CPUARCH == ARM)

int is_inirq(void)
{
	int ret = 0;
	
	asm volatile (
		"mrs r4,cpsr\n\t"
		"mov %0, r4\n\t"
		:"=r" (ret)
		:
		:"r4"
	);

	if(0x12 == (ret & 0x1f))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

UINT32 int_disable()
{
	if(!is_inirq())
		disable_irq();
}

UINT32 int_enable()
{
	if(!is_inirq())
		enable_irq();
}

#endif


#if (CPUARCH == MIPS)

#include "asm_regnames.h"

int is_inirq(void)
{
	int ret = 0;

	asm volatile
	(
		".set mips1\n\t"
		"move %0, $27\n\t"
		:"=r" (ret)
	);

	if(INTERRUPT_MAGIC_WORD == ret)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

UINT32 int_disable()
{
	if(!is_inirq())
		disable_irq();
}

UINT32 int_enable()
{
	if(!is_inirq())
		enable_irq();
}

#endif

BOOL int_isclockirq()
{
#if (CPUARCH == ARM)
	unsigned int tmp=(*(volatile unsigned int *)INTOFFSET);
	if(tmp == 14)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
#endif

#if (CPUARCH == MIPS)

	unsigned int tmp = 0;

	asm volatile
	(
		".set mips1\n\t"
		"mfc0 %0, $13\n\t"
		:"=r"(tmp)
	);

	/* according to config file, clock connect to irq7 */
	if(tmp & 0x8000)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

#endif
}



