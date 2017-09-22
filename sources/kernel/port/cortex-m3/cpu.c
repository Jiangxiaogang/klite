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

#define NVIC_INT_CTRL (*((volatile uint32_t*)0xE000ED04))
#define PEND_INT_SET  (1<<28)

void cpu_tcb_init(struct tcb* tcb)
{
    uint32_t *sp;
    sp = (uint32_t*)(tcb->sp_max & 0xFFFFFFF8);
    
    *(--sp) = 0x01000000;               // xPSR
    *(--sp) = (uint32_t)tcb->func;      // PC
    *(--sp) = (uint32_t)kthread_exit;   // R14(LR)
    *(--sp) = 0;                        // R12
    *(--sp) = 0;                        // R3
    *(--sp) = 0;                        // R2
    *(--sp) = 0;                        // R1
    *(--sp) = (uint32_t)tcb->arg;       // R0

    *(--sp) = 0;                        // R11
    *(--sp) = 0;                        // R10
    *(--sp) = 0;                        // R9
    *(--sp) = 0;                        // R8
    *(--sp) = 0;                        // R7
    *(--sp) = 0;                        // R6
    *(--sp) = 0;                        // R5
    *(--sp) = 0;                        // R4
    tcb->sp = (uint32_t)sp;
}

void cpu_tcb_switch(void)
{
    NVIC_INT_CTRL = PEND_INT_SET;
}

void SysTick_Handler(void)
{
    kernel_timetick();
}

