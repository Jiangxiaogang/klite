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

struct tcb*      sched_tcb_now;
struct tcb*      sched_tcb_new;
struct tcb_list  sched_list_sleep;
struct tcb_list  sched_list_ready;
static uint32_t  sched_locked;

static void ksched_switch(struct tcb* tcb)
{
    tcb->state = TCB_STATE_SWITCH;
    sched_tcb_new = tcb;
    cpu_tcb_switch();
}

static void ksched_timer(void)
{
    struct tcb* tcb;
    struct tcb_node* node;
    struct tcb_node* next;
    
    for(node=sched_list_sleep.head; node!=NULL; node=next)
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
            tcb->state  = TCB_STATE_READY;
            tcb->lsched = &sched_list_ready;
            list_remove(&sched_list_sleep, node);
            ksched_insert(&sched_list_ready, node);
        }
    }
}

static void ksched_preempt(void)
{
    struct tcb_node* node;
    if(sched_tcb_now->state != TCB_STATE_RUNNING)
    {
        return;
    }
    node = sched_list_ready.head;
    if(node == NULL)
    {
        return;
    }
    if(node->tcb->prio < sched_tcb_now->prio)
    {
        return;
    }
    node->tcb->lsched = NULL;
    list_remove(&sched_list_ready, node);
    sched_tcb_now->state  = TCB_STATE_READY;
    sched_tcb_now->lsched = &sched_list_ready;
    ksched_insert(&sched_list_ready, sched_tcb_now->nsched);
    ksched_switch(node->tcb);
}

void ksched_timetick(void)
{
    if(sched_tcb_now != NULL)
    {
        sched_tcb_now->time++;
        if(sched_locked == 0)
        {
            ksched_timer();
            ksched_preempt();
        }
    }
}

void ksched_insert(struct tcb_list* list, struct tcb_node* node)
{
    struct tcb_node* find;
    for(find=list->tail; find!=NULL; find=find->prev)
    {
        if(find->tcb->prio >= node->tcb->prio)
        {
            break;
        }
    }
    list_insert_after(list, find, node);
}

void ksched_execute(void)
{
    struct tcb_node* node;
    cpu_irq_disable();
    node = sched_list_ready.head;
    node->tcb->lsched = NULL;
    list_remove(&sched_list_ready, node);
    if(sched_tcb_now != node->tcb)
    {
        ksched_switch(node->tcb);
    }
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

void ksched_init(void)
{
    sched_locked  = 0;
    sched_tcb_now = NULL;
    sched_tcb_new = NULL;
    list_init(&sched_list_ready);
    list_init(&sched_list_sleep);
}

