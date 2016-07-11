/******************************************************************************
* kernel source file.
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
#include "kernel.h"
#include "internal.h"
#include "list.h"
#include "cpu.h"

#define KERNEL_VERSION_CODE	0x02010002

#define MEMORY_ALIGN		4
#define THREAD_PRIORITY_MAX	127
#define THREAD_PRIORITY_MIN	-127
#define THREAD_STACK_SIZE	0x400

struct tcb* kern_tcb_now;
struct tcb* kern_tcb_new;

static uint32_t kern_mem_size;
static struct mcb* kern_mem_mcb;

static struct tcb_list kern_list_ready;
static struct tcb_list kern_list_sleep;

static uint32_t kern_tick_count;
static uint32_t kern_lock_level;

#define LOCK_LEVEL_NONE		0
#define LOCK_LEVEL_PREEMPT	1
#define LOCK_LEVEL_ALL		2

#define kernel_lock(x)		{cpu_irq_disable();kern_lock_level = (x);cpu_irq_enable();}
#define kernel_unlock()		{cpu_irq_disable();kern_lock_level =0;cpu_irq_enable();}

/******************************************************************************
* kernel helper
******************************************************************************/
//线程链表结点已按优先级从高至低排好序,表头为最高优先级
static __inline void 
list_insert_order_by_prio(struct tcb_list* list, struct tcb_node* node)
{
	struct tcb_node* find;
	for(find=list->head;find!=NULL;find=find->next)
	{
		if(find->tcb->prio < node->tcb->prio)
		{
			list_insert(list,find->prev,node);
			return;
		}
	}
	list_append(list,node);
}

static __inline void ksched_switch_this(struct tcb* tcb)
{
	kern_tcb_new = tcb;
	cpu_tcb_switch();
}

//I'm ensure kern_list_ready.head NOT NULL
static void ksched_switch_next(void)
{
	struct tcb_node* node;
	node = kern_list_ready.head;
	cpu_irq_disable();
	list_remove(&kern_list_ready,node);
	ksched_switch_this(node->tcb);
	cpu_irq_enable();
}

static void ksched_tick(void)
{
	struct tcb* tcb;
	struct tcb_node* next;
	struct tcb_node* node;
	for(node=kern_list_sleep.head; node!=NULL; node=next)
	{
		next = node->next;
		tcb  = node->tcb;
		tcb->sleep--;
		if(tcb->sleep == 0)
		{
			if(tcb->state == TCB_STATE_TIMEDWAIT)
			{
				list_remove(tcb->lwait, tcb->nwait);
			}
			tcb->state = TCB_STATE_READY;
			list_remove(&kern_list_sleep, node);
			list_insert_order_by_prio(&kern_list_ready,node);
		}
	}
}

static void ksched_preempt(void)
{
	struct tcb_node* node;
	if(kern_tcb_now == NULL)
	{
		return;
	}
	node = kern_list_ready.head;
	if(node == NULL)
	{
		return;
	}
	if(node->tcb->prio < kern_tcb_now->prio)
	{
		return;
	}
	if(kern_tcb_now->state != TCB_STATE_RUNNING)
	{
		return;
	}
	kern_tcb_now->state = TCB_STATE_READY;
	list_remove(&kern_list_ready, node);
	list_insert_order_by_prio(&kern_list_ready,kern_tcb_now->nsched);
	ksched_switch_this(node->tcb);
}

/******************************************************************************
* kernel memory management
******************************************************************************/
void kmem_init(uint32_t addr, uint32_t size)
{
	uint32_t start;
	uint32_t end;
	
	start = addr;
	end = start + size;
	start = (start+MEMORY_ALIGN-1) & (~(MEMORY_ALIGN-1));
	end = end & (~(MEMORY_ALIGN-1));
	kern_mem_size= end - start;
	
	kern_mem_mcb = (void*)start;
	kern_mem_mcb->used = sizeof(struct mcb);
	kern_mem_mcb->next = (struct mcb*)(end - sizeof(struct mcb));
	kern_mem_mcb->next->used = sizeof(struct mcb);
	kern_mem_mcb->next->next = NULL;
}

