/******************************************************************************
* Copyright (c) 2015-2017 jiangxiaogang<kerndev@foxmail.com>
*
* This file is part of KLite distribution.
*
* KLite is free software, you can redistribute it and/or modify it under
* the MIT Licence.
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
* 
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
* 
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
******************************************************************************/
#include "kernel.h"
#include "internal.h"

#define TCB_OFFSET_SP			(0)
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
	tcb->sp = (uint32_t*)(sp_max & 0xFFFFFFF8);
	*(--tcb->sp) = 0x01000000;         			// xPSR
	*(--tcb->sp) = (uint32_t)tcb->func;			// PC
	*(--tcb->sp) = (uint32_t)kthread_exit; 		// LR
	*(--tcb->sp) = 0;         					// R12
	*(--tcb->sp) = 0;         					// R3
	*(--tcb->sp) = 0;         					// R2
	*(--tcb->sp) = 0;         					// R1
	*(--tcb->sp) = (uint32_t)tcb->arg;  		// R0
	*(--tcb->sp) = 0;         					// R7
	*(--tcb->sp) = 0;         					// R6
	*(--tcb->sp) = 0;         					// R5
	*(--tcb->sp) = 0;         					// R4
	*(--tcb->sp) = 0;         					// R11
	*(--tcb->sp) = 0;         					// R10
	*(--tcb->sp) = 0;         					// R9
	*(--tcb->sp) = 0;         					// R8
}


void cpu_tcb_switch(void)
{
	NVIC_INT_CTRL = PEND_INT_SET;
}

//CM0 ARMv6 Thumb
__asm void PendSV_Handler(void)
{
	PRESERVE8
    CPSID   I

    LDR     R0, =__cpp(&sched_tcb_now)
	LDR     R1, [R0]
	CMP		R1, #0
	BEQ     POPSTACK
    PUSH    {R4-R7}						;R8-R11不能直接入栈
	MOV     R4,R8
	MOV     R5,R9
	MOV     R6,R10
	MOV     R7,R11
	PUSH    {R4-R7}
	MOV		R2,SP
    STR     R2, [R1,#TCB_OFFSET_SP]

POPSTACK
    LDR     R2, =__cpp(&sched_tcb_new)
	LDR     R3, [R2]
    STR     R3, [R0]
    LDR     R0, [R3,#TCB_OFFSET_SP]
	MOV     SP, R0
    POP     {R4-R7}
    MOV     R8,R4
	MOV     R9,R5
	MOV     R10,R6
	MOV     R11,R7
	POP		{R4-R7}
	
    CPSIE   I
    BX      LR
	ALIGN
}

void SysTick_Handler(void)
{
	kernel_tick();
}
