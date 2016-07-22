/******************************************************************************
* lowlevel cpu arch functions of Cortex-M4
* Copyright (C) 2015-2016 jiangxiaogang <kerndev@foxmail.com>
*
* This file is part of klite.
* 
* klite is free software; you can redistribute it and/or modify it under the 
* terms of the GNU Lesser General Public License as published by the Free 
* Software Foundation; either version 2.1 of the License, or (at your option) 
* any later version.
*
* klite is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.
*
* You should have received a copy of the GNU Lesser General Public
* License along with klite; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
******************************************************************************/
#include "kernel.h"
#include "internal.h"
#include "cpu.h"

#define TCB_OFFSET_STATE		(0)
#define TCB_OFFSET_SP			(4)
#define NVIC_INT_CTRL   		(*((volatile uint32_t*)0xE000ED04))
#define PEND_INT_SET			(1<<28)

__asm void cpu_irq_disable(void)
{
    CPSID   I
    BX		LR
	ALIGN
}

__asm void cpu_irq_enable(void)
{
	CPSIE	I
    BX		LR
	ALIGN
}


void cpu_tcb_init(struct tcb* tcb, uint32_t sp_min, uint32_t sp_max)
{
	tcb->sp = (uint32_t*)(sp_max & 0xFFFFFFF8);	//
	//AUTO CORE
	*(--tcb->sp) = 0x01000000;         		// xPSR(Cortex-M3 say:xPSR bit24=1)
	*(--tcb->sp) = (uint32_t)tcb->main;		// PC
	*(--tcb->sp) = (uint32_t)kthread_exit;  // R14(LR)
	*(--tcb->sp) = 0;         				// R12
	*(--tcb->sp) = 0;         				// R3
	*(--tcb->sp) = 0;         				// R2
	*(--tcb->sp) = 0;         				// R1
	*(--tcb->sp) = (uint32_t)tcb->arg;  	// R0
	
	//CORE
	*(--tcb->sp) = 0xFFFFFFF9;         		// LR(EXC_RETURN)
	*(--tcb->sp) = 0;         				// R11
	*(--tcb->sp) = 0;         				// R10
	*(--tcb->sp) = 0;         				// R9
	*(--tcb->sp) = 0;         				// R8
	*(--tcb->sp) = 0;         				// R7
	*(--tcb->sp) = 0;         				// R6
	*(--tcb->sp) = 0;         				// R5
	*(--tcb->sp) = 0;         				// R4
}

void cpu_tcb_switch(void)
{
	NVIC_INT_CTRL = PEND_INT_SET;
}

__asm void PendSV_Handler(void)
{
	PRESERVE8
    CPSID   I
    LDR     R0, =__cpp(&kern_tcb_now)
	LDR     R1, [R0]
	CBZ     R1, POPSTK

	TST     LR,#0x10					;CHECK FPU
	VPUSHEQ	{S16-S31}
    PUSH    {LR,R4-R11}
    STR     SP, [R1,#TCB_OFFSET_SP]

POPSTK
    LDR     R2, =__cpp(&kern_tcb_new)
	LDR     R3, [R2]
    STR     R3, [R0]
	MOV		R0, #0						;TCB_STAT_RUNNING
	STR		R0, [R3,#TCB_OFFSET_STATE]
	
    LDR     SP, [R3,#TCB_OFFSET_SP]
    POP     {LR,R4-R11}
	TST     LR,#0x10
	VPOPEQ	{S16-S31}
    CPSIE   I
    BX      LR
	ALIGN
}

void SysTick_Handler(void)
{
	kernel_tick();
}