void* kmem_alloc(uint32_t size)
{
	struct mcb* mcb;
	struct mcb* tmp;
	uint32_t free;
	uint32_t need;
	void* ret;
	
	ret = NULL;
	size = (size+MEMORY_ALIGN-1) & (~(MEMORY_ALIGN-1));
	need = size + sizeof(struct mcb);
	kernel_lock(LOCK_LEVEL_PREEMPT);
	for(mcb=kern_mem_mcb; mcb->next!=NULL; mcb=mcb->next)
	{
		free = (uint32_t)mcb->next - (uint32_t)mcb - mcb->used;
		if(free >= need)
		{
			tmp = (struct mcb*)((uint32_t)mcb + mcb->used);
			tmp->next = mcb->next;
			tmp->used = need;
			mcb->next = tmp;
			ret=(void*)((uint32_t)(tmp+1));
			break;
		}
	}
	kernel_unlock();
	return ret;
}

void kmem_free(void* mem)
{
	struct mcb* mcb;
	struct mcb* prev;
	struct mcb* find;
	prev = kern_mem_mcb;
	mcb  = (struct mcb*)mem-1;

	kernel_lock(LOCK_LEVEL_PREEMPT);
	for(find=kern_mem_mcb->next; find->next!=NULL; find=find->next)
	{
		if(find == mcb)
		{
			prev->next = find->next;
			break;
		}
		prev = find;
	}
	kernel_unlock();
}

void kmem_info(uint32_t* total, uint32_t* used)
{
	struct mcb* mcb;
	*used = 0;
	*total = kern_mem_size;

	kernel_lock(LOCK_LEVEL_PREEMPT);
	for(mcb=kern_mem_mcb; mcb!=NULL; mcb=mcb->next)
	{
		*used += mcb->used;
	}
	kernel_unlock();
}

/******************************************************************************
* kernel core
******************************************************************************/
static void kernel_idle_main(void* arg)
{
	kthread_setprio(kthread_self(),THREAD_PRIORITY_MIN-1);
	for(;;)
	{
		cpu_idle();
	}
}

void kernel_init(uint32_t mem_addr, uint32_t mem_size)
{
	kern_tcb_now = NULL;
	kern_tcb_new = NULL;
	kern_tick_count=0;
	kern_lock_level=LOCK_LEVEL_NONE;
	list_init(&kern_list_ready);
	list_init(&kern_list_sleep);
	kmem_init(mem_addr,mem_size);
	kthread_create(kernel_idle_main,0,0x200);
}

void kernel_start(void)
{
	cpu_core_init();
	cpu_tick_init();
	ksched_switch_next();
	cpu_idle();
}

uint32_t kernel_version(void)
{
	return KERNEL_VERSION_CODE;
}

uint32_t kernel_time(void)
{
	return kern_tick_count;
}

void kernel_tick(void)
{	
	kern_tick_count++;
	
	if(kern_lock_level < LOCK_LEVEL_ALL)
	{
		ksched_tick();
	}
	if(kern_lock_level < LOCK_LEVEL_PREEMPT)
	{
		ksched_preempt();
	}
}

/******************************************************************************
* kernel thread
******************************************************************************/
kthread_t kthread_create(void(*func)(void*),void* arg,uint32_t stk_size)
{
	struct tcb* tcb;
	uint32_t sp_min;
	uint32_t sp_max;
	stk_size = stk_size ? stk_size : THREAD_STACK_SIZE;
	tcb = kmem_alloc(sizeof(struct tcb)+sizeof(struct tcb_node)*2+stk_size);
	if(tcb != NULL)
	{
		tcb->main  = func;
		tcb->arg   = arg;
		tcb->state = TCB_STATE_READY;
		tcb->prio  = 0;
		tcb->sleep = 0;
		tcb->lwait = NULL;
		tcb->nwait = (void*)(tcb+1);
		tcb->nsched= (void*)(tcb->nwait+1);
		tcb->nsched->tcb = tcb;
		tcb->nwait->tcb  = tcb;
		sp_min = (uint32_t)(tcb->nsched+1);
		sp_max = sp_min+stk_size;
		cpu_tcb_init(tcb,sp_min,sp_max);
		
		kernel_lock(LOCK_LEVEL_ALL);
		list_append(&kern_list_ready,tcb->nsched);
		kernel_unlock();
	}
	return (kthread_t)tcb;
}

