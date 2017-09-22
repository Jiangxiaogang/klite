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

#define MAKE_VERSION_CODE(a,b,c)    ((a<<24)|(b<<16)|(c))

#define MEMORY_ALIGN_BYTE           (4)
#define THREAD_PRIORITY_MAX         (+127)
#define THREAD_PRIORITY_MIN         (-127)
#define THREAD_DEFAULT_STKSIZE      (256)

#define TCB_STATE_RUNNING       0x00000000
#define TCB_STATE_READY         0x00000001
#define TCB_STATE_SLEEP         0x00000002
#define TCB_STATE_WAIT          0x00000003
#define TCB_STATE_TIMEDWAIT     0x00000004
#define TCB_STATE_SWITCH        0x00000005
#define TCB_STATE_SUSPEND       0x40000000
#define TCB_STATE_EXIT          0x80000000

struct tcb
{
    uint32_t sp;
    uint32_t sp_min;
    uint32_t sp_max;
    void   (*func)(void*);
    void*    arg;
    int32_t  prio;
    uint32_t time;
    uint32_t timeout;
    uint32_t state;
    struct tcb_node* nsched;
    struct tcb_node* nwait;
    struct tcb_list* lsched;
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

struct object
{
    struct tcb_node* head;
    struct tcb_node* tail;
    uint32_t data;
};

extern struct tcb* sched_tcb_now;
extern struct tcb* sched_tcb_new;
extern struct tcb_list sched_list_sleep;
extern struct tcb_list sched_list_ready;

void kmem_init(uint32_t addr, uint32_t size);
void kernel_timetick(void);

void ksched_init(void);
void ksched_lock(void);
void ksched_unlock(void);
void ksched_execute(void);
void ksched_timetick(void);
void ksched_insert(struct tcb_list* list, struct tcb_node* node);

void kobject_init(struct object* obj);
void kobject_post(struct object* obj, struct tcb* tcb);
void kobject_wait(struct object* obj, struct tcb* tcb);
void kobject_timedwait(struct object* obj, struct tcb* tcb, uint32_t timeout);

#endif
