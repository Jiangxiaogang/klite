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
#include "scheduler.h"

struct event
{
    struct tcb_node *head;
    struct tcb_node *tail;
    bool state;
    bool manual;
};

event_t event_create(bool state, bool manual)
{
    struct event *p_event;
    p_event = heap_alloc(sizeof(struct event));
    if(p_event != NULL)
    {
        p_event->head   = NULL;
        p_event->tail   = NULL;
        p_event->state  = state;
        p_event->manual = manual;
    }
    return (event_t)p_event;
}

void event_delete(event_t event)
{
    heap_free(event);
}

void event_wait(event_t event)
{
    struct event *p_event;
    p_event = (struct event *)event;
    sched_lock();
    if(p_event->state)
    {
        if(!p_event->manual)
        {
            p_event->state = false;
        }
        sched_unlock();
        return;
    }
    sched_tcb_wait(sched_tcb_now, (struct tcb_list *)p_event);
    sched_unlock();
    sched_switch();
}

bool event_timedwait(event_t event, uint32_t timeout)
{
    struct event *p_event;
    p_event = (struct event *)event;
    sched_lock();
    if(p_event->state)
    {
        if(!p_event->manual)
        {
            p_event->state = false;
        }
        sched_unlock();
        return true;
    }
    if(timeout == 0)
    {
        sched_unlock();
        return false;
    }
    sched_tcb_timedwait(sched_tcb_now, (struct tcb_list *)p_event, timeout);
    sched_unlock();
    sched_switch();
    return (sched_tcb_now->timeout != 0);
}

void event_post(event_t event)
{
    struct event *p_event;
    p_event = (struct event *)event;
    sched_lock();
    if(sched_tcb_wake_all((struct tcb_list *)p_event))
    {
        p_event->state = p_event->manual;
        sched_unlock();
        sched_preempt();
    }
    else
    {
        p_event->state = true;
        sched_unlock();
    }
}

void event_clear(event_t event)
{
    struct event *p_event;
    p_event = (struct event *)event;
    sched_lock();
    p_event->state = false;
    sched_unlock();
}
