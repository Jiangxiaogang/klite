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

#define THREAD_DEFAULT_STKSIZE  (256)
#define THREAD_DEFAULT_PRIORITY (0)

kthread_t kthread_create(void (*func)(void *), void *arg, uint32_t stk_size)
{
    struct tcb *tcb;
    struct tcb_node *node;
    uint32_t tcb_size;
    stk_size = stk_size ? stk_size : THREAD_DEFAULT_STKSIZE;
    tcb_size = sizeof(struct tcb) + sizeof(struct tcb_node) * 2 + stk_size;
    tcb = kmem_alloc(tcb_size);
    if(tcb != NULL)
    {
        node = (struct tcb_node *)(tcb + 1);
        tcb->nwait  = node++;
        tcb->nsched = node++;
        tcb->sp_min = (uint32_t)node;
        tcb->sp_max = tcb->sp_min + stk_size;
        tcb->func   = func;
        tcb->arg    = arg;
        tcb->prio   = THREAD_DEFAULT_PRIORITY;
        tcb->time   = 0;
        tcb->nwait->tcb  = tcb;
        tcb->nsched->tcb = tcb;
        sched_tcb_init(tcb);
        sched_lock();
        sched_tcb_ready(tcb);
        sched_unlock();
    }
    return (kthread_t)tcb;
}

void kthread_delete(kthread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    sched_lock();
    sched_tcb_suspend(tcb);
    kmem_free(tcb);
    sched_unlock();
}

void kthread_suspend(kthread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    sched_lock();
    sched_tcb_suspend(tcb);
    sched_unlock();
}

void kthread_resume(kthread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    sched_lock();
    sched_tcb_resume(tcb);
    sched_unlock();
}

void kthread_setprio(kthread_t thread, int prio)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    tcb->prio = prio;
}

int kthread_getprio(kthread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    return tcb->prio;
}

uint32_t kthread_time(kthread_t thread)
{
    struct tcb *tcb;
    tcb = (struct tcb *)thread;
    return tcb->time;
}

void kthread_sleep(uint32_t tick)
{
    if(tick != 0)
    {
        sched_lock();
        sched_tcb_sleep(sched_tcb_now, tick);
        sched_unlock();
        sched_switch();
    }
}

void kthread_exit(void)
{
    sched_lock();
    kmem_free(sched_tcb_now);
    sched_tcb_now = NULL;
    sched_unlock();
    sched_switch();
}

kthread_t kthread_self(void)
{
    return (kthread_t)sched_tcb_now;
}

