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

struct tcb* sched_tcb_now;
struct tcb* sched_tcb_new;

static uint32_t sched_locked;

static struct tcb_list lsched_ready;
static struct tcb_list lsched_sleep;

//链表结点已按优先级从高至低排好序,表头为最高优先级
static void list_insert_by_prio(struct tcb_list* list, struct tcb_node* node)
{
	struct tcb_node* find;
	for(find=list->tail; find!=NULL; find=find->prev)
	{
		if(node->tcb->prio <= find->tcb->prio)
		{
			break;
		}
	}
	list_insert(list, find, node);
}

static void ksched_tcb_switch(struct tcb* tcb)
{
	sched_tcb_new = tcb;
	cpu_tcb_switch();
}

static void ksched_timer(void)
{
	struct tcb* tcb;
	struct tcb_node* next;
	struct tcb_node* node;
	
	for(node=lsched_sleep.head; node!=NULL; node=next)
	{
		next = node->next;
		tcb  = node->tcb;
		tcb->timeout--;
		if(tcb->timeout == 0)
		{
			if(tcb->lwait != NULL)
			{
				list_remove(tcb->lwait, tcb->nwait);
				tcb->lwait = NULL;
			}
			list_remove(&lsched_sleep, node);
			tcb->lsched = &lsched_ready;
			list_insert_by_prio(&lsched_ready, node);
		}
	}
}


static void ksched_preempt(void)
{
	struct tcb_node* node;
	node = lsched_ready.head;
	if(node == NULL)
	{
		return;
	}
	if(sched_tcb_now == NULL)
	{
		return;
	}
	if(sched_tcb_now != sched_tcb_new)
	{
		return;
	}
	if(node->tcb->prio >= sched_tcb_now->prio)
	{
		node->tcb->lsched = NULL;
		list_remove(&lsched_ready, node);
		sched_tcb_now->lsched= &lsched_ready;
		list_insert_by_prio(&lsched_ready, sched_tcb_now->nsched);
		ksched_tcb_switch(node->tcb);
	}
}

void ksched_tick(void)
{
	if(sched_locked == 0)
	{
		ksched_timer();
		ksched_preempt();
	}
}

void ksched_switch(void)
{
	struct tcb_node* node;
	node = lsched_ready.head;
	cpu_irq_disable();
	node->tcb->lsched = NULL;
	list_remove(&lsched_ready, node);
	ksched_tcb_switch(node->tcb);
	cpu_irq_enable();
}

void ksched_lock(void)
{
	cpu_irq_disable();
	sched_locked++;
	cpu_irq_enable();
}

void ksched_unlock(void)
{
	cpu_irq_disable();
	sched_locked--;
	cpu_irq_enable();
}

void ksched_remove(struct tcb* tcb)
{
	ksched_lock();
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

void ksched_ready(struct tcb* tcb)
{
	tcb->lsched = &lsched_ready;
	list_insert_by_prio(&lsched_ready, tcb->nsched);
}

void ksched_sleep(struct tcb* tcb, uint32_t tick)
{
	tcb->timeout = tick;
	tcb->lsched  = &lsched_sleep;
	list_append(&lsched_sleep, tcb->nsched);
}

void ksched_wait(struct tcb* tcb, void* obj)
{
	tcb->lwait = (struct tcb_list*)obj;
	list_insert_by_prio((struct tcb_list*)obj, tcb->nwait);
}

void ksched_timedwait(struct tcb* tcb, void* obj, uint32_t timeout)
{
	tcb->timeout = timeout;
	tcb->lwait   = (struct tcb_list*)obj;
	tcb->lsched  = &lsched_sleep;
	list_append(&lsched_sleep, tcb->nsched);
	list_insert_by_prio((struct tcb_list*)obj, tcb->nwait);
}

void ksched_post(struct tcb* tcb, void* obj)
{
	if(tcb->lsched)
	{
		list_remove(tcb->lsched, tcb->nsched);
	}
	list_remove(obj, tcb->nwait);
	tcb->lwait  = NULL;
	tcb->lsched = &lsched_ready;
	list_insert_by_prio(&lsched_ready, tcb->nsched);
}

void ksched_init(void)
{
	sched_locked  = 0;
	sched_tcb_now = NULL;
	sched_tcb_new = NULL;
	list_init(&lsched_ready);
	list_init(&lsched_sleep);
}
