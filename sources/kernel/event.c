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

struct event_obj
{
	struct tcb_node* head;
	struct tcb_node* tail;
	uint32_t data;
};

kevent_t kevent_create(int state)
{
	struct event_obj* obj;
	obj = kmem_alloc(sizeof(struct event_obj));
	if(obj != NULL)
	{
		obj->head = NULL;
		obj->tail = NULL;
		obj->data = state;
	}
	return (kevent_t)obj;
}

void kevent_destroy(kevent_t event)
{
	kmem_free(event);
}

void kevent_wait(kevent_t event)
{
	struct event_obj* obj;
	obj = (struct event_obj*)event;
	
	ksched_lock();
	if(obj->data != 0)
	{
		obj->data = 0;
		ksched_unlock();
		return;
	}
	ksched_wait(sched_tcb_now, obj);
	ksched_unlock();
	ksched_switch();
}

int kevent_timedwait(kevent_t event, uint32_t timeout)
{
	struct event_obj* obj;
	obj = (struct event_obj*)event;
	
	ksched_lock();
	if(obj->data != 0)
	{
		obj->data = 0;
		ksched_unlock();
		return 1;
	}
	if(timeout == 0)
	{
		ksched_unlock();
		return 0;
	}
	ksched_timedwait(sched_tcb_now, obj, timeout);
	ksched_unlock();
	ksched_switch();
	return (sched_tcb_now->timeout != 0);
}

void kevent_post(kevent_t event)
{
	struct tcb_node* node;
	struct tcb_node* next;
	struct event_obj* obj;

	obj = (struct event_obj*)event;
	
	ksched_lock();
	if(obj->head == NULL)
	{
		obj->data = 1;
		ksched_unlock();
		return;
	}
	for(node=obj->head; node!=NULL; node=next)
	{
		next = node->next;
		ksched_post(node->tcb, obj);
	}
	ksched_unlock();
}
