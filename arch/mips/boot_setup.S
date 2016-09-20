/* setup.S
 *
 * A simple kernel to run one function void FUNCTION(void) in uncached,
 * unmapped kernel space. This program should be the first one linked in.
 */
#include "asm_regnames.h"
#include "arch.h"

/* These values should match the values given in ld.script. */
#define MEM_BASE		0xa0000000
#define MEM_SIZE		0x1000000
#define DATA_START		MEM_BASE + (MEM_SIZE*15/16)

#define INIT_STACK_BASE	DATA_START - 4
#define NTLBENTRIES		64
#define FUNCTION 		plat_boot

	.section .startup
	.globl __start

	.ent __start
__start:
	.set noreorder
	/* Here begins the DEC PROM jump table! */
	j reset  /* 0 - reset */
	nop
    jr $31      /* 1 - exec - not implemented */
    nop
    jr $31      /* 2 - restart (halt) - not implemented */
    nop
    jr $31      /* 3 - reinit - not implemented */
    nop
    jr $31      /* 4 - reboot - not implemented */
    nop
    jr $31      /* 5 - autoboot - not implemented */
    nop
    jr $31      /* 6 - open - not implemented */
    nop
    jr $31      /* 7 - read - not implemented */
    nop
    jr $31      /* 8 - write - not implemented */
    nop
    jr $31      /* 9 - ioctl - not implemented */
    nop
    jr $31      /* 10 - close - not implemented */
    nop
    jr $31      /* 11 - lseek - not implemented */
    nop
    jr $31  /* 12 - getchar */
    nop
    jr $31  /* 13 - putchar */
    nop
    jr $31  /* 14 - showchar - not implemented */
    nop
    jr $31  /* 15 - gets */
    nop
    jr $31  /* 16 - puts */
    nop
    jr $31  /* 17 - printf */
    nop
    jr $31  /* 18 - initproto - not used */
    nop
    jr $31  /* 19 - protoenable - not used */
    nop
    jr $31  /* 20 - protodisable - not used */
    nop
    jr $31  /* 21 - getpkt - not used */
    nop
    jr $31  /* 22 - putpkt - not used */
    nop
    jr $31  /* 23 - not used */
    nop
    jr $31  /* 24 - not used */
    nop
    jr $31  /* 25 - not used */
    nop
    jr $31  /* 26 - not used */
    nop
    jr $31  /* 27 - not used */
    nop
    jr $31  /* 28 - flushcache - not used */
    nop
    jr $31  /* 29 - clearcache - not used */
    nop
    jr $31  /* 30 - saveregs - not used */
    nop
    jr $31  /* 31 - loadregs - not used */
    nop
    j lowlevelpanic  /* 32 - jumps8 - user tlb exception vector 0x100 */
    nop
    jr $31  /* 33 - getenv2 */
    nop
    jr $31  /* 34 - setenv2 */
    nop
    jr $31  /* 35 - atonum - implemented as atoi */
    nop
    jr $31  /* 36 - strcmp */
    nop
    jr $31  /* 37 - strlen */
    nop
    jr $31  /* 38 - strcpy */
    nop
    jr $31  /* 39 - strcat */
    nop
    jr $31   /* 40 - getcmd - not implemented */
    nop
    jr $31  /* 41 - getnums - not implemented */
    nop
    jr $31  /* 42 - argparse - not implemented */
    nop
    jr $31  /* 43 - help - not implemented */
    nop
    jr $31  /* 44 - dump - not implemented */
    nop
    jr $31  /* 45 - setenv */
    nop
    jr $31  /* 46 - unsetenv */
    nop
    jr $31  /* 47 - printenv */
    nop
    j lowlevelpanic  /* 48 - general exception vector 0x180 */
    nop
    jr $31  /* 49 - enable - not implemented */
    nop
    jr $31  /* 50 - disable - not implemented */
    nop
    jr $31  /* 51 - printenv */
    nop
    jr $31  /* 52 - not used */
    nop
    jr $31  /* 53 - not used */
    nop
    jr $31  /* 54 - halt - not implemented */
    nop
	.set reorder
	.end __start

	/* Very simple exception handler for prom exceptions. */
	.ent lowlevelpanic
	.set noreorder
