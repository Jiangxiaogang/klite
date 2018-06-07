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
#include "port.h"

struct event
{
    struct tcb_node *head;
    struct tcb_node *tail;
    bool state;
    bool manual;
};

kevent_t kevent_create(bool state, bool manual)
{
    struct event *obj;
    obj = kmem_alloc(sizeof(struct event));
    if(obj != NULL)
    {
        obj->head   = NULL;
        obj->tail   = NULL;
        obj->state  = state;
        obj->manual = manual;
    }
    return (kevent_t)obj;
}

void kevent_delete(kevent_t event)
{
    kmem_free(event);
}

void kevent_wait(kevent_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    if(obj->state)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
        sched_unlock();
        return;
    }
    sched_tcb_wait(sched_tcb_now, obj);
    sched_unlock();
    sched_switch();
}

bool kevent_timedwait(kevent_t event, uint32_t timeout)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    if(obj->state)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
        sched_unlock();
        return true;
    }
    if(timeout == 0)
    {
        sched_unlock();
        return false;
    }
    sched_tcb_timedwait(sched_tcb_now, obj, timeout);
    sched_unlock();
    sched_switch();
    return (sched_tcb_now->timeout != 0);
}

void kevent_set(kevent_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    obj->state = true;
    if(obj->head)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
        while(obj->head)
        {
            sched_tcb_ready(obj->head->tcb);
        }
    }
    sched_unlock();
}

void kevent_reset(kevent_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    sched_lock();
    obj->state = false;
    sched_unlock();
}

void kevent_isr_wait(kevent_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    cpu_irq_disable();
    if(obj->state)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
        cpu_irq_enable();
        return;
    }
    sched_tcb_wait(sched_tcb_now, obj);
    cpu_irq_enable();
    sched_switch();
}

bool kevent_isr_timedwait(kevent_t event, uint32_t timeout)
{
    struct event *obj;
    obj = (struct event *)event;
    cpu_irq_disable();
    if(obj->state)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
        cpu_irq_enable();
        return true;
    }
    if(timeout == 0)
    {
        cpu_irq_enable();
        return false;
    }
    sched_tcb_timedwait(sched_tcb_now, obj, timeout);
    cpu_irq_enable();
    sched_switch();
    return (sched_tcb_now->timeout != 0);
}

void kevent_isr_set(kevent_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    cpu_irq_disable();
    obj->state = true;
    if(obj->head)
    {
        if(!obj->manual)
        {
            obj->state = false;
        }
        while(obj->head)
        {
            sched_tcb_ready(obj->head->tcb);
        }
    }
    cpu_irq_enable();
}

void kevent_isr_reset(kevent_t event)
{
    struct event *obj;
    obj = (struct event *)event;
    cpu_irq_disable();
    obj->state = false;
    cpu_irq_enable();
}