void kthread_destroy(kthread_t thread)
{
	struct tcb* tcb;
	tcb = (struct tcb*)thread;

	switch(tcb->state)
	{
		case TCB_STATE_TIMEDWAIT:
			kernel_lock(LOCK_LEVEL_ALL);
			list_remove(tcb->lwait, tcb->nwait);
			list_remove(&kern_list_sleep, tcb->nsched);
			kernel_unlock();
			kmem_free(tcb);
			break;
		case TCB_STATE_WAIT:
			kernel_lock(LOCK_LEVEL_ALL);
			list_remove(tcb->lwait, tcb->nwait);
			kernel_unlock();
			kmem_free(tcb);
			break;
		case TCB_STATE_SLEEP:
			kernel_lock(LOCK_LEVEL_ALL);
			list_remove(&kern_list_sleep, tcb->nsched);
			kernel_unlock();
			kmem_free(tcb);
			break;
		case TCB_STATE_READY:
			kernel_lock(LOCK_LEVEL_ALL);
			list_remove(&kern_list_ready, tcb->nsched);
			kernel_unlock();
			kmem_free(tcb);
			break;
		case TCB_STATE_RUNNING:
			cpu_irq_disable();
			kmem_free(tcb);
			kern_tcb_now = NULL;
			ksched_switch_next();
			break;
		default:
			break;
	}
}

void kthread_setprio(kthread_t thread, int prio)
{
	struct tcb* tcb;
	tcb = thread;

	prio = prio < THREAD_PRIORITY_MAX ? prio : THREAD_PRIORITY_MAX;
	prio = prio > THREAD_PRIORITY_MIN ? prio : THREAD_PRIORITY_MIN;	
	tcb->prio = prio;
	if(tcb->state == TCB_STATE_READY)
	{
		kernel_lock(LOCK_LEVEL_ALL);
		list_remove(&kern_list_ready, tcb->nsched);
		list_insert_order_by_prio(&kern_list_ready,tcb->nsched);
		kernel_unlock();
	}
}

int kthread_getprio(kthread_t thread)
{
	struct tcb* tcb;
	tcb = thread;
	return tcb->prio;
}

void kthread_exit(void)
{
	kthread_destroy(kern_tcb_now);
}

kthread_t kthread_self(void)
{
	return (kthread_t)kern_tcb_now;
}

void kthread_sleep(uint32_t ms)
{
	if(ms != 0)
	{
		kern_tcb_now->sleep = ms;
		kern_tcb_now->state = TCB_STATE_SLEEP;
		kernel_lock(LOCK_LEVEL_ALL);
		list_append(&kern_list_sleep,kern_tcb_now->nsched);
		kernel_unlock();
		ksched_switch_next();
	}
}

/******************************************************************************
* kernel object
******************************************************************************/
kmutex_t kmutex_create(void)
{
	struct kobj* obj;
	obj = kmem_alloc(sizeof(struct kobj));
	if(obj != NULL)
	{
		obj->head = NULL;
		obj->tail = NULL;
		obj->data = 0;
	}
	return (kmutex_t)obj;
}

void kmutex_destroy(kmutex_t mutex)
{
	kmem_free(mutex);
}

void kmutex_lock(kmutex_t mutex)
{
	struct kobj* obj;
	obj = (struct kobj*)mutex;

	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->data == 0)
	{
		obj->data = 1;
		kernel_unlock();
	}
	else
	{
		kern_tcb_now->state = TCB_STATE_WAIT;
		kern_tcb_now->lwait = (struct tcb_list*)obj;
		list_insert_order_by_prio((struct tcb_list*)obj, kern_tcb_now->nwait);
		kernel_unlock();
		ksched_switch_next();
	}
}

void kmutex_unlock(kmutex_t mutex)
{
	struct kobj* obj;
	struct tcb* tcb;
	obj = (struct kobj*)mutex;
	
	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->head != NULL)
	{
		tcb = obj->head->tcb;
		tcb->state = TCB_STATE_READY;
		tcb->lwait = NULL;
		list_remove(obj, tcb->nwait);
		list_insert_order_by_prio(&kern_list_ready,tcb->nsched);
	}
	else
	{
		obj->data = 0;
	}
	kernel_unlock();
}


kevent_t kevent_create(int state)
{
	struct kobj* obj;
	obj = kmem_alloc(sizeof(struct kobj));
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
	struct kobj* obj;
	obj = (struct kobj*)event;
	
	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->data != 0)
	{
		obj->data = 0;
		kernel_unlock();
	}
	else
	{
		kern_tcb_now->lwait = (struct tcb_list*)obj;
		kern_tcb_now->state = TCB_STATE_WAIT;
		list_append(obj, kern_tcb_now->nwait);
		kernel_unlock();
		ksched_switch_next();
	}
}

