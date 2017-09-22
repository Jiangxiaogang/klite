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
#include "list.h"
#include "port.h"

kthread_t kthread_create(void(*func)(void*), void* arg, uint32_t stk_size)
{
    struct tcb* tcb;
    uint32_t tcb_size;
    stk_size = stk_size ? stk_size : THREAD_DEFAULT_STKSIZE;
    tcb_size = sizeof(struct tcb) + sizeof(struct tcb_node)*2 + stk_size;
    tcb = kmem_alloc(tcb_size);
    if(tcb != NULL)
    {
        tcb->func   = func;
        tcb->arg    = arg;
        tcb->prio   = 0;
        tcb->timeout= 0;
        tcb->time   = 0;
        tcb->state  = TCB_STATE_READY;
        tcb->lwait  = NULL;
        tcb->lsched = &sched_list_ready;
        tcb->nsched = (struct tcb_node*)(tcb+1);
        tcb->nwait  = tcb->nsched+1;
        tcb->nwait->tcb  = tcb;
        tcb->nsched->tcb = tcb;
        tcb->sp_min = (uint32_t)(tcb->nwait+1);
        tcb->sp_max = tcb->sp_min+stk_size;
        cpu_tcb_init(tcb);
        ksched_lock();
        ksched_insert(&sched_list_ready, tcb->nsched);
        ksched_unlock();
    }
    return (kthread_t)tcb;
}

void kthread_destroy(kthread_t thread)
{
    struct tcb* tcb;
    tcb = (struct tcb*)thread;
    ksched_lock();
    tcb->state |= TCB_STATE_EXIT;
    if(tcb->lwait)
    {
        list_remove(tcb->lwait, tcb->nwait);
    }
    if(tcb->lsched)
    {
        list_remove(tcb->lsched, tcb->nsched);
    }
    kmem_free(tcb);
    ksched_unlock();
}

void kthread_suspend(kthread_t thread)
{
    struct tcb* tcb;
    tcb = (struct tcb*)thread;
    ksched_lock();
    tcb->state |= TCB_STATE_SUSPEND;
    if(tcb->lwait)
    {
        list_remove(tcb->lwait, tcb->nwait);
    }
    if(tcb->lsched)
    {
        list_remove(tcb->lsched, tcb->nsched);
    }
    ksched_unlock();
}

void kthread_resume(kthread_t thread)
{
    struct tcb* tcb;
    tcb = (struct tcb*)thread;
    ksched_lock();
    tcb->state &= ~TCB_STATE_SUSPEND;
    if(tcb->lwait)
    {
        ksched_insert(tcb->lwait, tcb->nwait);
    }
    if(tcb->lsched)
    {
        ksched_insert(tcb->lsched, tcb->nsched);
    }
    ksched_unlock();
}

void kthread_setprio(kthread_t thread, int prio)
{
    struct tcb* tcb;
    tcb  = (struct tcb*)thread;
    prio = (prio < THREAD_PRIORITY_MAX) ? prio : THREAD_PRIORITY_MAX;
    prio = (prio > THREAD_PRIORITY_MIN) ? prio : THREAD_PRIORITY_MIN;   
    tcb->prio = prio;
}

int kthread_getprio(kthread_t thread)
{
    struct tcb* tcb;
    tcb  = (struct tcb*)thread;
    return tcb->prio;
}

uint32_t kthread_time(kthread_t thread)
{
    struct tcb* tcb;
    tcb  = (struct tcb*)thread;
    return tcb->time;
}

void kthread_exit(void)
{
    ksched_lock();
    sched_tcb_now->state |= TCB_STATE_EXIT;
    kmem_free(sched_tcb_now);
    sched_tcb_now = NULL;
    ksched_unlock();
    ksched_execute();
}

kthread_t kthread_self(void)
{
    return (kthread_t)sched_tcb_now;
}

void kthread_sleep(uint32_t tick)
{
    struct tcb* tcb;
    if(tick != 0)
    {
        ksched_lock();
        tcb = sched_tcb_now;
        tcb->timeout = tick;
        tcb->state   = TCB_STATE_SLEEP;
        tcb->lsched  = &sched_list_sleep;
        list_append(&sched_list_sleep, tcb->nsched);
        ksched_unlock();
        ksched_execute();
    }
}