lowlevelpanic:

	li k1, INTERRUPT_MAGIC_WORD

	.set noat
	sw at, -116(sp)
	.set at
	sw v0, -112(sp)
	sw v1, -108(sp)
	sw a0, -104(sp)
	sw a1, -100(sp)
	sw a2, -96 (sp)
	sw a3, -92 (sp)
	sw t0, -88 (sp)
	sw t1, -84 (sp)
	sw t2, -80 (sp)
	sw t3, -76 (sp)
	sw t4, -72 (sp)
	sw t5, -68 (sp)
	sw t6, -64 (sp)
	sw t7, -60 (sp)
	sw t8, -56 (sp)
	sw t9, -52 (sp)
	sw s0, -48 (sp)
	sw s1, -44 (sp)
	sw s2, -40 (sp)
	sw s3, -36 (sp)
	sw s4, -32 (sp)
	sw s5, -28 (sp)
	sw s6, -24 (sp)
	sw s7, -20 (sp)
	sw gp, -16 (sp)
	sw sp, -12 (sp)
	sw fp, -8  (sp)
	sw ra, -4  (sp)
	mfc0 v0, EPC
	nop
	sw v0, 0   (sp) /* EPC -> pc */
	nop
	addiu sp, sp, -120 
	sw sp, 108(sp)

	addiu sp, sp, -16
	jal int_isclockirq
	nop
	addiu sp, sp, 16

	ori v1, zero, 0x1
	beq v0, v1, clockirq
	nop

commonirq:
	addiu sp, sp, -16
	jal common_irq_handler
	nop
	addiu sp, sp, 16
	b retfromirq
	nop

clockirq:
	addiu sp, sp, -16
	jal task_tickdo
	nop
	addiu sp, sp, 16

	addiu sp, sp, -16
	jal int_clear_clock_int
	nop
	addiu sp, sp, 16

	addiu sp, sp, -16
	jal task_needsched
	nop
	addiu sp, sp, 16

	ori v1, zero, 0x1
	beq v0, v1, taskswichirq
	nop
	b retfromirq
	nop
	
taskswichirq:
	addiu sp, sp, -16
	jal task_getcursp
	nop
	addiu sp, sp, 16
	
	move a1, v0
	move a0, sp
	addiu a0, a0, 4
	li a2, 120

1:
	lw v0, 0(a0)
	nop
	sw v0, 0(a1)
	nop
	addiu a0, a0, 4
	addiu a1, a1, 4
	addiu a2, a2, -4
	bne a2, zero, 1b 
	nop
	mfc0 v0, Status
	nop
	sw v0, 0(a1)
	nop
	addiu a1, a1, 4
	mfc0 v0, Cause
	nop
	sw v0, 0(a1)
	nop
	addiu a1, a1, 4
	mfc0 v0, EPC
	nop
	sw v0, 0(a1)
	nop
	addiu a1, a1, 4

	addiu sp, sp, -16
	jal task_nextrun
	nop
	addiu sp, sp, 16

	move a0, v0
	lw a1, 104(a0)
	move sp, a1
	addiu a1, a1, 4
	li a2, 120

1:
	lw v0, 0(a0)
	nop
	sw v0, 0(a1)
	nop
	addiu a0, a0, 4
	addiu a1, a1, 4
	addiu a2, a2, -4
	bne a2, zero, 1b 
	nop
	lw v0, 0(a0)
	nop
	mtc0 v0, Status
	nop
	addiu a0, a0, 4
	lw v0, 0(a0)
	nop
	mtc0 v0, Cause
	nop
	addiu a0, a0, 4
	lw v0, 0(a0)
	nop
	mtc0 v0, EPC
	nop
	/*sw v0, 120(sp)*/
	addiu a0, a0, 4

	.set noat
	lw at, 4  (sp)
	.set at
	lw v0, 8  (sp)  
	lw v1, 12 (sp)  
	lw a0, 16 (sp)  
	lw a1, 20 (sp)  
	lw a2, 24 (sp)  
	lw a3, 28 (sp)  
	lw t0, 32 (sp)  
	lw t1, 36 (sp)  
	lw t2, 40 (sp)  
	lw t3, 44 (sp)  
	lw t4, 48 (sp)  
	lw t5, 52 (sp)  
	lw t6, 56 (sp)  
	lw t7, 60 (sp)  
	lw t8, 64 (sp)  
	lw t9, 68 (sp) 
	lw s0, 72 (sp)
	lw s1, 76 (sp)
	lw s2, 80 (sp)   
	lw s3, 84 (sp) 
	lw s4, 88 (sp) 
	lw s5, 92 (sp) 
	lw s6, 96 (sp) 
	lw s7, 100(sp)
	lw gp, 104(sp)   
	lw sp, 108(sp)  
	lw fp, 112(sp)  
	lw ra, 116(sp) 
	lw k0, 120(sp)
	nop
	addiu sp, sp, 120
	move k1, zero
	nop
	jr k0
	rfe

