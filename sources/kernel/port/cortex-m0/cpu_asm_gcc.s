;/******************************************************************************
;* Copyright (c) 2015-2017 jiangxiaogang<kerndev@foxmail.com>
;*
;* This file is part of KLite distribution.
;*
;* KLite is free software, you can redistribute it and/or modify it under
;* the MIT Licence.
;*
;* Permission is hereby granted, free of charge, to any person obtaining a copy
;* of this software and associated documentation files (the "Software"), to deal
;* in the Software without restriction, including without limitation the rights
;* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
;* copies of the Software, and to permit persons to whom the Software is
;* furnished to do so, subject to the following conditions:
;* 
;* The above copyright notice and this permission notice shall be included in all
;* copies or substantial portions of the Software.
;* 
;* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
;* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;* SOFTWARE.
;******************************************************************************/
    .syntax unified
    
    .equ TCB_OFFSET_SP,    0x00
    .equ TCB_OFFSET_STATE, 0x20
    
    .extern sched_tcb_now
    .extern sched_tcb_new
    
    .global cpu_irq_enable
    .global cpu_irq_disable
    .global PendSV_Handler
    
    .thumb
    .section ".text"
    .align  4
    
cpu_irq_enable:
    .fnstart
    .cantunwind
    CPSIE   I
    BX      LR
    .fnend
    
cpu_irq_disable:
    .fnstart
    .cantunwind
    CPSID   I
    BX      LR
    .fnend

PendSV_Handler:
    .fnstart
    .cantunwind
    CPSID   I
    LDR     R0, =sched_tcb_now
    LDR     R1, [R0]
    CMP R1, #0
    BEQ     POPSTACK
    PUSH    {R4-R7}
    MOV     R4, R8
    MOV     R5, R9
    MOV     R6, R10
    MOV     R7, R11
    PUSH    {R4-R7}
    MOV R2,SP
    STR     R2, [R1,#TCB_OFFSET_SP]

POPSTACK:
    LDR     R2, =sched_tcb_new
    LDR     R3, [R2]
    STR     R3, [R0]
    MOVS    R1, #0
    STR R1, [R3,#TCB_OFFSET_STATE]
    LDR     R0, [R3,#TCB_OFFSET_SP]
    MOV     SP, R0
    POP     {R4-R7}
    MOV     R8, R4
    MOV     R9, R5
    MOV     R10,R6
    MOV     R11,R7
    POP {R4-R7}
    CPSIE   I
    BX      LR
    .fnend
    
    .end
    