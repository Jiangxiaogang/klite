/******************************************************************************
* Copyright (c) 2015-2018 jiangxiaogang<kerndev@foxmail.com>
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
#include "sched.h"
#include "port.h"

thread_t thread_create(void (*entry)(void *), void *arg, uint32_t stack_size)
{
    struct tcb *tcb;
    uint32_t tcb_size;
    stack_size = stack_size ? stack_size : THREAD_STACK_DEFAULT;
    tcb_size   = sizeof(struct tcb) + stack_size;
    tcb        = heap_alloc(tcb_size);
    if(tcb != NULL)
    {
        tcb->sp_min     = (uintptr_t)(tcb + 1);
        tcb->sp_max     = tcb->sp_min + stack_size;
        tcb->entry      = entry;
        tcb->arg        = arg;
        tcb->prio       = THREAD_PRIORITY_DEFAULT;
        tcb->nwait.tcb  = tcb;
        tcb->nsched.tcb = tcb;
        sched_tcb_init(tcb);
        sched_lock();
        sched_tcb_ready(tcb);
        sched_unlock();
    }
    return (thread_t)tcb;
}

void thread_delete(thread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    sched_lock();
    sched_tcb_suspend(tcb);
    heap_free(tcb);
    sched_unlock();
}

void thread_suspend(thread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    sched_lock();
    sched_tcb_suspend(tcb);
    sched_unlock();
}

void thread_resume(thread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    sched_lock();
    sched_tcb_resume(tcb);
    sched_unlock();
}

void thread_setprio(thread_t thread, int prio)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    tcb->prio = prio;
}

int thread_getprio(thread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    return tcb->prio;
}

uint32_t thread_time(thread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    return tcb->time;
}

void thread_sleep(uint32_t time)
{
    if(time != 0)
    {
        sched_lock();
        sched_tcb_sleep(sched_tcb_now, time);
        sched_unlock();
        sched_switch();
    }
}

void thread_yield(void)
{
    sched_lock();
    sched_tcb_ready(sched_tcb_now);
    sched_unlock();
    sched_switch();
}

void thread_exit(void)
{
    struct tcb *tcb;
    tcb = sched_tcb_now;
    sched_lock();
    sched_tcb_now = NULL;
    sched_unlock();
    heap_free(tcb);
    sched_switch();
}

thread_t thread_self(void)
{
    return (thread_t)sched_tcb_now;
}

