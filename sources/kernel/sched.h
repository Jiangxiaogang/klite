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
#ifndef __SCHED_H
#define __SCHED_H

#define TCB_STATE_RUNNING    0x00
#define TCB_STATE_READY      0x01
#define TCB_STATE_SLEEP      0x02
#define TCB_STATE_WAIT       0x03
#define TCB_STATE_TIMEDWAIT  0x04
#define TCB_STATE_SUSPEND    0x80

struct tcb_list
{
    struct tcb_node *head;
    struct tcb_node *tail;
};

struct tcb_node
{
    struct tcb_node *prev;
    struct tcb_node *next;
    struct tcb      *tcb;
};

struct tcb
{
    uintptr_t        sp;
    uintptr_t        sp_min;
    uintptr_t        sp_max;
    void           (*entry)(void*);
    void            *arg;
    int              prio;
    uint32_t         time;
    uint32_t         timeout;
    uint32_t         state;
    struct tcb_node  nsched;
    struct tcb_node  nwait;
    struct tcb_list *lsched;
    struct tcb_list *lwait;
};

extern struct tcb *sched_tcb_now;
extern struct tcb *sched_tcb_new;

void sched_init(void);
void sched_lock(void);
void sched_unlock(void);
void sched_switch(void);
void sched_preempt(void);
void sched_timetick(uint32_t time);

void sched_tcb_init(struct tcb *tcb);
void sched_tcb_ready(struct tcb *tcb);
void sched_tcb_suspend(struct tcb *tcb);
void sched_tcb_resume(struct tcb *tcb);
void sched_tcb_sleep(struct tcb *tcb, uint32_t timeout);
void sched_tcb_wait(struct tcb *tcb, struct tcb_list *list);
void sched_tcb_timedwait(struct tcb *tcb, struct tcb_list *list, uint32_t timeout);
void sched_tcb_wakeup(struct tcb *tcb);
bool sched_tcb_wakeone(struct tcb_list *list);
bool sched_tcb_wakeall(struct tcb_list *list);

void sched_swi_init(struct tcb *tcb);
void sched_swi_raise(void);
void sched_swi_exit(void);

#endif
