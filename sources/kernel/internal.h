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
#ifndef __INTERNAL_H
#define __INTERNAL_H

#define MAKE_VERSION_CODE(a,b,c)	((a<<24)|(b<<16)|(c))

#define THREAD_PRIORITY_MAX	(+127)
#define THREAD_PRIORITY_MIN	(-127)
#define THREAD_STACK_SIZE	(1024)

struct tcb
{
	uint32_t* sp;
	void (*func)(void*);
	void* arg;
	int32_t prio;
	uint32_t timeout;
	struct tcb_list* lsched;
	struct tcb_node* nsched;
	struct tcb_list* lwait;
	struct tcb_node* nwait;
};

struct tcb_node
{
	struct tcb_node* prev;
	struct tcb_node* next;
	struct tcb* tcb;
};

struct tcb_list
{
	struct tcb_node* head;
	struct tcb_node* tail;
};

extern struct tcb* sched_tcb_now;
extern struct tcb* sched_tcb_new;

void ksched_init(void);
void ksched_tick(void);
void ksched_lock(void);
void ksched_unlock(void);
void ksched_switch(void);
void ksched_remove(struct tcb* tcb);
void ksched_ready(struct tcb* tcb);
void ksched_sleep(struct tcb* tcb, uint32_t time);
void ksched_post(struct tcb* tcb, void* obj);
void ksched_wait(struct tcb* tcb, void* obj);
void ksched_timedwait(struct tcb* tcb, void* obj, uint32_t timeout);

void kmem_init(uint32_t addr, uint32_t size);

void kernel_tick(void);

void cpu_os_init(void);
void cpu_os_start(void);
void cpu_os_idle(void);
void cpu_irq_enable(void);
void cpu_irq_disable(void);
void cpu_tcb_init(struct tcb* tcb, uint32_t sp_min, uint32_t sp_max);
void cpu_tcb_switch(void);

#endif
