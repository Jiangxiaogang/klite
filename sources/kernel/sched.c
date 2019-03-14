/******************************************************************************
* Copyright (c) 2015-2019 jiangxiaogang<kerndev@foxmail.com>
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
static struct tcb_list  sched_list_sleep;
static struct tcb_list  sched_list_ready;
static struct tcb_list  sched_list_exited;
static uint32_t         sched_time_sleep;
static uint32_t         sched_time_pend;

static void list_sorted_insert(struct tcb_list *list, struct tcb_node *node)
{
    struct tcb_node *find;
    if(node->tcb->prio > 0)
    {
        for(find = list->head; find != NULL; find = find->next)
        {
            if(find->tcb->prio < node->tcb->prio)
            {
                break;
            }
        }
        list_insert_before(list, find, node);
    }
    else
    {
        for(find = list->tail; find != NULL; find = find->prev)
        {
            if(find->tcb->prio >= node->tcb->prio)
            {
                break;
            }
        }
        list_insert_after(list, find, node);
    }
}

void sched_tcb_sort(struct tcb *tcb)
{
    if(tcb->list_wait)
    {
        list_remove(tcb->list_wait, &tcb->node_wait);
        list_sorted_insert(tcb->list_wait, &tcb->node_wait);
    }
    if(tcb->list_sched)
    {
        list_remove(tcb->list_sched, &tcb->node_sched);
        list_sorted_insert(tcb->list_sched, &tcb->node_sched);
    }
}

void sched_tcb_init(struct tcb *tcb)
{
    tcb->time = 0;
    tcb->timeout = 0;
    tcb->state = TCB_STATE_READY;
    tcb->list_wait = NULL;
    tcb->list_sched = NULL;
    cpu_tcb_init(tcb);
}

void sched_tcb_run(struct tcb *tcb)
{
    list_remove(tcb->list_sched, &tcb->node_sched);
    tcb->list_sched = NULL;
    tcb->state = TCB_STATE_RUNNING;
    if(sched_tcb_now != tcb)
    {
        sched_tcb_new = tcb;
        cpu_tcb_switch();
    }
}

void sched_tcb_ready(struct tcb *tcb)
{
    tcb->state = TCB_STATE_READY;
    tcb->list_sched = &sched_list_ready;
    list_sorted_insert(&sched_list_ready, &tcb->node_sched);
}

void sched_tcb_sleep(struct tcb *tcb, uint32_t timeout)
{
    tcb->state = TCB_STATE_SLEEP;
    tcb->timeout = timeout;
    tcb->list_sched = &sched_list_sleep;
    list_append(&sched_list_sleep, &tcb->node_sched);
    sched_time_sleep = (sched_time_sleep < timeout) ? sched_time_sleep : timeout;
}

void sched_tcb_suspend(struct tcb *tcb)
{
    tcb->state = TCB_STATE_SUSPEND;
    if(tcb->list_wait)
    {
        list_remove(tcb->list_wait, &tcb->node_wait);
    }
    if(tcb->list_sched)
    {
        list_remove(tcb->list_sched, &tcb->node_sched);
    }
}

void sched_tcb_resume(struct tcb *tcb)
{
    if(tcb->state == TCB_STATE_SUSPEND)
    {
        tcb->state = TCB_STATE_READY;
        tcb->list_sched = &sched_list_ready;
        list_sorted_insert(&sched_list_ready, &tcb->node_sched);
    }
}

void sched_tcb_exit(struct tcb *tcb)
{
    tcb->state = TCB_STATE_SUSPEND;
    list_append(&sched_list_exited, &tcb->node_sched);
}

struct tcb* sched_tcb_clean(void)
{
    struct tcb_node *node;
    node = sched_list_exited.head;
    if(node != NULL)
    {
        list_remove(&sched_list_exited, node);
        return node->tcb;
    }
    return NULL;
}

void sched_tcb_wait(struct tcb *tcb, struct tcb_list *list)
{
    tcb->state = TCB_STATE_WAIT;
    tcb->list_wait = list;
    list_sorted_insert(list, &tcb->node_wait);
}

void sched_tcb_timed_wait(struct tcb *tcb, struct tcb_list *list, uint32_t timeout)
{
    tcb->state = TCB_STATE_TIMEDWAIT;
    tcb->timeout = timeout;
    tcb->list_wait = list;
    tcb->list_sched = &sched_list_sleep;
    list_sorted_insert(list, &tcb->node_wait);
    list_append(&sched_list_sleep, &tcb->node_sched);
    sched_time_sleep = (sched_time_sleep < timeout) ? sched_time_sleep : timeout;
}

void sched_tcb_wake(struct tcb *tcb)
{
    if(tcb->list_wait)
    {
        list_remove(tcb->list_wait, &tcb->node_wait);
        tcb->list_wait = NULL;
    }
    if(tcb->list_sched)
    {
        list_remove(tcb->list_sched, &tcb->node_sched);
        tcb->list_sched = NULL;
    }
    tcb->state = TCB_STATE_READY;
    tcb->list_sched = &sched_list_ready;
    list_sorted_insert(&sched_list_ready, &tcb->node_sched);
}

bool sched_tcb_wake_one(struct tcb_list *list)
{
    struct tcb *tcb;
    if(list->head)
    {
        tcb = list->head->tcb;
        sched_tcb_wake(tcb);
        return true;
    }
    return false;
}

bool sched_tcb_wake_all(struct tcb_list *list)
{
    struct tcb *tcb;
    if(list->head)
    {
        while(list->head)
        {
            tcb = list->head->tcb;
            sched_tcb_wake(tcb);
        }
        return true;
    }
    return false;
}

void sched_time_tick(uint32_t time)
{
    struct tcb *tcb;
    struct tcb_node *node;
    struct tcb_node *next;
    cpu_irq_disable();
    if(sched_tcb_now != NULL)
    {
        sched_tcb_now->time += time;
    }
    if(sched_time_sleep > time)
    {
        sched_time_sleep -= time;
        sched_time_pend += time;
        cpu_irq_enable();
        return;
    }
    time += sched_time_pend;
    sched_time_pend = 0;
    sched_time_sleep = -1U;
    for(node = sched_list_sleep.head; node != NULL; node = next)
    {
        next = node->next;
        tcb = node->tcb;
        if(tcb->timeout > time)
        {
            tcb->timeout -= time;
            sched_time_sleep = (sched_time_sleep < tcb->timeout) ? sched_time_sleep : tcb->timeout;
        }
        else
        {
            tcb->timeout = 0;
            sched_tcb_wake(tcb);
        }
    }
    cpu_irq_enable();
}

void sched_preempt(void)
{
    struct tcb *tcb;
    cpu_irq_disable();
    if(sched_tcb_now == NULL)
    {
        cpu_irq_enable();
        return;
    }
    if(sched_tcb_now->state != TCB_STATE_RUNNING)
    {
        cpu_irq_enable();
        return;
    }
    if(sched_list_ready.head == NULL)
    {
        cpu_irq_enable();
        return;
    }
    tcb = sched_list_ready.head->tcb;
    if(tcb->prio < sched_tcb_now->prio)
    {
        cpu_irq_enable();
        return;
    }
    sched_tcb_ready(sched_tcb_now);
    sched_tcb_run(tcb);
    cpu_irq_enable();
}

void sched_switch(void)
{
    cpu_irq_disable();
    sched_tcb_run(sched_list_ready.head->tcb);
    cpu_irq_enable();
}

void sched_lock(void)
{
    cpu_irq_disable();
}

void sched_unlock(void)
{
    cpu_irq_enable();
}

void sched_idle(void)
{
    cpu_sys_idle(sched_time_sleep);
}

void sched_init(void)
{
    sched_tcb_now = NULL;
    sched_tcb_new = NULL;
    sched_time_pend = 0;
    sched_time_sleep = -1U;
    list_init(&sched_list_ready);
    list_init(&sched_list_sleep);
    list_init(&sched_list_exited);
    cpu_sys_init();
}
