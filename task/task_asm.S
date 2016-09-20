
/* 

	extern void task_switch(void *old_task_ctx, void *new_task_ctx); 

*/

#if (CPUARCH == ARM) 

.global	task_switch

.text
.code 32

task_switch:
	stmia r0!, {r4-r12}
	stmia r0!, {r0-r3}
	stmia r0!, {r13, r14}
	mrs r2,cpsr
	stmia r0!, {r2}
	stmia r0!, {r14}


	ldmia r1!, {r4-r12}
	mov r14, r1
	ldmia r14!, {r0-r3}
	ldmia r14!, {r13}

	sub r13, r13, #4
	stmfd r13!,{r2}
	stmfd r13!,{r1}
	mov r1, r14
	ldmia r1!, {r14}
	ldmia r1!, {r2}
	msr cpsr,r2
	ldmia r1!, {r2}
	add r13, r13, #12
	stmfd r13!,{r2}
	sub r13, r13, #8
	ldmfd r13!, {r1, r2, pc}

#endif

#if (CPUARCH == MIPS) 

#include "asm_regnames.h"

.globl task_switch

	.ent task_switch
	.set noreorder

task_switch:
	.set noat
	sw at, 0  (a0)
	.set at
	sw v0, 4  (a0)  
	sw v1, 8  (a0)  
	sw a0, 12 (a0)  
	sw a1, 16 (a0)  
	sw a2, 20 (a0)  
	sw a3, 24 (a0)  
	sw t0, 28 (a0)  
	sw t1, 32 (a0)  
	sw t2, 36 (a0)  
	sw t3, 40 (a0)  
	sw t4, 44 (a0)  
	sw t5, 48 (a0)  
	sw t6, 52 (a0)  
	sw t7, 56 (a0)  
	sw t8, 60 (a0)  
	sw t9, 64 (a0) 
	sw s0, 68 (a0)
	sw s1, 72 (a0)
	sw s2, 76 (a0)   
	sw s3, 80 (a0) 
	sw s4, 84 (a0) 
	sw s5, 88 (a0) 
	sw s6, 92 (a0) 
	sw s7, 96 (a0)
	sw gp, 100(a0)
	addiu sp, sp, -120
	sw sp, 104(a0)
	sw fp, 108(a0)
	sw ra, 112(a0)
	sw ra, 116(a0) /* ra -> pc */
	nop
	addiu a0, a0, 120
	mfc0 v0, Status
	nop
	sw v0, 0(a0)
	nop
	addiu a0, a0, 4
	mfc0 v0, Cause
	nop
	sw v0, 0(a0)
	nop
	addiu a0, a0, 4
	mfc0 v0, EPC
	nop
	sw v0, 0(a0)
	nop
	addiu a0, a0, 4

	.set noat
	lw at, 0  (a1)
	.set at
	lw v0, 4  (a1)  
	lw v1, 8  (a1)  
	lw a0, 12 (a1)  
	/*lw a1, 16 (a1) */ 
	lw a2, 20 (a1)  
	lw a3, 24 (a1)  
	lw t0, 28 (a1)  
	lw t1, 32 (a1)  
	lw t2, 36 (a1)  
	lw t3, 40 (a1)  
	lw t4, 44 (a1)  
	lw t5, 48 (a1)  
	lw t6, 52 (a1)  
	lw t7, 56 (a1)  
	lw t8, 60 (a1)  
	lw t9, 64 (a1) 
	lw s0, 68 (a1)
	lw s1, 72 (a1)
	lw s2, 76 (a1)   
	lw s3, 80 (a1) 
	lw s4, 84 (a1) 
	lw s5, 88 (a1) 
	lw s6, 92 (a1) 
	lw s7, 96 (a1)
	lw gp, 100(a1)   
	lw sp, 104(a1)
	addiu sp, sp, 120
	lw fp, 108(a1)
	lw ra, 112(a1)
	lw a3, 116(a1)  /* pc */
	nop
	addiu a1, a1, 120
	lw v0, 0  (a1)
	nop
	
	andi a2, v0, 0x4
	beq a2, zero, int_tmp
	nop
	ori v0, v0, 0x1
	
int_tmp:
	mtc0 v0, Status
	nop
	addiu a1, a1, 4
	lw v0, 0(a1)
	nop
	mtc0 v0, Cause
	nop
	addiu a1, a1, 4
	lw v0, 0(a1)
	nop
	mtc0 v0, EPC
	nop
	addiu a1, a1, 4

	jr a3
	
	.end task_switch
	.set reorder


#endif