int kevent_timedwait(kevent_t event, uint32_t timeout)
{
	struct kobj* obj;
	obj = (struct kobj*)event;
	
	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->data != 0)
	{
		obj->data = 0;
		kernel_unlock();
		return 1;
	}
	else if(timeout != 0)
	{
		kern_tcb_now->state = TCB_STATE_TIMEDWAIT;
		kern_tcb_now->sleep = timeout;
		kern_tcb_now->lwait = (struct tcb_list*)obj;
		list_append(&kern_list_sleep, kern_tcb_now->nsched);
		list_append(obj, kern_tcb_now->nwait);
		kernel_unlock();
		ksched_switch_next();
		return (kern_tcb_now->sleep != 0);
	}
	kernel_unlock();
	return 0;
}

void kevent_post(kevent_t event)
{
	struct tcb*  tcb;
	struct tcb_node* node;
	struct kobj* obj;

	obj = (struct kobj*)event;
	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->head != NULL)
	{
		for(node=obj->head;node!=NULL;node=node->next)
		{
			tcb = node->tcb;
			if(tcb->state == TCB_STATE_TIMEDWAIT)
			{
				list_remove(&kern_list_sleep, tcb->nsched);
			}
			tcb->state = TCB_STATE_READY;
			tcb->lwait = NULL;
			list_insert_order_by_prio(&kern_list_ready, tcb->nsched);
		}
		list_init(obj);
	}
	else
	{
		obj->data = 1;
	}
	kernel_unlock();
}

ksem_t ksem_create(int value)
{
	struct kobj* obj;
	obj = kmem_alloc(sizeof(struct kobj));
	if(obj != NULL)
	{
		obj->data = value;
		obj->head = NULL;
		obj->tail = NULL;
	}
	return (ksem_t)obj;
}

void ksem_destroy(ksem_t sem)
{
	kmem_free(sem);
}

void ksem_wait(ksem_t sem)
{
	struct kobj* obj;
	obj = (struct kobj*)sem;
	
	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->data != 0)
	{
		obj->data--;
		kernel_unlock();
	}
	else
	{
		kern_tcb_now->state = TCB_STATE_WAIT;
		kern_tcb_now->lwait = (struct tcb_list*)obj;
		list_insert_order_by_prio((struct tcb_list*)obj, kern_tcb_now->nwait);
		kernel_unlock();
		ksched_switch_next();
	}
}

int ksem_timedwait(ksem_t sem, uint32_t timeout)
{
	struct kobj* obj;
	obj = (struct kobj*)sem;
	
	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->data != 0)
	{
		obj->data--;
		kernel_unlock();
		return 1;
	}
	else if(timeout != 0)
	{
		kern_tcb_now->state = TCB_STATE_TIMEDWAIT;
		kern_tcb_now->sleep = timeout;
		kern_tcb_now->lwait = (struct tcb_list*)obj;
		list_insert_order_by_prio((struct tcb_list*)obj, kern_tcb_now->nwait);
		list_append(&kern_list_sleep, kern_tcb_now->nsched);
		kernel_unlock();
		ksched_switch_next();
		return (kern_tcb_now->sleep != 0);
	}
	kernel_unlock();
	return 0;
}

void ksem_post(ksem_t sem)
{
	struct kobj* obj;
	struct tcb*  tcb;
	obj = (struct kobj*)sem;
	
	kernel_lock(LOCK_LEVEL_ALL);
	if(obj->head != NULL)
	{
		tcb = obj->head->tcb;
		if(tcb->state == TCB_STATE_TIMEDWAIT)
		{
			list_remove(&kern_list_sleep, tcb->nsched);
		}
		tcb->state = TCB_STATE_READY;
		tcb->lwait = NULL;
		list_remove(obj, tcb->nwait);
		list_insert_order_by_prio(&kern_list_ready, tcb->nsched);
	}
	else
	{
		obj->data++;
	}
	kernel_unlock();
}

int ksem_getvalue(ksem_t sem)
{
	struct kobj* obj;
	obj = (struct kobj*)sem;
	return obj->data;
}