retfromirq:
	.set noat
	lw at, 4  (sp)
	.set at
	lw v0, 8  (sp)  
	lw v1, 12 (sp)  
	lw a0, 16 (sp)  
	lw a1, 20 (sp)  
	lw a2, 24 (sp)  
	lw a3, 28 (sp)  
	lw t0, 32 (sp)  
	lw t1, 36 (sp)  
	lw t2, 40 (sp)  
	lw t3, 44 (sp)  
	lw t4, 48 (sp)  
	lw t5, 52 (sp)  
	lw t6, 56 (sp)  
	lw t7, 60 (sp)  
	lw t8, 64 (sp)  
	lw t9, 68 (sp) 
	lw s0, 72 (sp)
	lw s1, 76 (sp)
	lw s2, 80 (sp)   
	lw s3, 84 (sp) 
	lw s4, 88 (sp) 
	lw s5, 92 (sp) 
	lw s6, 96 (sp) 
	lw s7, 100(sp)
	lw gp, 104(sp)   
	lw sp, 108(sp)  
	lw fp, 112(sp)  
	lw ra, 116(sp) 
	nop
	addiu sp, sp, 120
	move k1, zero
	mfc0 k0, EPC
	nop
	jr k0
	rfe

	.set reorder
	.end lowlevelpanic

	

	.globl reset
	.ent reset
reset:
/* Start by clearing everything out. */
	.set noat
	move $1, $0
	.set at
	move $2, $0
	move $3, $0
	move $4, $0
	move $5, $0
	move $6, $0
	move $7, $0
	move $8, $0
	move $9, $0
	move $10, $0
	move $11, $0
	move $12, $0
	move $13, $0
	move $14, $0
	move $15, $0
	move $16, $0
	move $17, $0
	move $18, $0
	move $19, $0
	move $20, $0
	move $21, $0
	move $22, $0
	move $23, $0
	move $24, $0
	move $25, $0
	move $26, $0
	move $27, $0
	move $28, $0
	move $29, $0
	move $30, $0
	mtc0 zero, $4
	mtc0 zero, $8
	mtc0 zero, $14

#if 0
	li t2, 100000
1:
	addiu t2,t2,-1
	bnez t2, 1b

	break 0x0

#endif

	/* Clear out the TLB. */
	li t2, NTLBENTRIES	/* t2 = TLB entry number */
	li t3, 0x00000fc0	/* t3 = (VPN 0x0, ASID 0x3f) */
1:
	addiu t2, t2, -1	/* Decrement TLB entry number */
	sll t1, t2, 8		/* Shift entry number into Index field position */
	mtc0 t1, $0		/* set Index */
	mtc0 zero, $2		/* clear EntryLo */
	mtc0 t3, $10		/* set EntryHi */
	tlbwi				/* write TLB[Index] with (EntryHi, EntryLo) */
	bnez t2, 1b		/* Go back if we're not done yet. */
	nop
	mtc0 zero, $10		/* clear EntryHi (sets effective ASID=0x0) */

	/* Set up the stack and globals pointer. */
	li sp, INIT_STACK_BASE
	la gp, _gp

	/* Copy writeable data to writeable RAM. */
	la t1, _copystart		/* t1 = beginning source address for copy */
	la t2, _copyend
	/*addiu t2, t2, 4		  t2 = one word past ending source address */
	move t3, gp			/* t3 = beginning dest address */
1:
	lw t4, 0(t1)			/* load t4 from ROM */
	sw t4, 0(t3)			/* store it in RAM */
	addiu t1, t1, 4		/* increment both pointers */
	addiu t3, t3, 4
	bne t1, t2, 1b		/* if we're not finished, loop. */
	nop

	/* Call the function. */
	jal FUNCTION

	/* Wait a minute, wait a minute, stop the execution! */
	break 0x0
	.end reset

