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
	CBZ     R1, POPSTACK
    PUSH    {R4-R11}
    STR     SP, [R1,#TCB_OFFSET_SP]
POPSTACK:
    LDR     R2, =kern_tcb_new
	LDR     R3, [R2]
    STR     R3, [R0]
	MOV		R0, #0						//TCB_STAT_RUNNING
	STR		R0, [R3,#TCB_OFFSET_STATE]
    LDR     SP, [R3,#TCB_OFFSET_SP]
    POP     {R4-R11}
    
    CPSIE   I
    BX      LR
	.fnend
	
	.end
	