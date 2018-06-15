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

struct tasklet_node
{
    struct tasklet_node *prev;
    struct tasklet_node *next;
    bool   state;
    void (*func)(void *);
    void  *data;
};

struct tasklet_list
{
    struct tasklet_node *head;
    struct tasklet_node *tail;
};

static struct tasklet_list m_tasklet_list;

static void tasklet_thread(void *arg)
{
    struct tasklet_node *node;
    while(1)
    {
        cpu_irq_disable();
        node = m_tasklet_list.head;
        if(node != NULL)
        {
            cpu_irq_enable();
            node->func(node->data);
            
            cpu_irq_disable();
            node->state = false;
            list_remove(&m_tasklet_list, node);
            cpu_irq_enable();
        }
        else
        {
            sched_tcb_isr->state = TCB_STATE_SUSPEND|TCB_STATE_READY;
            cpu_irq_enable();
            sched_switch();
        }
    }
}

void tasklet_init(uint32_t stack_size)
{
    struct tcb *tcb;
    uint32_t tcb_size;
    stack_size = stack_size ? stack_size : 256;
    tcb_size = sizeof(struct tcb) + stack_size;
    tcb = heap_alloc(tcb_size);
    if(tcb != NULL)
    {
        tcb->nwait  = NULL;
        tcb->nsched = NULL;
        tcb->sp_min = (uint32_t)(tcb + 1);
        tcb->sp_max = tcb->sp_min + stack_size;
        tcb->entry  = tasklet_thread;
        tcb->arg    = NULL;
        tcb->prio   = THREAD_PRIORITY_MAX + 1;
        tcb->time   = 0;
        sched_tcb_init(tcb);
        cpu_irq_disable();
        sched_tcb_isr = tcb;
        sched_tcb_isr->state |= TCB_STATE_SUSPEND;
        cpu_irq_enable();
    }
}

tasklet_t tasklet_create(void (*func)(void *), void *data)
{
    struct tasklet_node *node;
    node = heap_alloc(sizeof(struct tasklet_node));
    if(node != NULL)
    {
        node->state = false;
        node->func  = func;
        node->data  = data;
    }
    return (tasklet_t)node;
}

void tasklet_delete(tasklet_t tasklet)
{
    heap_free(tasklet);
}

void tasklet_schedule(tasklet_t tasklet)
{
    struct tasklet_node *node;
    node = (struct tasklet_node *)tasklet;
    cpu_irq_disable();
    if(node->state == false)
    {
        node->state = true;
        list_append(&m_tasklet_list, node);
        sched_tcb_isr->state &= ~TCB_STATE_SUSPEND;
    }
    sched_preempt();
    cpu_irq_enable();
}
