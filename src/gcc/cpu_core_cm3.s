/******************************************************************************
* lowlevel cpu arch functions of Cortex-M3
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
	#define TCB_OFFSET_STATE		(0)
	#define TCB_OFFSET_SP			(4)
	#define TCB_OFFSET_MAIN			(8)
	#define TCB_OFFSET_ARG			(12)
	#define NVIC_INT_CTRL   		(0xE000ED04)
	#define PEND_INT_SET			(1<<28)
	
	EXTERN	kern_tcb_now
	EXTERN	kern_tcb_new
	EXTERN	kernel_tick
	EXTERN	kthread_exit
	
	PUBLIC cpu_irq_enable
	PUBLIC cpu_irq_disable
	PUBLIC cpu_tcb_switch
	PUBLIC cpu_tcb_init
	PUBLIC PendSV_Handler
	PUBLIC SysTick_Handler
	
	SECTION .text:CODE:NOROOT(4)
	
cpu_irq_disable:
	CPSID 	I
	BX		LR
	
cpu_irq_enable:
	CPSIE 	I
	BX		LR
	
cpu_tcb_switch:
	LDR		R0,=NVIC_INT_CTRL
	LDR		R1,=PEND_INT_SET
	STR		R1,[R0]
	BX		LR

PendSV_Handler:
    CPSID   I
    LDR     R0, =kern_tcb_now
	LDR     R1, [R0]
	CBZ     R1, POPSTACK
    PUSH    {R4-R11}
    STR     SP, [R1,#TCB_OFFSET_SP]
POPSTACK
    LDR     R2, =kern_tcb_new
	LDR     R3, [R2]
    STR     R3, [R0]
	MOV		R0, #0						;TCB_STAT_RUNNING
	STR		R0, [R3,#TCB_OFFSET_STATE]
    LDR     SP, [R3,#TCB_OFFSET_SP]
    POP     {R4-R11}
    
    CPSIE   I
    BX      LR
	
SysTick_Handler:
	PUSH    {LR}
	LDR		R0, =kernel_tick
	BLX		R0
	POP		{LR}
	BX		LR
	
//void cpu_tcb_init(struct tcb* tcb, uint32_t sp_min, uint32_t sp_max)
cpu_tcb_init:
	PUSH    {R12}
	LSR		R12,R2,#+3
    LSL    	R12,R12,#+3
	SUB     R12,R12,#+4
//xPSR = 0x01000000
	LDR		R3,=0x01000000
	STMDB   R12!,{R3}
//PC=tcb->main
	LDR		R3,[R0,#TCB_OFFSET_MAIN]
	STMDB   R12!,{R3}
//R14(LR)=kthread_exit
	LDR		R3,=kthread_exit
	STMDB   R12!,{R3}
//R12
	MOV		R3,#0
	STMDB   R12!,{R3}
//R1-R3
	MOV		R3,#0
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
//R0
	LDR		R3,[R0,#TCB_OFFSET_ARG]
	STMDB   R12!,{R3}

//R4-R11
	MOV		R3,#0
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
	STMDB   R12!,{R3}
//tcb->sp = R12
	STR     R12,[R0, #TCB_OFFSET_SP]
	POP     {R12}
	BX		LR
	
	END
	