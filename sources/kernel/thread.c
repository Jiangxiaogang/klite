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

kthread_t kthread_create(void(*func)(void*), void* arg, uint32_t stk_size)
{
	struct tcb* tcb;
	uint32_t sp_min;
	uint32_t sp_max;
	stk_size = stk_size ? stk_size : THREAD_STACK_SIZE;
	tcb = kmem_alloc(sizeof(struct tcb)+sizeof(struct tcb_node)*2+stk_size);
	if(tcb != NULL)
	{
		tcb->func   = func;
		tcb->arg    = arg;
		tcb->prio   = 0;
		tcb->timeout= 0;
		tcb->lwait  = NULL;
		tcb->lsched = NULL;
		tcb->nwait  = (void*)(tcb+1);
		tcb->nwait->tcb  = tcb;
		tcb->nsched= (void*)(tcb->nwait+1);
		tcb->nsched->tcb = tcb;
		sp_min = (uint32_t)(tcb->nsched+1);
		sp_max = sp_min+stk_size;
		cpu_tcb_init(tcb, sp_min, sp_max);
		ksched_lock();
		ksched_ready(tcb);
		ksched_unlock();
	}
	return (kthread_t)tcb;
}

void kthread_destroy(kthread_t thread)
{
	struct tcb* tcb;
	tcb = (struct tcb*)thread;

	if(tcb != sched_tcb_now)
	{
		ksched_lock();
		ksched_remove(tcb);
		ksched_unlock();
		kmem_free(tcb);
		return;
	}
	ksched_lock();
	kmem_free(tcb);
	sched_tcb_now = NULL;
	ksched_unlock();
	ksched_switch();
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

void kthread_exit(void)
{
	kthread_destroy(sched_tcb_now);
}

kthread_t kthread_self(void)
{
	return (kthread_t)sched_tcb_now;
}

void kthread_sleep(uint32_t tick)
{
	if(tick != 0)
	{
		ksched_lock();
		ksched_sleep(sched_tcb_now, tick);
		ksched_unlock();
		ksched_switch();
	}
}
