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
	.syntax unified
	
	.equ TCB_OFFSET_STATE,	0
	.equ TCB_OFFSET_SP,	   	4
	
	.extern	kern_tcb_now
	.extern	kern_tcb_new
	
	.global cpu_irq_enable
	.global cpu_irq_disable
	.global PendSV_Handler
	
	.thumb
	.section ".text"
	.align  4
	
cpu_irq_disable:
	.fnstart
	.cantunwind
	CPSID 	I
	BX		LR
	.fnend
	
cpu_irq_enable:
	.fnstart
	.cantunwind
	CPSIE 	I
	BX		LR
	.fnend

PendSV_Handler:
	.fnstart
	.cantunwind
    CPSID   I
    LDR     R0, =kern_tcb_now
	LDR     R1, [R0]
	CMP		R1, #0
	BEQ     POPSTACK
    PUSH    {R4-R7}						//R8-R11不能直接入栈
	MOV     R4,R8
	MOV     R5,R9
	MOV     R6,R10
	MOV     R7,R11
	PUSH    {R4-R7}
	MOV		R2,SP
    STR     R2, [R1,#TCB_OFFSET_SP]

POPSTACK:
    LDR     R2, =kern_tcb_new
	LDR     R3, [R2]
    STR     R3, [R0]
	MOVS	R0, #0						//TCB_RUNNING
	STR		R0, [R3,#TCB_OFFSET_STATE]
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
	.fnend
	
	.end
	