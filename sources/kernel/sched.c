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
#include "list.h"
#include "port.h"

struct tcb             *sched_tcb_now;
struct tcb             *sched_tcb_new;
struct tcb             *sched_tcb_isr;
static uint32_t         sched_lock_count;
static struct tcb_list  sched_list_sleep;
static struct tcb_list  sched_list_ready;

static void sched_tcb_insert(struct tcb_list *list, struct tcb_node *node)
{
    struct tcb_node *find;
    for(find=list->tail; find!=NULL; find=find->prev)
    {
        if(find->tcb->prio >= node->tcb->prio)
        {
            break;
        }
    }
    list_insert_after(list, find, node);
}

static void sched_tcb_switch(struct tcb *tcb)
{
    tcb->state    = TCB_STATE_SWITCH;
    tcb->lsched   = NULL;
    sched_tcb_new = tcb;
    cpu_tcb_switch();
}

void sched_tcb_init(struct tcb *tcb)
{
    tcb->timeout = 0;
    tcb->state   = TCB_STATE_READY;
    tcb->lwait   = NULL;
    tcb->lsched  = NULL;
    cpu_tcb_init(tcb);
}

void sched_tcb_ready(struct tcb *tcb)
{
    if(tcb->lsched)
    {
        list_remove(tcb->lsched, tcb->nsched);
    }
    tcb->state  = TCB_STATE_READY;
    tcb->lsched = &sched_list_ready;
    sched_tcb_insert(&sched_list_ready, tcb->nsched);
}

void sched_tcb_sleep(struct tcb *tcb, uint32_t timeout)
{
    tcb->state   = TCB_STATE_SLEEP;
    tcb->timeout = timeout;
    tcb->lsched  = &sched_list_sleep;
    list_append(&sched_list_sleep, tcb->nsched);
}

void sched_tcb_suspend(struct tcb *tcb)
{
    if(tcb->lwait)
    {
        list_remove(tcb->lwait, tcb->nwait);
    }
    if(tcb->lsched)
    {
        list_remove(tcb->lsched, tcb->nsched);
    }
    tcb->state |= TCB_STATE_SUSPEND;
}

void sched_tcb_resume(struct tcb *tcb)
{
    if(tcb->lwait)
    {
        sched_tcb_insert(tcb->lwait, tcb->nwait);
    }
    if(tcb->lsched)
    {
        sched_tcb_insert(tcb->lsched, tcb->nsched);
    }
    tcb->state &= ~TCB_STATE_SUSPEND;
}

void sched_timetick(uint32_t time)
{
    struct tcb *tcb;
    struct tcb_node *node;
    struct tcb_node *next;
    if(sched_tcb_now == NULL)
    {
        return;
    }
    sched_tcb_now->time += time;
    if(sched_lock_count != 0)
    {
        return;
    }
    for(node=sched_list_sleep.head; node!=NULL; node=next)
    {
        next = node->next;
        tcb  = node->tcb;
        if(tcb->timeout > time)
        {
            tcb->timeout -= time;
        }
        else
        {
            if(tcb->lwait)
            {
                list_remove(tcb->lwait, tcb->nwait);
                tcb->lwait = NULL;
            }
            tcb->timeout = 0;
            list_remove(&sched_list_sleep, tcb->nsched);
            sched_tcb_ready(tcb);
        }
    }
}


void sched_preempt(void)
{
    struct tcb *tcb;
    if(sched_lock_count != 0)
    {
        return;
    }
    if(sched_tcb_now->state != TCB_STATE_RUNNING)
    {
        return;
    }
    if(sched_tcb_isr != NULL)
    {
        if(sched_tcb_isr->state == TCB_STATE_READY)
        {
            sched_tcb_ready(sched_tcb_now);
            sched_tcb_switch(sched_tcb_isr);
            return;
        }
    }
    if(sched_list_ready.head != NULL)
    {
        tcb = sched_list_ready.head->tcb;
        if(tcb->prio >= sched_tcb_now->prio)
        {
            list_remove(&sched_list_ready, tcb->nsched);
            sched_tcb_ready(sched_tcb_now);
            sched_tcb_switch(tcb);
        }
    }
}

void sched_switch(void)
{
    struct tcb *tcb;
    cpu_irq_disable();
    if(sched_tcb_isr != NULL)
    {
        if(sched_tcb_isr->state == TCB_STATE_READY)
        {
            sched_tcb_switch(sched_tcb_isr);
            cpu_irq_enable();
            return;
        }
    }
    tcb = sched_list_ready.head->tcb;
    if(sched_tcb_now != tcb)
    {
        list_remove(&sched_list_ready, tcb->nsched);
        sched_tcb_switch(tcb);
    }
    cpu_irq_enable();
}

void sched_lock(void)
{
    cpu_irq_disable();
    sched_lock_count++;
    cpu_irq_enable();
}

void sched_unlock(void)
{
    cpu_irq_disable();
    sched_lock_count--;
    cpu_irq_enable();
}

void sched_init(void)
{
    sched_tcb_now = NULL;
    sched_tcb_new = NULL;
    sched_lock_count = 0;
    list_init(&sched_list_ready);
    list_init(&sched_list_sleep);
}
