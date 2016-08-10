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
	#define TCB_OFFSET_STATE		(0)
	#define TCB_OFFSET_SP			(4)

	EXTERN	kern_tcb_now
	EXTERN	kern_tcb_new
	
	PUBLIC cpu_irq_enable
	PUBLIC cpu_irq_disable
	PUBLIC PendSV_Handler
	
	SECTION .text:CODE:NOROOT(4)
	
cpu_irq_disable:
	CPSID 	I
	BX		LR
	
cpu_irq_enable:
	CPSIE 	I
	BX		LR
	
PendSV_Handler:
    CPSID   I
    LDR     R0, =kern_tcb_now
	LDR     R1, [R0]
	CBZ     R1, POPSTACK

	TST     LR,#0x10					;CHECK FPU
	IT      EQ
	VPUSHEQ	{S16-S31}
	PUSH	{LR}
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
	POP		{LR}
	TST     LR,#0x10
	IT      EQ
	VPOPEQ	{S16-S31}
	
    CPSIE   I
    BX      LR

	END
	