/******************************************************************************
* kernel internal data struct.
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

#ifndef __INTERNAL_H
#define __INTERNAL_H

enum
{
	TCB_STATE_RUNNING = 0,
	TCB_STATE_READY,
	TCB_STATE_SLEEP,
	TCB_STATE_TIMEDWAIT,
	TCB_STATE_WAIT,
	TCB_STATE_EXIT,
};

#define TCB_OFFSET_STATE		(0)
#define TCB_OFFSET_SP			(4)

struct tcb
{
	uint32_t state;
	uint32_t* sp;
	void (*main)(void*);
	void* arg;
	int32_t prio;
	uint32_t sleep;
	struct tcb_node* nsched;
	struct tcb_node* nwait;
	struct tcb_list* lwait;
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

struct kobj
{
	struct tcb_node* head;
	struct tcb_node* tail;
	uint32_t data;
};

struct mcb
{
	uint32_t used;
	struct mcb* next;
};

extern struct tcb* kern_tcb_now;
extern struct tcb* kern_tcb_new;

extern void kernel_tick(void);

#endif
